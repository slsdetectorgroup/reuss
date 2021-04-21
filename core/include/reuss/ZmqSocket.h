#pragma once
#include "reuss/ImageView.h"
#include "reuss/ZmqSocket.h"
#include <cstddef>
#include <string>

namespace ur {
class ZmqSocket {
    void *context{nullptr};
    void *socket{nullptr};

  public:
    explicit ZmqSocket(const std::string &endpoint);
    ~ZmqSocket();
    void send(ImageView &img, size_t data_size);
};
} // namespace ur