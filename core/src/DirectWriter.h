#pragma once
#include <cstddef>
#include <string>
class DirectWriter {
    int fd_ = -1;

  public:
    void write(void *buf, size_t len);
    void open(const std::string &fname);
    void close();
};