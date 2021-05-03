#include "reuss/StreamingReceiver.h"

#include <memory>
#include <sls/Detector.h>
// #include <fmt/core.h>
using namespace std::chrono_literals;
namespace reuss {
StreamingReceiver::StreamingReceiver() {}

void StreamingReceiver::start() {
    constexpr auto endpoint = "tcp://*:4545";
    try {
        {
            sls::Detector d;
            auto udp_srcip = d.getDestinationUDPIP();
            auto udp_srcip2 = d.getDestinationUDPIP2();
            auto udp_dstport = d.getDestinationUDPPort();
            auto udp_dstport2 = d.getDestinationUDPPort2();

            for (int i = 0; i != d.size(); ++i) {
                udp_ip.push_back(udp_srcip[i].str());
                udp_port.push_back(std::to_string(udp_dstport[i]));
                udp_ip.push_back(udp_srcip2[i].str());
                udp_port.push_back(std::to_string(udp_dstport2[i]));
            }
        }

        for (int i = 0; i != udp_ip.size(); ++i) {
            receivers_.push_back(std::make_unique<Receiver>(
                udp_ip[i], udp_port[i]));
                fmt::print("{}\n", udp_port[i].size());
        }
        int cpu = 0;

        for (auto &r : receivers_) {
            threads_.emplace_back(&Receiver::receivePackets, r.get(), cpu++);
        }

        assembler_ = std::make_unique<FrameAssembler>(receivers_);
        threads_.emplace_back(&FrameAssembler::assemble, assembler_.get(),
                              cpu++);

        streamer_ = std::make_unique<Streamer>(endpoint, assembler_->fifo());
        threads_.emplace_back(&Streamer::stream, streamer_.get(), cpu++);

    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
}

void StreamingReceiver::stop() {
    fmt::print("Stopping\n");
    for (auto &r : receivers_)
        r->stop();
    assembler_->stop();
    streamer_->stop();

    for (auto &&t : threads_)
        t.join();

    threads_.clear();
    assembler_= nullptr;
    streamer_ = nullptr;
    receivers_.clear();
    udp_ip.clear();
    udp_port.clear();

    fmt::print(fg(fmt::color::hot_pink), "Bye!\n");
}

int64_t StreamingReceiver::packets_lost(){
    int64_t total = 0;
    for (auto& r : receivers_)
        total += r->packets_lost();
    return total;
}

int64_t StreamingReceiver::last_frame(){
    if (streamer_)
        return streamer_->last_frame();
    else
        return -1;
}

} // namespace reuss