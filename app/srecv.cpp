#include "reuss/SummingReceiver.h"
#include "reuss/file_io.h"
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
    namespace rs = reuss;
    rs::direct_input();
    try {
        // The receiver reads the config from the sls::Detector API
        
        rs::SummingReceiver receiver;
        receiver.set_frames_to_sum(50);
        auto cal = reuss::load_numpy<float,3>("/home/l_msdetect/erik/epoc/calibration.npy");
        auto pd  = reuss::load<float,3>("/dev/shm/reuss/pedestal.bin", std::array<ssize_t,3>{3,512,1024});
        receiver.set_calibration(cal);
        receiver.set_pedestal(pd);
        receiver.start();
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        // receiver.record_pedestal();

        // Listen for 'q'
        while (true) {
            auto key = std::cin.get();
            if (key == 'q') {
                fmt::print(fg(fmt::color::red), "Received \'q\' aborting!\n");
                receiver.stop();
                break;
            } else if (key == 's') {
                fmt::print(
                    "Total frames: {}, last frame: {}, lost packets: {}\n",
                    receiver.total_frames(), receiver.last_frame(),
                    receiver.lost_packets());
            }else if (key == 'p') {
                fmt::print("Pedestal collection");
                receiver.record_pedestal();
            }
        }

    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
    rs::reset_terminal();
}