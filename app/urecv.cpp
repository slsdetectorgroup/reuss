#include "reuss/FrameAssembler.h"
#include "reuss/Receiver.h"
#include "reuss/Streamer.h"
#include "reuss/utils.h"

// #include "Writer.h"
#include <fmt/color.h>
#include <fmt/format.h>
#include <thread>


#include <iostream>
#include <memory>
#include <vector>


constexpr auto endpoint = "tcp://*:4545";

int main(int argc, char *argv[]) {
    namespace rs = reuss;
    rs::direct_input();
    try {
        // Create receivers
        std::vector<std::unique_ptr<rs::Receiver>> receivers;
        receivers.push_back(
            std::make_unique<rs::Receiver>("10.1.2.160", "50020"));
        receivers.push_back(
            std::make_unique<rs::Receiver>("10.1.1.160", "50021"));

        // Start listening threads
        int cpu = 0;
        std::vector<std::thread> threads;
        for (auto &r : receivers) {
            threads.emplace_back(&rs::Receiver::receivePackets, r.get(), cpu++);
        }

        rs::FrameAssembler assembler(receivers);
        threads.emplace_back(&rs::FrameAssembler::assemble, &assembler, cpu++);

        rs::Streamer streamer(endpoint, assembler.fifo());
        threads.emplace_back(&rs::Streamer::stream, &streamer, cpu++);

        // fmt::print("Frame size: {} bytes\n", assembler.frame_size());

        // Listen for 'q'
        while (true) {
            auto key = std::cin.get();
            if (key == 'q') {
                fmt::print(fg(fmt::color::red), "Received \'q\' aborting!\n");
                for (auto &r : receivers)
                    r->stop();
                assembler.stop();
                streamer.stop();
                break;
            }
        }

        for (auto &&t : threads)
            t.join();

    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
    fmt::print(fg(fmt::color::hot_pink), "Bye!\n");
    rs::reset_terminal();
}