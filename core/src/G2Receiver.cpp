#include "reuss/G2Receiver.h"
#include "UdpSocket.h"
#include <chrono>
#include <cstdlib>
#include <fmt/color.h>
#include <fmt/format.h>
#include <memory>
#include <thread>

namespace reuss {

constexpr size_t g2_payload_size = 2560;
constexpr size_t g2_packet_size = sizeof(PacketHeader) + g2_payload_size;

G2Receiver::G2Receiver(const std::string &node, const std::string &port)
    : G2Receiver(node, port, QUEUE_SIZE) {}

G2Receiver::G2Receiver(const std::string &node, const std::string &port,
                   size_t fifo_size)
    : fifo_(fifo_size, FRAME_SIZE) {
    sock = std::make_unique<UdpSocket>(node, port, g2_packet_size);
    fmt::print("Listening to: {}:{}\n", node, port);
    sock->setBufferSize(DEFAULT_UDP_BUFFER_SIZE);
    fmt::print("UDP buffer size: {} MB\n",
               static_cast<double>(sock->bufferSize()) / (1024. * 1024.));
}

G2Receiver::~G2Receiver() = default;

void G2Receiver::stop() {
    stopped_ = true;
    sock->shutdown();
}

int G2Receiver::lost_packets() const noexcept { return total_lost_packets_; }

void G2Receiver::receivePackets(int cpu) {
    pin_this_thread(cpu);
    set_realtime_priority();
    
    char packet_buffer[g2_packet_size];
    PacketHeader header{};
    sock->receivePacket(packet_buffer, header); // waits here for data
    uint64_t currentFrameNumber = header.frameNumber;

    int64_t totalFramesReceived = 0;

    while (!stopped_) {
        int numPacketsReceived = 0;
        uint64_t lastFrameNumber = currentFrameNumber;
        sock->receivePacket(packet_buffer, header); // waits here for data
        currentFrameNumber = header.frameNumber;
        if (currentFrameNumber-lastFrameNumber != 1){
            fmt::print("Lost frames: {}, {}\n", currentFrameNumber, lastFrameNumber);
        }
        if (!(currentFrameNumber%1000))
            fmt::print("Got frame {}\n", currentFrameNumber);
            
        // ImageView img = fifo_.pop_free();
        // img.frameNumber = currentFrameNumber;
        // while (!stopped_) {
        //     // copy ROI per row, controlled by COL_MAX and COL_MIN
        //     constexpr size_t packet_data_size =
        //         (COL_MAX - COL_MIN) * sizeof(uint16_t) * ROWS_PER_PACKET;
        //     auto offset = packet_data_size * header.packetNumber;
        //     auto dst = img.data + offset;
        //     auto src = &packet_buffer[COL_MIN * sizeof(uint16_t)] +
        //                sizeof(PacketHeader);
        //     auto dst_step = (COL_MAX - COL_MIN) * sizeof(uint16_t); // rowsize
        //     auto src_step = PKT_BYTES_PER_ROW;
        //     for (size_t row = 0; row < 4; ++row) {
        //         memcpy(dst, src, (COL_MAX - COL_MIN) * sizeof(u_int16_t));
        //         dst += dst_step;
        //         src += src_step;
        //     }

        //     ++numPacketsReceived;
            // sock->receivePacket(packet_buffer, header); // waits here for data
        //     if (currentFrameNumber != header.frameNumber)
        //         break;
        // }
        // if (numPacketsReceived != PACKETS_PER_FRAME) {
        //     int lost = PACKETS_PER_FRAME - numPacketsReceived;
        //     fmt::print("Frame: {} lost {} pkts\n", currentFrameNumber, lost);
        //     total_lost_packets_ += lost;
        // }
        // currentFrameNumber = header.frameNumber;
        // fifo_.push_image(img);
        // totalFramesReceived += 1;
    }
    // make sure we have time to sink images
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

} // namespace reuss