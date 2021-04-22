#include <string>
#include <vector>
#include <cstddef>
namespace reuss {

class ZmqReceiver {
    std::string endpoint;
    uint64_t zmq_hwm = 1000;

  public:
    ZmqReceiver(const std::string &endpoint);
    void receive_into(int n_frames, int64_t* frame_numbers, std::byte* data);
    void set_zmq_hwm(uint64_t hwm);
};

} // namespace reuss