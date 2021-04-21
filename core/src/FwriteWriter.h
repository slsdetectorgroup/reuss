#pragma once
#include <cstddef>
#include <string>
class FwriteWriter {
    FILE *fd_ = nullptr;

  public:
    void write(void *buf, size_t len);
    void open(const std::string &fname);
    void close();
};