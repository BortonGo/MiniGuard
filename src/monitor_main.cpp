#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <optional>
#include <poll.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/fanotify.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <unistd.h>

#include "file_descriptor.hpp"

std::optional<uid_t> read_process_filesystem_uid(pid_t pid) {
    const std::string path = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream in(path);
    if (!in.is_open()) {
        return std::nullopt;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (!line.starts_with("Uid:")) {
            continue;
        }

        std::istringstream parser{line};
        std::string mark;
        uid_t real = 0;
        uid_t effective = 0;
        uid_t saved = 0;
        uid_t filesystem = 0;

        if (!(parser >> mark >> real >> effective >> saved >> filesystem)) {
            return std::nullopt;
        }

        return filesystem;
    }

    return std::nullopt;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: miniguard-monitor <directory_path>\n";
        return 1;
    }

    const int raw_fanotify_fd =
        ::fanotify_init(FAN_CLASS_NOTIF | FAN_CLOEXEC, O_RDONLY | O_CLOEXEC);

    if (raw_fanotify_fd == -1) {
        const int error = errno;
        std::cerr << "Cannot initialize fanotify: " << std::strerror(error)
                  << '\n';
        return 1;
    }

    FileDescriptor fanotify_fd{raw_fanotify_fd};

    std::cout << "Fanotify queue fd: " << fanotify_fd.get() << '\n';

    const int mark_result =
        ::fanotify_mark(fanotify_fd.get(), FAN_MARK_ADD | FAN_MARK_ONLYDIR,
                        FAN_OPEN | FAN_EVENT_ON_CHILD, AT_FDCWD, argv[1]);

    if (mark_result == -1) {
        const int error = errno;
        std::cerr << "Cannot add fanotify mark: " << std::strerror(error)
                  << '\n';
        return 1;
    }

    std::cout << "Directory marked: " << argv[1] << '\n';

    sigset_t signal_mask{};

    if (::sigemptyset(&signal_mask) == -1) {
        const int error = errno;
        std::cerr << "Cannot initialize signal mask: " << std::strerror(error)
                  << '\n';
        return 1;
    }

    if (::sigaddset(&signal_mask, SIGINT) == -1) {
        const int error = errno;
        std::cerr << "Cannot add SIGINT to signal mask: "
                  << std::strerror(error) << '\n';
        return 1;
    }

    if (::sigprocmask(SIG_BLOCK, &signal_mask, nullptr) == -1) {
        const int error = errno;
        std::cerr << "Cannot block SIGINT: " << std::strerror(error) << '\n';
        return 1;
    }

    const int raw_signal_fd = ::signalfd(-1, &signal_mask, SFD_CLOEXEC);

    if (raw_signal_fd == -1) {
        const int error = errno;
        std::cerr << "Cannot initialize signal: " << std::strerror(error)
                  << '\n';
        return 1;
    }

    FileDescriptor signal_fd{raw_signal_fd};
    std::cout << "Signal fd: " << signal_fd.get() << '\n';

    alignas(struct fanotify_event_metadata) char buffer[4096];

    struct pollfd monitored_fds[2]{};
    monitored_fds[0].fd = fanotify_fd.get();
    monitored_fds[0].events = POLLIN;

    monitored_fds[1].fd = signal_fd.get();
    monitored_fds[1].events = POLLIN;

    bool running = true;

    while (running) {
        int poll_result = -1;

        while (true) {
            poll_result = ::poll(monitored_fds, 2, -1);

            if (poll_result >= 0) {
                break;
            }

            const int error = errno;

            if (error == EINTR) {
                continue;
            }

            std::cerr << "Cannot wait for events: " << std::strerror(error)
                      << '\n';
            return 1;
        }

        constexpr short poll_errors = POLLERR | POLLHUP | POLLNVAL;

        if ((monitored_fds[0].revents & poll_errors) != 0) {
            std::cerr << "Fanotify descriptor reported poll error\n";
            return 1;
        }

        if ((monitored_fds[1].revents & poll_errors) != 0) {
            std::cerr << "Signal descriptor reported poll error\n";
            return 1;
        }

        if ((monitored_fds[1].revents & POLLIN) != 0) {
            struct signalfd_siginfo signal_info{};

            const ssize_t signal_bytes =
                ::read(signal_fd.get(), &signal_info, sizeof(signal_info));

            if (signal_bytes == -1) {
                const int error = errno;
                std::cerr << "Cannot read signal information: "
                          << std::strerror(error) << '\n';
                return 1;
            }

            if (signal_bytes != static_cast<ssize_t>(sizeof(signal_info))) {
                std::cerr << "Incomplete signal information\n";
                return 1;
            }

            if (signal_info.ssi_signo == SIGINT) {
                running = false;
                continue;
            }
        }

        if ((monitored_fds[0].revents & POLLIN) == 0) {
            continue;
        }

        const ssize_t received_bytes =
            ::read(fanotify_fd.get(), buffer, sizeof(buffer));

        if (received_bytes == -1) {
            const int error = errno;

            if (error == EINTR) {
                continue;
            }

            std::cerr << "Cannot read fanotify queue: " << std::strerror(error)
                      << '\n';
            return 1;
        }

        if (received_bytes == 0) {
            std::cerr << "Fanotify queue closed unexpectedly\n";
            return 1;
        }

        if (received_bytes <
            static_cast<ssize_t>(sizeof(struct fanotify_event_metadata))) {
            std::cerr << "Incomplete fanotify event\n";
            return 1;
        }

        std::cout << "Received " << received_bytes
                  << " bytes from fanotify queue\n";

        auto *event =
            reinterpret_cast<struct fanotify_event_metadata *>(buffer);

        ssize_t remaining_bytes = received_bytes;

        while (FAN_EVENT_OK(event, remaining_bytes)) {
            if (event->vers != FANOTIFY_METADATA_VERSION) {
                std::cerr << "Unsupported fanotify metadata version\n";
                return 1;
            }

            if ((event->mask & FAN_Q_OVERFLOW) != 0) {
                std::cerr << "Fanotify queue overflow\n";
            } else if (event->fd < 0) {
                std::cerr << "Fanotify event has no file descriptor\n";
            } else {
                FileDescriptor event_file{event->fd};
                const std::string fd_link =
                    "/proc/self/fd/" + std::to_string(event_file.get());
                char path_buffer[4096];
                const ssize_t path_length = ::readlink(
                    fd_link.c_str(), path_buffer, sizeof(path_buffer) - 1);

                if (path_length == -1) {
                    const int error = errno;
                    std::cerr
                        << "Cannot resolve event path: " << std::strerror(error)
                        << '\n';
                } else if (path_length ==
                           static_cast<ssize_t>(sizeof(path_buffer) - 1)) {
                    std::cerr << "Event path is too long\n";
                } else {
                    path_buffer[path_length] = '\0';
                    std::cout << "Event path: " << path_buffer << '\n';
                }

                const auto process_uid =
                    read_process_filesystem_uid(event->pid);
                if (process_uid) {
                    std::cout << "Process filesystem UID: " << *process_uid
                              << '\n';
                } else {
                    std::cout << "Process filesystem UID: unavailable\n";
                }

                std::cout << "Event PID: " << event->pid << '\n';
                std::cout << "Event fd: " << event_file.get() << '\n';
                std::cout << "Event mask: 0x" << std::hex << event->mask
                          << std::dec << '\n';
            }

            event = FAN_EVENT_NEXT(event, remaining_bytes);
        }
    }

    std::cout << "Stopping miniguard-monitor\n";
    return 0;
}