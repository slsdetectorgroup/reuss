#include "reuss/DummyDetector.h"
#include <fmt/core.h>

namespace reuss{
    void DummyDetector::start(){
        fmt::print("DummyDetector::start()\n");
    }
    void DummyDetector::stop(){
        fmt::print("DummyDetector::stop()\n");
    }
    void DummyDetector::set_gain(int gain){
        fmt::print("DummyDetector::set_gain({})\n", gain);
    }

    int DummyDetector::get_gain() const{
        fmt::print("DummyDetector::get_gain() --> {}\n", gain_);
        return gain_;
    }

    void DummyDetector::set_period(double period){
        fmt::print("DummyDetector::set_period({})\n", period);
    }
    double DummyDetector::get_period() const{
        fmt::print("DummyDetector::get_period() --> {}\n", period_);
        return period_;
    }



}