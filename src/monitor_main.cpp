#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/fanotify.h>
#include <unistd.h>
#include <string>
#include <signal.h>

#include "file_descriptor.hpp"

namespace
{
    volatile sig_atomic_t stop_requested = 0;

    void handle_sigint(int)
    {
        stop_requested = 1;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: miniguard-monitor <directory_path>\n";
        return 1;
    }

    const int raw_fanotify_fd = ::fanotify_init(FAN_CLASS_NOTIF | FAN_CLOEXEC, O_RDONLY | O_CLOEXEC);

    if (raw_fanotify_fd == -1)
    {
        const int error = errno;
        std::cerr << "Cannot initialize fanotify: " << std::strerror(error) << '\n';
        return 1;
    }

    FileDescriptor fanotify_fd{raw_fanotify_fd};

    std::cout << "Fanotify queue fd: " << fanotify_fd.get() << '\n';

    const int mark_result = ::fanotify_mark(
        fanotify_fd.get(),
        FAN_MARK_ADD | FAN_MARK_ONLYDIR,
        FAN_OPEN | FAN_EVENT_ON_CHILD,
        AT_FDCWD,
        argv[1]);

    if (mark_result == -1)
    {
        const int error = errno;
        std::cerr << "Cannot add fanotify mark: " << std::strerror(error) << '\n';
        return 1;
    }

    std::cout << "Directory marked: " << argv[1] << '\n';

    struct sigaction action{};
    action.sa_handler = handle_sigint;
    action.sa_flags = 0;

    ::sigemptyset(&action.sa_mask);

    if (::sigaction(SIGINT, &action, nullptr) == -1)
    {
        const int error = errno;
        std::cerr << "Cannot install SIGINT handler: " << std::strerror(error) << '\n';
        return 1;
    }

    alignas(struct fanotify_event_metadata) char buffer[4096];

    while (stop_requested == 0)
    {
        const ssize_t received_bytes = ::read(fanotify_fd.get(), buffer, sizeof(buffer));

        if (received_bytes == -1)
        {
            const int error = errno;

            if (error == EINTR)
            {
                continue;
            }

            std::cerr << "Cannot read fanotify queue: " << std::strerror(error) << '\n';
            return 1;
        }

        if (received_bytes == 0)
        {
            std::cerr << "Fanotify queue closed unexpectedly\n";
            return 1;
        }

        if (received_bytes < static_cast<ssize_t>(sizeof(struct fanotify_event_metadata)))
        {
            std::cerr << "Incomplete fanotify event\n";
            return 1;
        }

        std::cout << "Received " << received_bytes << " bytes from fanotify queue\n";

        auto *event = reinterpret_cast<struct fanotify_event_metadata *>(buffer);

        ssize_t remaining_bytes = received_bytes;

        while (FAN_EVENT_OK(event, remaining_bytes))
        {
            if (event->vers != FANOTIFY_METADATA_VERSION)
            {
                std::cerr << "Unsupported fanotify metadata version\n";
                return 1;
            }

            if ((event->mask & FAN_Q_OVERFLOW) != 0)
            {
                std::cerr << "Fanotify queue overflow\n";
            }
            else if (event->fd < 0)
            {
                std::cerr << "Fanotify event has no file descriptor\n";
            }
            else
            {
                FileDescriptor event_file{event->fd};
                const std::string fd_link = "/proc/self/fd/" + std::to_string(event_file.get());
                char path_buffer[4096];
                const ssize_t path_length = ::readlink(fd_link.c_str(), path_buffer, sizeof(path_buffer) - 1);

                if (path_length == -1)
                {
                    const int error = errno;
                    std::cerr << "Cannot resolve event path: "
                              << std::strerror(error) << '\n';
                }
                else if (path_length == static_cast<ssize_t>(sizeof(path_buffer) - 1))
                {
                    std::cerr << "Event path is too long\n";
                }
                else
                {
                    path_buffer[path_length] = '\0';
                    std::cout << "Event path: " << path_buffer << '\n';
                }

                std::cout << "Event PID: " << event->pid << '\n';
                std::cout << "Event fd: " << event_file.get() << '\n';
                std::cout << "Event mask: 0x" << std::hex << event->mask << std::dec << '\n';
            }

            event = FAN_EVENT_NEXT(event, remaining_bytes);
        }
    }

    std::cout << "Stopping miniguard-monitor\n";
    return 0;
}