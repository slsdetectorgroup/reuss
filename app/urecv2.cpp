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

#include "reuss/StreamingReceiver.h"
constexpr auto endpoint = "tcp://*:4545";

int main(int argc, char *argv[]) {
    namespace rs = reuss;
    rs::direct_input();
    try {
        // Create receivers
        rs::StreamingReceiver receiver;
        receiver.start();
        
        // Listen for 'q'
        while (true) {
            auto key = std::cin.get();
            if (key == 'q') {
                fmt::print(fg(fmt::color::red), "Received \'q\' aborting!\n");
                receiver.stop();
                break;
            }
        }


    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
    fmt::print(fg(fmt::color::hot_pink), "Bye!\n");
    rs::reset_terminal();
}