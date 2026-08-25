#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

#include "file_descriptor.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: miniguard <file>\n";
        return 1;
    }

    const int raw_fd = ::open(argv[1], O_RDONLY | O_CLOEXEC);

    if (raw_fd == -1) {
        const int error = errno;
        std::cerr << "Cannot open file: " << std::strerror(error) << '\n';
        return 1;
    }

    FileDescriptor file{raw_fd};

    char buffer[4096];
    std::uint64_t total_bytes = 0;
    int read_error = 0;

    while (true) {
        const ssize_t bytes_read = ::read(file.get(), buffer, sizeof(buffer));
        if (bytes_read > 0) {
            total_bytes += static_cast<std::uint64_t>(bytes_read);
            continue;
        }

        if (bytes_read == 0) {
            break;
        }

        if (errno == EINTR) {
            continue;
        }

        read_error = errno;
        break;
    }

    if (read_error != 0) {
        std::cerr << "Cannot read file: " << std::strerror(read_error) << '\n';
        return 1;
    }

    std::cout << "File size read: " << total_bytes << " bytes\n";
    return 0;
}