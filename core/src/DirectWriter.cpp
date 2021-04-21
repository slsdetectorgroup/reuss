#include "DirectWriter.h"
#include <fcntl.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <stdexcept>
#include <unistd.h>

void DirectWriter::open(const std::string &fname) {
    fd_ = ::open(fname.c_str(), O_RDWR | O_CREAT | O_TRUNC | O_DIRECT,
                 O_CLOEXEC, S_IRWXU | S_IROTH);
    if (fd_ == -1) {
        throw std::runtime_error(fmt::format("fderror: {}\n", strerror(errno)));
    }
}

void DirectWriter::close() {
    ::close(fd_);
    fd_ = -1;
}

void DirectWriter::write(void *buf, size_t len) {
    int rc = ::write(fd_, buf, len);
    if (rc != len) {
        throw std::runtime_error("Failed write");
    }
}