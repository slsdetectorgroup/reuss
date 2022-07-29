
/*
Send random data over zmq, used for debugging and development.

*/


#include "reuss/ZmqSocket.h"
#include "reuss/ImageView.h"
#include "reuss/project_defs.h"
#include <fmt/core.h>
#include <string>
#include <vector>
#include <cstddef>
#include <random>

#include <chrono>
#include <thread>
#include <fstream>
using namespace std::chrono_literals;

void generate_data(std::vector<uint16_t>& data){
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);
    for (auto& item : data)
        item = static_cast<uint16_t>(dist(rng));
}

int main(int argc, char** argv){
    
    int n_frames = 500;
    std::ifstream ifs;
    std::string fname;
    if(argc>1)
        n_frames = std::atoi(argv[1]);
    if (!n_frames){
        fmt::print("Could not decode number of frames from arg 1: {}\n", argv[1]);
        exit(1);
    }
    if(argc > 2){
        fname = argv[2];
        ifs.open(fname, std::ios::binary);
    }
        
    std::string endpoint = RAW_FRAMES_ENDPOINT;    
    std::vector<uint16_t> buffer(IMAGE_SIZE.rows*IMAGE_SIZE.cols);
    reuss::ImageView image{int64_t(0), int64_t(1),reinterpret_cast<char*> (&buffer[0])};
    reuss::ZmqSocket socket(endpoint);

    for (int i = 0; i<n_frames; ++i){
        image.frameNumber++;
        if (fname.empty())
            generate_data(buffer);
        else
            ifs.read(reinterpret_cast<char*> (buffer.data()), IMAGE_SIZE_BYTES);
        socket.send(image, IMAGE_SIZE_BYTES);
        fmt::print("{}\n", i);
        // std::this_thread::sleep_for(100ms);
    }
    
}