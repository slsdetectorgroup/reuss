#pragma once
/*
Minimal detector interface to isolate the reuss software from the concrete
detector API and to provide testing.
*/

#include <chrono>
#include <string>
#include <vector>

namespace reuss {

struct UdpSource {
    std::string addr;
    std::string port;
};

class DetectorInterface {
  public:
    // DetectorInterface() = default;
    virtual ~DetectorInterface() = default;
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void set_gain(int gain) = 0;
    virtual int get_gain() const = 0;

    virtual void set_period(double period) = 0;
    virtual double get_period() const = 0;

    virtual std::vector<UdpSource> get_udp_sources() const = 0;
};

} // namespace reuss
