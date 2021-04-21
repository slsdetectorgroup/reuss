#include "reuss/Receiver.h"
#include "UdpSocket.h"
#include <chrono>
#include <cstdlib>
#include <fmt/color.h>
#include <fmt/format.h>
#include <thread>
#include <memory>

namespace reuss {

Receiver::Receiver(const std::string &node, const std::string &port)
    : Receiver(node, port, QUEUE_SIZE) {}

Receiver::Receiver(const std::string &node, const std::string &port,
                   size_t fifo_size)
    : fifo_(fifo_size, FRAME_SIZE) {
    sock = std::make_unique<UdpSocket>(node, port, PACKET_SIZE);
    fmt::print("Listening to: {}:{}\n", node, port);
    sock->setBufferSize(DEFAULT_UDP_BUFFER_SIZE);
    fmt::print("UDP buffer size: {} MB\n",
               sock->bufferSize() / (1024. * 1024.));
}

Receiver::~Receiver() = default;

void Receiver::stop() {
    stopped_ = true;
    sock->shutdown();
}

void Receiver::receivePackets(int cpu) {
    pin_this_thread(cpu);
    set_realtime_priority();
    char packet_buffer[PACKET_SIZE];
    PacketHeader header{};
    sock->receivePacket(packet_buffer, header); // waits here for data
    uint64_t currentFrameNumber = header.frameNumber;
    int numPacketsReceived = 0;
    int64_t totalPacketsLost = 0;
    int64_t totalFramesReceived = 0;
    while (!stopped_) {
        ImageView img = fifo_.pop_free();
        img.frameNumber = currentFrameNumber;
        while (!stopped_) {
            memcpy(img.data + PAYLOAD_SIZE * header.packetNumber,
                   packet_buffer + sizeof(PacketHeader), PAYLOAD_SIZE);
            ++numPacketsReceived;
            sock->receivePacket(packet_buffer, header); // waits here for data
            if (currentFrameNumber != header.frameNumber)
                break;
        }
        if (numPacketsReceived != PACKETS_PER_FRAME) {
            auto lost = PACKETS_PER_FRAME - numPacketsReceived;
            fmt::print("Frame: {} lost {} pkts\n", currentFrameNumber, lost);
            totalPacketsLost += lost;
        }
        currentFrameNumber = header.frameNumber;
        numPacketsReceived = 0;
        fifo_.push_image(img);
        totalFramesReceived += 1;
        if (totalFramesReceived % PRINT_MOD == 0) {
            fmt::print("{} Received: {} frames, lost {} packets\n", cpu,
                       totalFramesReceived, totalPacketsLost);
            fmt::print("{} Free: {}, data: {}\n", cpu, fifo_.numFreeSlots(),
                       fifo_.numFilledSlots());
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(
        1000)); // make sure we have time to sink images
    fmt::print("UDP thread done\n");
}



} // namespace reuss