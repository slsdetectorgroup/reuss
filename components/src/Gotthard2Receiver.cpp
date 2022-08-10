

#include "reuss/Gotthard2Receiver.h"
#include "reuss/Streamer.h"
#include "reuss/Writer.h"

#include <fmt/core.h>
#include <fmt/color.h>

namespace fs = std::filesystem;

namespace reuss{

    Gotthard2Receiver::Gotthard2Receiver() = default;
    Gotthard2Receiver::~Gotthard2Receiver(){
        delete receiver;
        delete preview_streamer;
    }

    void Gotthard2Receiver::setUdpSource(std::string_view source){
        auto pos = source.find(":");
        udp_ip = source.substr(0, pos);
        udp_port = source.substr(pos+1);

        stop();

        delete receiver;
        receiver = new G2UdpReceiver(udp_ip, udp_port, fifo_size);
        delete preview_streamer;
        preview_streamer = new Streamer("tcp://*:4547", receiver->preview_fifo());
    }

    void Gotthard2Receiver::setFrames(size_t frames){
        n_frames = frames;
    }

    bool Gotthard2Receiver::Fwrite(){
        return write_to_file;
    }

    void Gotthard2Receiver::setFpath(fs::path file_path){
        fpath = file_path;
    }

    fs::path Gotthard2Receiver::Fpath(){
        return fpath;
    }

    void Gotthard2Receiver::setFwrite(bool write){
        write_to_file = write;
    }
    
    size_t Gotthard2Receiver::Frames(){
        return n_frames;
    }

    void Gotthard2Receiver::setFname(std::string_view fname){
        base_name = fname;
    }

    std::string Gotthard2Receiver::Fname() const{
        return base_name;
    }

    std::string Gotthard2Receiver::UdpSource() const{
        return fmt::format("{}:{}", udp_ip, udp_port);
    }

    std::string Gotthard2Receiver::Pathname(){
        return fpath / base_name;
    }

    void Gotthard2Receiver::setStreamNth(size_t n){
        stream_nth = n;
    }
    size_t Gotthard2Receiver::StreamNth(){
        return stream_nth;
    }

    void Gotthard2Receiver::start(){
        fmt::print("Starting receiver\n");
        writer = std::make_unique<Writer>(receiver->fifo(), n_frames, Pathname());
        if (write_to_file)
            threads.emplace_back(&Writer::write, writer.get(), 1);
        else
            threads.emplace_back(&Writer::sink, writer.get(), 1);

    
        threads.emplace_back(&G2UdpReceiver::receive_n, receiver, 0, n_frames, 1024);
        threads.emplace_back(&Streamer::stream, preview_streamer, 2);
    }

    void Gotthard2Receiver::stop(){
        fmt::print(fg(fmt::color::red), "Stopping receiver!\n");
            if (receiver)
                receiver->stop();
            if (writer)
                writer->stop();
            if (preview_streamer)
                preview_streamer->stop();
            for(auto& t: threads)
                t.join();
            threads.clear();
            fmt::print(fg(fmt::color::red), "Receiver stopped!\n");
    }

    bool Gotthard2Receiver::done() const{
        if (receiver)
            return receiver->done();
        else    
            return false;
    }

    double Gotthard2Receiver::progress() const{
        if (receiver)
            return receiver->progress();
        else    
            return 0;
    }
}