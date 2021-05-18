#pragma once
#include <cstddef>
#include <string>
#include <vector>
namespace reuss {

class ZmqReceiver {
    std::string endpoint;
    uint64_t zmq_hwm = 1000;
    int timeout = -1; // wait forever
    void *context = nullptr;
    void *socket = nullptr;

  public:
    ZmqReceiver(const std::string &endpoint);
    ~ZmqReceiver();

    void connect();
    void disconnect();
    int receive_into(int n_frames, int64_t *frame_numbers, std::byte *data);
    void set_zmq_hwm(uint64_t hwm);
    void set_timeout(int t);
};

} // namespace reuss