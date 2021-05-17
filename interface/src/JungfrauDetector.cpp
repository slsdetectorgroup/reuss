#include "reuss/JungfrauDetector.h"
#include <fmt/core.h>
#include <sls/Detector.h>
#include <chrono>

namespace reuss {
JungfrauDetector::JungfrauDetector()
    : det{std::make_unique<sls::Detector>()} {};
JungfrauDetector::~JungfrauDetector() = default;

void JungfrauDetector::start() {
    fmt::print("JungfrauDetector::start()\n");
    det->startDetector();
}
void JungfrauDetector::stop() {
    fmt::print("JungfrauDetector::stop()\n");
    det->stopDetector();
}
void JungfrauDetector::set_gain(int gain) {
    fmt::print("JungfrauDetector::set_gain({})\n", gain);
    if (gain == 0)
        det->setSettings(sls::defs::DYNAMICGAIN);
    else if(gain == 1)
        det->setSettings(sls::defs::FORCESWITCHG1);
    else if(gain == 2)
        det->setSettings(sls::defs::FORCESWITCHG2);
    else
        throw std::runtime_error("Unknown gain");
}

int JungfrauDetector::get_gain() const{
    auto g = det->getSettings().squash();

    int gain = -1;
    if (g == sls::defs::DYNAMICGAIN)
        gain = 0;
    else if (g == sls::defs::FORCESWITCHG1)
        gain = 1;
    else if (g == sls::defs::FORCESWITCHG2)
        gain = 2;
    else
        throw std::runtime_error("Unknown gain");

    fmt::print("JungfrauDetector::get_gain() --> {}\n", gain);
    return gain;
}

void JungfrauDetector::set_period(double period) {
    fmt::print("JungfrauDetector::set_period({})\n", period);
    std::chrono::duration<double> d(period);
    auto p = std::chrono::duration_cast<std::chrono::nanoseconds>(d);
    det->setPeriod(p);
}
double JungfrauDetector::get_period() const{
    fmt::print("JungfrauDetector::get_period()\n");
    auto p = det->getPeriod().squash();

    return std::chrono::duration<double>(p).count();
}
} // namespace reuss