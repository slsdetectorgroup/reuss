#pragma once
#include "reuss/project_defs.h"


#include <atomic>
#include <chrono>
#include <fmt/color.h>
#include <fmt/core.h>
#include <string>
#include <thread>
#include <zmq.h>


using namespace std::chrono_literals;
namespace reuss {

class ZmqWriter {
    std::string endpoint;
    std::string filename = "test";
    std::thread writer_thread;
    std::atomic<bool> request_stop{false};
    std::atomic<bool> running{false};
    std::atomic<int64_t> frames_written_{};

    void write();

  public:
    ZmqWriter(const std::string &endpoint);
    void set_fname(const std::string &fname);
    void start();
    void stop();
    bool is_running();
    bool stop_requested();
    int64_t frames_written()const noexcept;
};

} // namespace jf