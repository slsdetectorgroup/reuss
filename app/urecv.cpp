
#include "reuss/StreamingReceiver.h"

#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <thread>

constexpr auto endpoint = "tcp://*:4545";

int main(int argc, char *argv[]) {
    namespace rs = reuss;
    rs::direct_input();
    try {
        // The receiver reads the config from the sls::Detector API
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
    rs::reset_terminal();
}