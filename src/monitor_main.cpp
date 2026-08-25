#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/fanotify.h>

#include "file_descriptor.hpp"

int main() {
    const int raw_fanotify_fd = ::fanotify_init(FAN_CLASS_NOTIF | FAN_CLOEXEC, O_RDONLY | O_CLOEXEC);

    if (raw_fanotify_fd == -1) {
        const int error = errno;
        std::cerr << "Cannot initialize fanotify: " << std::strerror(error) << '\n';
        return 1;
    }

    FileDescriptor fanotify_fd{raw_fanotify_fd};

    std::cout << "Fanotify queue fd: " << fanotify_fd.get() << '\n';
    return 0;
}