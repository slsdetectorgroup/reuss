#include <string>
#include <vector>
#include <cstddef>
namespace reuss {

class ZmqReceiver {
    std::string endpoint;
    uint64_t zmq_hwm = 1000;
    int timeout = -1;
    void *context;
    void *socket;

  public:
    ZmqReceiver(const std::string &endpoint);
    ~ZmqReceiver();
    void receive_into(int n_frames, int64_t* frame_numbers, std::byte* data);
    void set_zmq_hwm(uint64_t hwm);
    void set_timeout(int t);
};

} // namespace reuss