
#include <string>
#include <vector>
#include "reuss/Receiver.h"
#include "reuss/Streamer.h"
#include "reuss/FrameAssembler.h"

namespace reuss {
class StreamingReceiver {
    std::vector<std::unique_ptr<Receiver>>  receivers_;
    std::vector<std::thread> threads_;
    std::unique_ptr<Streamer> streamer_;
    std::unique_ptr<FrameAssembler> assembler_;

    std::vector<std::string> udp_ip;
        std::vector<std::string> udp_port;


  public:
    StreamingReceiver();
    void start();
    void stop();

    int64_t packets_lost();
    int64_t last_frame();
  
};
} // namespace reuss
