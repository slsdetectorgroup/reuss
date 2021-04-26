
/*
Send random data over zmq, used for debugging and development.

*/


#include "reuss/ZmqSocket.h"
#include "reuss/ImageView.h"
#include <fmt/core.h>
#include <string>
#include <vector>
#include <cstddef>
#include <random>

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

void generate_data(std::vector<uint16_t>& data){
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);

    for (auto& item : data)
        item = dist(rng);

}

int main(int argc, char** argv){
    
    int n_frames = 500;
    if(argc>1)
        n_frames = std::atoi(argv[1]);
    if (!n_frames){
        fmt::print("Could not decode number of frames from arg 1: {}\n", argv[1]);
        exit(1);
    }
        
    std::string endpoint = "tcp://*:4545";
    std::vector<uint16_t> buffer(512*1024);
    reuss::ImageView image{int64_t(0), reinterpret_cast<char*> (&buffer[0])};
    reuss::ZmqSocket socket(endpoint);

    for (int i = 0; i<n_frames; ++i){
        image.frameNumber++;
        generate_data(buffer);
        socket.send(image, 512*1024*2);
        fmt::print("{}\n", i);
        std::this_thread::sleep_for(100ms);
    }
    
}