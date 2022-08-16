#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <thread>
#include <memory>
#include <filesystem>

#include "reuss/G2UdpReceiver.h"
#include "reuss/Streamer.h"
#include "reuss/Writer.h"


namespace reuss{

class Gotthard2Receiver{
    std::string udp_ip;
    std::string udp_port;
    std::vector<std::thread> threads;
    std::filesystem::path fpath;
    std::string base_name{G2_DEFAULT_BASE_NAME};

    G2UdpReceiver* receiver{};
    Streamer* preview_streamer{};
    std::unique_ptr<Writer> writer{};

    size_t n_frames{};
    size_t fifo_size{100};
    size_t stream_nth{1024};
    size_t findex{};
    bool write_to_file{};
    // bool receiving_done{};
    bool running_{};
    

    

public:
    Gotthard2Receiver();
    ~Gotthard2Receiver();

    void setUdpSource(std::string_view source);
    std::string UdpSource() const;

    void setFrames(size_t frames);
    size_t Frames();

    void setFwrite(bool write);
    bool Fwrite();

    void setFindex(size_t i);
    size_t Findex() const;

    void setStreamNth(size_t n);
    size_t StreamNth();

    void setFname(std::string_view fname);
    std::string Fname() const;

    void setFpath(std::filesystem::path file_path);
    std::filesystem::path Fpath();

    std::string Pathname();
    void start();
    void stop();

    bool done() const;
    bool running() const;
    double progress() const;
};

}