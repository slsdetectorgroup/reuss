#include <fmt/core.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "reuss/DummyDetector.h"
#include "reuss/JungfrauDetector.h"

namespace py = pybind11;

class PyDetectorInterface : public reuss::DetectorInterface {
  public:
    /* Inherit the constructors */
    using DetectorInterface::DetectorInterface;
    void start() override {
        PYBIND11_OVERRIDE_PURE(
            void,              /* Return type */
            DetectorInterface, /* Parent class */
            start, /* Name of function in C++ (must match Python name) */
        );
    }
    void stop() override {
        PYBIND11_OVERRIDE_PURE(
            void,              /* Return type */
            DetectorInterface, /* Parent class */
            stop, /* Name of function in C++ (must match Python name) */
        );
    }
    void set_gain(int gain) override {
        PYBIND11_OVERRIDE_PURE(
            void,              /* Return type */
            DetectorInterface, /* Parent class */
            set_gain, /* Name of function in C++ (must match Python name) */
            gain);
    }
    int get_gain() const override {
        PYBIND11_OVERRIDE_PURE(
            int,               /* Return type */
            DetectorInterface, /* Parent class */
            get_gain, /* Name of function in C++ (must match Python name) */
        );
    }
    void set_period(double period) override {
        PYBIND11_OVERRIDE_PURE(
            void,              /* Return type */
            DetectorInterface, /* Parent class */
            set_period, /* Name of function in C++ (must match Python name) */
            period);
    }
    double get_period() const override {
        PYBIND11_OVERRIDE_PURE(
            double,            /* Return type */
            DetectorInterface, /* Parent class */
            get_period, /* Name of function in C++ (must match Python name) */
        );
    }
    std::vector<reuss::UdpSource> get_udp_sources() const override {
        PYBIND11_OVERRIDE_PURE(
            std::vector<reuss::UdpSource>,            /* Return type */
            DetectorInterface, /* Parent class */
            get_udp_sources, /* Name of function in C++ (must match Python name) */
        );
    }
};

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void variant_func(std::variant<int, double> var) {
    std::visit(overloaded{
                //    [](auto arg) { std::cout << arg << ' '; },
                   [](double arg) { fmt::print("Got an double: {}\n", arg); },
                   [](int arg) { fmt::print("Got an int: {}\n", arg); },
               },
               var);
}

void init_interface(py::module &m) {
    m.def("variant_func", &variant_func);
    py::class_<reuss::DetectorInterface,
               PyDetectorInterface /* <--- trampoline*/>(m, "DetectorInterface")
        .def(py::init<>())
        .def("start", &reuss::DetectorInterface::start)
        .def("stop", &reuss::DetectorInterface::stop)
        .def("set_gain", &reuss::DetectorInterface::set_gain)
        .def("set_period", &reuss::DetectorInterface::set_period)
        .def("get_period", &reuss::DetectorInterface::get_period);

    // Do we need to be able to access the methods on the dummy detector?
    py::class_<reuss::DummyDetector, reuss::DetectorInterface> DummyDetector(
        m, "DummyDetector");
    DummyDetector.def(py::init());

    py::class_<reuss::JungfrauDetector, reuss::DetectorInterface>
        JungfrauDetector(m, "JungfrauDetector");
    JungfrauDetector.def(py::init());
}