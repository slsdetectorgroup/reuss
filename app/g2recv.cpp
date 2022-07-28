
#include "reuss/StreamingReceiver.h"
#include "reuss/G2Receiver.h"

#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
    namespace rs = reuss;
    // rs::direct_input();
    rs::reset_terminal();
    try {
    //     // The receiver reads the config from the sls::Detector API
        std::vector<std::thread> threads;
        rs::G2Receiver receiver("10.1.2.125", "50001");
        threads.emplace_back(&rs::G2Receiver::receivePackets, &receiver, 0);
        
        rs::Streamer streamer(RAW_FRAMES_ENDPOINT, receiver.fifo());
        threads.emplace_back(&rs::Streamer::stream, &streamer, 1);

        rs::Streamer preview_streamer("tcp://*:5000", receiver.preview_fifo());
        threads.emplace_back(&rs::Streamer::stream, &preview_streamer, 2);

        // Listen for 'q'
        while (true) {
            auto key = std::cin.get();
            if (key == 'q') {
                fmt::print(fg(fmt::color::red), "Received \'q\' aborting!\n");
                receiver.stop();
                streamer.stop();
                preview_streamer.stop();
                for(auto& t: threads)
                    t.join();

                break;
            } else if (key == 's') {
                // fmt::print(
                //     "Total frames: {}, last frame: {}, lost packets: {}\n",
                //     receiver.total_frames(), receiver.last_frame(),
                //     receiver.lost_packets());
            }
        }

    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
    // rs::reset_terminal();
}