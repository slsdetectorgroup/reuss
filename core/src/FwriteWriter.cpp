#include "FwriteWriter.h"
#include <fcntl.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <stdexcept>
#include <unistd.h>

void FwriteWriter::open(const std::string &fname) {
    fd_ = fopen(fname.c_str(), "wb");
    if (fd_ == nullptr) {
        throw std::runtime_error(fmt::format("fderror: {}\n", strerror(errno)));
    }
}

void FwriteWriter::close() {
    fclose(fd_);
    fd_ = nullptr;
}

void FwriteWriter::write(void *buf, size_t len) {
    int rc = fwrite(buf, len, 1, fd_);
    if (rc != 1) {
        throw std::runtime_error(fmt::format("Failed write, got {}", rc));
    }
}