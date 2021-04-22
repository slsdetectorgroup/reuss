#include <string>
#include <vector>
#include <cstddef>
namespace reuss {

class ZmqReceiver {
    std::string endpoint;

  public:
    ZmqReceiver(const std::string &endpoint);
    void receive_into(int n_frames, int64_t* frame_numbers, std::byte* data);
};

} // namespace reuss