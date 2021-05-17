#pragma once
#include "reuss/DetectorInterface.h"
#include <memory>

namespace sls {
class Detector;
}

namespace reuss {
class JungfrauDetector : public DetectorInterface {

  public:
    JungfrauDetector();
    virtual ~JungfrauDetector();
    void start() override;
    void stop() override;
    void set_gain(int gain) override;
    int get_gain() const override;

    void set_period(double period) override;
    double get_period() const override;

  private:
    std::unique_ptr<sls::Detector> det;
};

} // namespace reuss
