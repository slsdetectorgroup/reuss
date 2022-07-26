#pragma once
#include <cstddef>
#include <string>
#include <vector>
namespace reuss {

class ZmqReceiver {
    std::string endpoint;
    int zmq_hwm = 1000;
    int timeout = -1; // wait forever
    void *context = nullptr;
    void *socket = nullptr;

  public:
    ZmqReceiver(const std::string &endpoint);
    ~ZmqReceiver();
    ZmqReceiver(const ZmqReceiver&) = delete;
    ZmqReceiver operator=(const ZmqReceiver&) = delete;
    ZmqReceiver(ZmqReceiver&&) = delete;


    void connect();
    void disconnect();
    int receive_into(int n_frames, int64_t *frame_numbers, std::byte *data);
    void set_zmq_hwm(int hwm);
    void set_timeout(int t);
};

} // namespace reuss