#include "reuss/G2Receiver.h"
#include "UdpSocket.h"
#include <chrono>
#include <cstdlib>
#include <fmt/color.h>
#include <fmt/format.h>
#include <memory>
#include <thread>


#include <cassert>
namespace reuss {




G2Receiver::G2Receiver(const std::string &node, const std::string &port)
    : G2Receiver(node, port, QUEUE_SIZE) {}

G2Receiver::G2Receiver(const std::string &node, const std::string &port,
                   size_t fifo_size)
    : fifo_(fifo_size, G2_FRAME_SIZE), preview_fifo_(fifo_size, G2_PAYLOAD_SIZE) {
    sock = std::make_unique<UdpSocket>(node, port, G2_PACKET_SIZE);
    fmt::print("Listening to: {}:{}\n", node, port);
    sock->setBufferSize(1024 * 1024 * 1000);
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
    


    char packet_buffer[G2_PACKET_SIZE];
    auto src = &packet_buffer[0] +sizeof(PacketHeader);
    uint64_t currentFrameNumber = 0;
    uint64_t lastFrameNumber = 0;
    int64_t totalFramesReceived = 0;

    while (!stopped_) {
        ImageView img = fifo_.pop_free();
        
        size_t i = 0;
        while (i<G2_PACK && !stopped_){

            PacketHeader header{};
            sock->receivePacket(packet_buffer, header); // waits here for data
            currentFrameNumber = header.frameNumber;

            //Debug printing
            if (currentFrameNumber-lastFrameNumber != 1){
                fmt::print(fg(fmt::color::red),"Lost {} frames\n", currentFrameNumber-lastFrameNumber);
            }
            if (!(currentFrameNumber%10000))
                fmt::print("Got frame {}\n", currentFrameNumber);

            lastFrameNumber = currentFrameNumber;
            auto offset = G2_PAYLOAD_SIZE * i; //copy to index position
            auto dst = img.data + offset;
            

            memcpy(dst, src, G2_PAYLOAD_SIZE);

            ++i;

        }
        
        //Push one image to the preview fifo for each pack 
        ImageView preview = preview_fifo_.pop_free();
        preview.frameNumber = currentFrameNumber;
        memcpy(preview.data, src, G2_PAYLOAD_SIZE);
        preview_fifo_.push_image(preview);

        // img.frameNumber = currentFrameNumber;
        img.frameNumber = totalFramesReceived++;
        fifo_.push_image(img);
    }
    // make sure we have time to sink images
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

} // namespace reuss