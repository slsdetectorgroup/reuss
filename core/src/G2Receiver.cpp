#include "reuss/G2Receiver.h"
#include "RawFile.h"
#include "UdpSocket.h"
#include <chrono>
#include <cstdlib>
#include <fmt/color.h>
#include <fmt/format.h>
#include <memory>
#include <thread>


#include <cassert>
namespace reuss {

/* 
Specialized G2 receiver, making use of the fact that we have 
one packet per frame. 
*/


G2Receiver::G2Receiver(const std::string &node, const std::string &port)
    : G2Receiver(node, port, QUEUE_SIZE, 0U) {}

G2Receiver::G2Receiver(const std::string &node, const std::string &port,
                   size_t fifo_size, size_t n_frames)
    : fifo_(fifo_size, G2_FRAME_SIZE), preview_fifo_(fifo_size, G2_PAYLOAD_SIZE), n_frames(n_frames) {
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
    
    size_t frames_received = 0;
    int64_t frame_index = 0; //index to address frames when sending bunches

    while (!stopped_ && (frames_received<n_frames)) {
        ImageView img = fifo_.pop_free();
        
        size_t i = 0;
        while (i<G2_PACK && !stopped_ && (frames_received<n_frames)){

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
            auto offset = G2_PACKET_SIZE * i; //copy to index position
            auto dst = img.data + offset;
            
            memcpy(dst, &packet_buffer[0], G2_PACKET_SIZE);

            ++frames_received;
            ++i;
        }
        
        //Push one image to the preview fifo for each pack 
        ImageView preview = preview_fifo_.pop_free();
        preview.frameNumber = currentFrameNumber;
        preview.framesInPack = 1;
        memcpy(preview.data, src, G2_PAYLOAD_SIZE);
        preview_fifo_.push_image(preview);

        // img.frameNumber = currentFrameNumber;
        img.frameNumber = frame_index++;
        img.framesInPack = i;
        fifo_.push_image(img);
    }
    // make sure we have time to sink images
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    fmt::print("G2Receiver done\n");
}

} // namespace reuss