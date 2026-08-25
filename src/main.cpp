#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

#include "file_descriptor.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: miniguard <file>\n";
        return 1;
    }

    const pid_t pid = ::getpid();

    const uid_t real_uid = ::getuid();
    const uid_t effective_uid = ::geteuid();

    const gid_t real_gid = ::getgid();
    const gid_t effective_gid = ::getegid();

    std::cout << "Process PID: " << pid << '\n';
    std::cout << "Real UID/GID: " << real_uid << '/' << real_gid << '\n';
    std::cout << "Effective UID/GID: " << effective_uid << '/' << effective_gid << '\n';

    const int raw_fd = ::open(argv[1], O_RDONLY | O_CLOEXEC);

    if (raw_fd == -1) {
        const int error = errno;
        std::cerr << "Cannot open file: " << std::strerror(error) << '\n';
        return 1;
    }

    FileDescriptor file{raw_fd};

    struct stat metadata {};

    if (::fstat(file.get(), &metadata) == -1) {
        const int error = errno;
        std::cout << "Cannot read file metadata: " << std::strerror(error) << '\n';
        return 1;
    }

    const char* file_type = "other";

    if (S_ISREG(metadata.st_mode)) {
        file_type = "regular file";
    } else if (S_ISDIR(metadata.st_mode)) {
        file_type = "directory";
    }

    std::cout << "Type: " << file_type << '\n';
    std::cout << "Inode: " << metadata.st_ino << '\n';
    std::cout << "Declared size: " << metadata.st_size << " bytes\n";
    std::cout << "Owner UID: " << metadata.st_uid << '\n';
    std::cout << "Owner GID: " << metadata.st_gid << '\n';
    std::cout << "Permissions: " << std::oct << (metadata.st_mode & 07777) << std::dec << '\n';

    if (!S_ISREG(metadata.st_mode)) {
        std::cerr << "Unsupported file type: " << file_type << '\n';
        return 1;
    }

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