#include "reuss/ZmqSocket.h"
#include <fmt/format.h>
#include <stdexcept>
#include <thread>
#include <zmq.h>
namespace ur {
    
ZmqSocket::ZmqSocket(const std::string &endpoint) {
    context = zmq_ctx_new();
    socket = zmq_socket(context, ZMQ_PUB);
    if (zmq_bind(socket, endpoint.c_str()))
        throw std::runtime_error("Could not bind endpoint");

    std::this_thread::sleep_for(std::chrono::seconds(1));
    fmt::print("Publishing data on on: {}\n", endpoint);
}

void ZmqSocket::send(ImageView &img, size_t data_size) {
    auto sent = zmq_send(socket, &img.frameNumber, sizeof(img.frameNumber),
                         ZMQ_SNDMORE);
    sent += zmq_send(socket, img.data, data_size, 0);
    if (static_cast<size_t>(sent) != (data_size + sizeof(img.frameNumber))) {
        auto msg = fmt::format("Failed to send image. Sent {}  of bytes", sent);
        throw std::runtime_error(msg);
    }
}

ZmqSocket::~ZmqSocket() {
    zmq_close(socket);
    zmq_ctx_destroy(context);
}
} // namespace ur