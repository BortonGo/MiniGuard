#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/fanotify.h>
#include <unistd.h>
#include <cstdint>
#include <string>

#include "file_descriptor.hpp"

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

    alignas(struct fanotify_event_metadata) char buffer[4096];
    std::uint64_t received_bytes = 0;
    int read_error = 0;

    while (true)
    {
        const ssize_t bytes_read = ::read(fanotify_fd.get(), buffer, sizeof(buffer));
        if (bytes_read > 0)
        {
            received_bytes = static_cast<std::uint64_t>(bytes_read);
            break;
        }

        if (bytes_read == 0)
        {
            break;
        }

        if (errno == EINTR)
        {
            continue;
        }

        read_error = errno;
        break;
    }

    if (read_error != 0)
    {
        std::cerr << "Cannot read fanotify queue: " << std::strerror(read_error) << '\n';
        return 1;
    }

    if (received_bytes < sizeof(struct fanotify_event_metadata))
    {
        std::cerr << "Incomplete fanotify event\n";
        return 1;
    }

    auto *event = reinterpret_cast<struct fanotify_event_metadata *>(buffer);

    if (event->vers != FANOTIFY_METADATA_VERSION)
    {
        std::cerr << "Unsupported fanotify metadata version\n";
        return 1;
    }

    if ((event->mask & FAN_Q_OVERFLOW) != 0)
    {
        std::cerr << "Fanotify queue overflow\n";
        return 1;
    }

    if (event->fd < 0)
    {
        std::cerr << "Fanotify event has no file descriptor\n";
        return 1;
    }

    std::cout << "Received " << received_bytes << " bytes from fanotify queue\n";

    FileDescriptor event_file{event->fd};
    const std::string fd_link = "/proc/self/fd/" + std::to_string(event_file.get());
    char path_buffer[4096];
    const ssize_t path_length = ::readlink(fd_link.c_str(), path_buffer, sizeof(path_buffer) - 1);

    if (path_length == -1)
    {
        const int error = errno;
        std::cerr << "Cannot resolve event path: " << std::strerror(error) << '\n';
        return 1;
    }

    if (path_length == static_cast<ssize_t>(sizeof(path_buffer) - 1))
    {
        std::cerr << "Event path is too long\n";
        return 1;
    }

    path_buffer[path_length] = '\0';

    std::cout << "Event path: " << path_buffer << '\n';
    std::cout << "Event PID: " << event->pid << '\n';
    std::cout << "Event fd: " << event_file.get() << '\n';
    std::cout << "Event mask: 0x" << std::hex << event->mask << std::dec << '\n';

    return 0;
}