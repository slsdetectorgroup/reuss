#include "reuss/ZmqSocket.h"
#include <fmt/format.h>
#include <stdexcept>
#include <thread>
#include <zmq.h>
#include <cstring>
namespace reuss {

ZmqSocket::ZmqSocket(const std::string &endpoint, int zmq_hwm) {
    context = zmq_ctx_new();
    socket = zmq_socket(context, ZMQ_PUB);
    fmt::print("Setting ZMQ_SNDHWM to {}\n", zmq_hwm);
    int rc = zmq_setsockopt(socket, ZMQ_SNDHWM, &zmq_hwm, sizeof(zmq_hwm)); 
    if (rc)
        throw std::runtime_error(fmt::format("Could not set ZMQ_SNDHWM: {}", strerror(errno)));
    int bufsize = 1024*1024*zmq_hwm;
    fmt::print("Setting ZMQ_SNDBUF to: {} MB\n", bufsize/(1024*1024));
    rc = zmq_setsockopt(socket, ZMQ_SNDBUF, &bufsize, sizeof(bufsize));
    if (rc)
        throw std::runtime_error(fmt::format("Could not set ZMQ_SNDBUF: {}", strerror(errno)));


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
} // namespace reuss