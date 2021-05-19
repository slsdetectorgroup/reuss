#include "reuss/StreamingReceiver.h"
#include <memory>
#include <sls/Detector.h>
;
namespace reuss {
StreamingReceiver::StreamingReceiver() {}

void StreamingReceiver::start() {
    constexpr auto endpoint = "tcp://*:4545";
    try {

        auto sources = get_sources();
        for (auto & s : sources) {
            receivers_.push_back(
                std::make_unique<Receiver>(s.addr, s.port));
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

    last_frame_ = last_frame();
    total_frames_ = total_frames();
    lost_packets_ = lost_packets();
    threads_.clear();
    assembler_ = nullptr;
    streamer_ = nullptr;
    receivers_.clear();
    fmt::print(fg(fmt::color::hot_pink), "Bye!\n");
}

int64_t StreamingReceiver::lost_packets() {
    if (receivers_.empty())
        return lost_packets_;

    int64_t total = 0;
    for (auto &r : receivers_)
        total += r->lost_packets();
    return total;
}

int64_t StreamingReceiver::last_frame() {
    if (streamer_)
        return streamer_->last_frame();
    else
        return last_frame_;
}

int64_t StreamingReceiver::total_frames(){
    if (streamer_)
        return streamer_->total_frames();
    else
        return -1;
}

std::vector<StreamingReceiver::UdpSources> StreamingReceiver::get_sources() {
    sls::Detector d;
    auto udp_srcip = d.getDestinationUDPIP();
    auto udp_srcip2 = d.getDestinationUDPIP2();
    auto udp_dstport = d.getDestinationUDPPort();
    auto udp_dstport2 = d.getDestinationUDPPort2();

    std::vector<StreamingReceiver::UdpSources> sources;

    for (int i = 0; i != d.size(); ++i) {
        sources.push_back({udp_srcip[i].str(), std::to_string(udp_dstport[i])});
        sources.push_back({udp_srcip2[i].str(), std::to_string(udp_dstport2[i])});
        // udp_ip.push_back(udp_srcip[i].str());
        // udp_port.push_back(std::to_string(udp_dstport[i]));
        // udp_ip.push_back(udp_srcip2[i].str());
        // udp_port.push_back(std::to_string(udp_dstport2[i]));
    }
    return sources;
}

} // namespace reuss