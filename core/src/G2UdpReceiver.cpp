#include "reuss/G2UdpReceiver.h"
#include "RawFile.h"

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <fmt/color.h>
#include <fmt/format.h>
#include <memory>
#include <thread>

namespace reuss {

/*
Specialized G2 receiver, making use of the fact that we have
one packet per frame.
*/

G2UdpReceiver::G2UdpReceiver(const std::string &node, const std::string &port)
    : G2UdpReceiver(node, port, QUEUE_SIZE) {}

G2UdpReceiver::G2UdpReceiver(const std::string &node, const std::string &port,
                             size_t fifo_size)
    : fifo_(fifo_size, G2_FRAME_SIZE),
      preview_fifo_(fifo_size, G2_PAYLOAD_SIZE), node(node), port(port) {}

G2UdpReceiver::~G2UdpReceiver() = default;
void G2UdpReceiver::stop() { stopped_ = true; }
int G2UdpReceiver::lost_packets() const noexcept { return total_lost_packets_; }
bool G2UdpReceiver::done() const { return done_; }
double G2UdpReceiver::progress() const { return progress_; }

void G2UdpReceiver::receive_n(int cpu, size_t n_frames, size_t stream_nth) {
    progress_ = 0;
    stopped_ = false;
    done_ = false;
    pin_this_thread(cpu);
    set_realtime_priority();

    PacketBuffer<G2_PAYLOAD_SIZE> pbuf;
    static_assert(sizeof(pbuf) == G2_PACKET_SIZE);
    uint64_t currentFrameNumber = 0;
    uint64_t lastFrameNumber = 0;
    size_t frames_received = 0;
    int64_t frame_index = 0; // index to address frames when sending bunches

    UdpSocket sock(node, port, G2_PACKET_SIZE);
    sock.setBufferSize(G2_UDP_SOCKET_SIZE);
// #define RMULTIRECV


#ifdef RMULTIRECV

    while (!stopped_ && (frames_received < n_frames)) {
        ImageView img = fifo_.pop_free();
        size_t i = 0;
        int rc = sock.multirecv(img.data);
        // fmt::print("Got: {} packets\n", rc);
        if (rc != -1) {
            frames_received += rc;
            ++i;
            img.framesInPack = rc;
            progress_ = static_cast<double>(frames_received) /
                        static_cast<double>(n_frames);
        } else {
            img.framesInPack = 0;
        }
        ImageView preview = preview_fifo_.pop_free();
        preview.framesInPack = 1;
        memcpy(preview.data, img.data + sizeof(PacketHeader),
               G2_PAYLOAD_SIZE); // copy first pkg
        preview_fifo_.push_image(preview);
        fifo_.push_image(img);
    }

#else
    fmt::print("Expecting: {} frames\n", n_frames);
    fmt::print("Frames received: {}\n", frames_received);
    while (!stopped_ && (frames_received < n_frames)) {
        ImageView img = fifo_.pop_free();
        size_t i = 0;

        while (i < G2_PACK && !stopped_ && (frames_received < n_frames)) {
            if (sock.receivePacket(&pbuf)) {
                currentFrameNumber = pbuf.header.frameNumber;

                // Debug printing
                if ((currentFrameNumber - lastFrameNumber != 1) &&
                    (lastFrameNumber != 0)) {
                    fmt::print(fg(fmt::color::red), "Lost {} frames\n",
                               currentFrameNumber - lastFrameNumber);
                }

                lastFrameNumber = currentFrameNumber;
                auto offset = G2_PACKET_SIZE * i;
                auto dst = img.data + offset;
                memcpy(dst, &pbuf, G2_PACKET_SIZE);

                // Stream one per n_frames
                if (frames_received && (frames_received % stream_nth == 0)) {
                    ImageView preview = preview_fifo_.pop_free();
                    preview.frameNumber = currentFrameNumber;
                    preview.framesInPack = 1;
                    memcpy(preview.data, &pbuf.data[0], G2_PAYLOAD_SIZE);
                    preview_fifo_.push_image(preview);
                }

                ++frames_received;
                ++i;
                progress_ = static_cast<double>(frames_received) /
                            static_cast<double>(n_frames);
                fmt::print("progress: {}\n", progress_);
            }
        }
        img.frameNumber = frame_index++;
        img.framesInPack = i;
        fifo_.push_image(img);
    }
    
#endif
    // make sure we have time to sink images
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    done_ = true;
    fmt::print("Caught {}/{} frames\n", frames_received, n_frames);
    fmt::print("G2UdpReceiver done\n");
}

} // namespace reuss