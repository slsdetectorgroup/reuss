#include "reuss/FrameAssembler.h"
#include "reuss/Receiver.h"
#include "reuss/Streamer.h"
#include "reuss/utils.h"

// #include "Writer.h"
#include <fmt/color.h>
#include <fmt/format.h>
#include <thread>

// #include "RxMemory.h"

#include <iostream>
#include <memory>
#include <vector>

constexpr auto node = "192.168.122.1";
constexpr auto endpoint = "tcp://*:4545";
// constexpr int starting_port = 50001;

int main(int argc, char *argv[]) {
    namespace rs = reuss;
    rs::direct_input();
    try {
        // Create receivers
        std::vector<std::unique_ptr<rs::Receiver>> receivers;
        // for (int i = 0; i < 2; ++i) {
        //     receivers.push_back(rs::make_unique<rs::Receiver>(
        //         node, std::to_string(starting_port + i)));
        // }
        receivers.push_back(
            std::make_unique<rs::Receiver>("10.1.1.160", "50020"));
        receivers.push_back(
            std::make_unique<rs::Receiver>("10.1.2.160", "50021"));

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

        // rs::Writer writer(assembler.fifo());
        // threads.emplace_back(&rs::Writer::write, &writer, cpu++);

        // Listen for 'q'
        while (true) {
            auto key = std::cin.get();
            if (key == 'q') {
                fmt::print(fg(fmt::color::red), "Recived \'q\' aborting!\n");
                for (auto &r : receivers)
                    r->stop();
                assembler.stop();
                streamer.stop();
                // writer.stop();
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