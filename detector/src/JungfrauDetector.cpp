#include "reuss/JungfrauDetector.h"
#include <sls/Detector.h>
#include <chrono>
#include <fmt/core.h>


// #include "reuss/project_defs.h"

namespace reuss {
JungfrauDetector::JungfrauDetector()
    : det{std::make_unique<sls::Detector>()} {};
JungfrauDetector::~JungfrauDetector() = default;

void JungfrauDetector::start() {
    det->startDetector();
}
void JungfrauDetector::stop() {
    det->stopDetector();
}

enum gainMode {
        DYNAMIC,
        FORCE_SWITCH_G1,
        FORCE_SWITCH_G2,
        FIX_G1,
        FIX_G2,
        FIX_G0
    };

void JungfrauDetector::set_gain(int gain) {
    if (gain == 0)
        det->setGainMode(sls::defs::DYNAMIC);
    else if(gain == 1)
        det->setGainMode(sls::defs::FORCE_SWITCH_G1);
    else if(gain == 2)
        det->setGainMode(sls::defs::FORCE_SWITCH_G2);
    else
        throw std::runtime_error("Unknown gain");
}

int JungfrauDetector::get_gain() const{
    auto g = det->getGainMode().squash();
    int gain = -1;
    if (g == sls::defs::DYNAMIC)
        gain = 0;
    else if (g == sls::defs::FORCE_SWITCH_G1)
        gain = 1;
    else if (g == sls::defs::FORCE_SWITCH_G2)
        gain = 2;
    else
        throw std::runtime_error("Unknown gain");

    return gain;
}

void JungfrauDetector::set_period(double period) {
    std::chrono::duration<double> d(period);
    auto p = std::chrono::duration_cast<std::chrono::nanoseconds>(d);
    det->setPeriod(p);
}
double JungfrauDetector::get_period() const{
    auto p = det->getPeriod().squash();
    return std::chrono::duration<double>(p).count();
}

std::vector<UdpSource> JungfrauDetector::get_udp_sources() const {

    auto udp_srcip = det->getDestinationUDPIP();
    auto udp_srcip2 = det->getDestinationUDPIP2();
    auto udp_dstport = det->getDestinationUDPPort();
    auto udp_dstport2 = det->getDestinationUDPPort2();

    std::vector<UdpSource> sources;

    for (int i = 0; i != det->size(); ++i) {
        sources.push_back({udp_srcip[i].str(), std::to_string(udp_dstport[i])});
        sources.push_back({udp_srcip2[i].str(), std::to_string(udp_dstport2[i])});
    }
    return sources;
}

} // namespace reuss