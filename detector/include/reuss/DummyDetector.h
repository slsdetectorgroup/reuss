#pragma once
#include "reuss/DetectorInterface.h"

namespace reuss {
class DummyDetector : public DetectorInterface {
    double period_{};
    int gain_{};
    public:
        void start() override;
        void stop() override;
        void set_gain(int gain) override;
        int get_gain() const override;

        void set_period(double period) override;
        double get_period() const override;
        std::vector<UdpSource> get_udp_sources() const override;

        void set_pedestal_mode(uint8_t frames, uint16_t loops) override;
        void disable_pedestal_mode() override;


};
} // namespace reuss
