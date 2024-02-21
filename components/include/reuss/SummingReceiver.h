#pragma once
#include "reuss/FrameAssembler.h"
#include "reuss/FrameSummer.h"
#include "reuss/Receiver.h"
#include "reuss/Streamer.h"
#include <string>
#include <vector>

namespace reuss {
class DetectorInterface;

class SummingReceiver {
    std::vector<std::unique_ptr<Receiver>> receivers_;
    std::vector<std::thread> receiving_threads_;
    std::vector<std::thread> processing_threads_;
    std::unique_ptr<Streamer> streamer_;
    std::vector<std::unique_ptr<Streamer>> streamers_;
    std::unique_ptr<FrameAssembler> assembler_;
    // std::unique_ptr<FrameSummer<float>> summer_;
    std::vector<std::unique_ptr<FrameSummer<float>>> summers_;
    std::unique_ptr<DetectorInterface> det_;

    int64_t lost_packets_{};
    int64_t total_frames_{};
    int64_t last_frame_{};


  public:
    SummingReceiver();
    SummingReceiver(std::unique_ptr<DetectorInterface>&& d);
    ~SummingReceiver();
    void start();
    void stop();

    int64_t lost_packets();
    int64_t last_frame();
    int64_t total_frames();

    void set_pedestal(ImageData<float, 3> pedestal);
    ImageData<float, 3> get_pedestal() const;
    void record_pedestal();

    void set_calibration(ImageData<float, 3> calibration);
    ImageData<float, 3> get_calibration() const;

    void set_frames_to_sum(int n) { 
      for(auto &summer_ : summers_){
        summer_->set_frames_to_sum(n);
      }
    }
    int get_frames_to_sum() const { return summers_[0]->get_frames_to_sum(); }

    
};
} // namespace reuss
