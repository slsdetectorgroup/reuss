#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <thread>
#include <memory>
#include <filesystem>

#include "reuss/G2Receiver.h"
#include "reuss/Streamer.h"
#include "reuss/Writer.h"


namespace reuss{

class Gotthard2Receiver{
    std::string udp_ip;
    std::string udp_port;
    std::vector<std::thread> threads;
    std::filesystem::path fpath;

    G2Receiver* receiver{};
    Streamer* preview_streamer{};
    std::unique_ptr<Writer> writer{};

    size_t n_frames{};
    size_t fifo_size{100};
    bool write_to_file{};
    bool receiving_done{};

    

public:
    Gotthard2Receiver();
    ~Gotthard2Receiver();

    void setUdpSource(std::string_view source);
    std::string UdpSource() const;

    void setFrames(size_t frames);
    size_t Frames();

    void setFwrite(bool write);
    bool Fwrite();

    void setFpath(std::filesystem::path file_path);
    std::filesystem::path Fpath();

    std::string Pathname();
    void start();
    void stop();

    bool done() const;
    double progress() const;
};

}