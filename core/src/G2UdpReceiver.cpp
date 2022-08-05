#include "reuss/G2UdpReceiver.h"
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

G2UdpReceiver::G2UdpReceiver(const std::string &node, const std::string &port)
    : G2UdpReceiver(node, port, QUEUE_SIZE) {}

G2UdpReceiver::G2UdpReceiver(const std::string &node, const std::string &port,
                             size_t fifo_size)
    : fifo_(fifo_size, G2_FRAME_SIZE),
      preview_fifo_(fifo_size, G2_PAYLOAD_SIZE) {
    sock = std::make_unique<UdpSocket>(node, port, G2_PACKET_SIZE);
    fmt::print("Listening to: {}:{}\n", node, port);
    sock->setBufferSize(G2_UDP_SOCKET_SIZE);
    fmt::print("UDP buffer size: {} MB\n",
               static_cast<double>(sock->bufferSize()) / (1024. * 1024.));
}

G2UdpReceiver::~G2UdpReceiver() {
    if (sock)
        sock->shutdown();
}

void G2UdpReceiver::stop() { stopped_ = true; }

int G2UdpReceiver::lost_packets() const noexcept { return total_lost_packets_; }

bool G2UdpReceiver::done() const { return done_; }

double G2UdpReceiver::progress() const { return progress_; }

void G2UdpReceiver::receive_n(int cpu, size_t n_frames, size_t stream_nth) {
    stopped_ = false;
    done_ = false;
    pin_this_thread(cpu);
    set_realtime_priority();

    char packet_buffer[G2_PACKET_SIZE];
    auto src = &packet_buffer[0] + sizeof(PacketHeader);
    uint64_t currentFrameNumber = 0;
    uint64_t lastFrameNumber = 0;

    size_t frames_received = 0;
    int64_t frame_index = 0; // index to address frames when sending bunches

    while (!stopped_ && (frames_received < n_frames)) {
        ImageView img = fifo_.pop_free();
        size_t i = 0;
        while (i < G2_PACK && !stopped_ && (frames_received < n_frames)) {

            PacketHeader header{};
            if (sock->receivePacket(packet_buffer, header)) {
                currentFrameNumber = header.frameNumber;

                // Debug printing
                if ((currentFrameNumber - lastFrameNumber != 1) &&
                    (lastFrameNumber != 0)) {
                    fmt::print(fg(fmt::color::red), "Lost {} frames\n",
                               currentFrameNumber - lastFrameNumber);
                }

                lastFrameNumber = currentFrameNumber;
                auto offset = G2_PACKET_SIZE * i;
                auto dst = img.data + offset;
                memcpy(dst, &packet_buffer[0], G2_PACKET_SIZE);

                // Stream one per n_frames
                if (frames_received % stream_nth == 0) {
                    ImageView preview = preview_fifo_.pop_free();
                    preview.frameNumber = currentFrameNumber;
                    preview.framesInPack = 1;
                    memcpy(preview.data, src, G2_PAYLOAD_SIZE);
                    preview_fifo_.push_image(preview);
                }

                ++frames_received;
                ++i;

                progress_ = static_cast<double>(frames_received) /
                            static_cast<double>(n_frames);
            }
        }
        img.frameNumber = frame_index++;
        img.framesInPack = i;
        fifo_.push_image(img);
    }
    // make sure we have time to sink images
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    done_ = true;
    fmt::print("G2UdpReceiver done\n");
}

} // namespace reuss