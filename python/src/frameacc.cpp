#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "reuss/DataSpan.h"
#include "reuss/DetectorInterface.h"
#include "reuss/pedestal.h"
#include "span_helpers.h"

#include "reuss/FrameAccumulator.h"

namespace py = pybind11;

using DataType = float;

// py::array_t<double> wrap(dobule *data){
//         // Create a Python object that will free the allocated
//         // memory when destroyed:
//         py::capsule free_when_done(data, [](void *f) {
//             double *foo = reinterpret_cast<double *>(f);
//             std::cerr << "Element [0] = " << foo[0] << "\n";
//             std::cerr << "freeing memory @ " << f << "\n";
//             delete[] foo;
//         });

//         return py::array_t<double>(
//             {100, 1000, 1000}, // shape
//             {1000*1000*8, 1000*8, 8}, // C-style contiguous strides for double
//             foo, // the data pointer
//             free_when_done); // numpy array references this parent
// }

void init_frameacc(py::module &m) {


//     m.def("npfunc", []() {
//         // Allocate and initialize some data; make this big so
//         // we can see the impact on the process memory use:
//         constexpr size_t size = 100*1000*1000;
//         double *foo = new double[size];
//         for (size_t i = 0; i < size; i++) {
//             foo[i] = (double) i;
//         }

//         return wrap(foo);

//         // // Create a Python object that will free the allocated
//         // // memory when destroyed:
//         // py::capsule free_when_done(foo, [](void *f) {
//         //     double *foo = reinterpret_cast<double *>(f);
//         //     std::cerr << "Element [0] = " << foo[0] << "\n";
//         //     std::cerr << "freeing memory @ " << f << "\n";
//         //     delete[] foo;
//         // });

//         // return py::array_t<double>(
//         //     {100, 1000, 1000}, // shape
//         //     {1000*1000*8, 1000*8, 8}, // C-style contiguous strides for double
//         //     foo, // the data pointer
//         //     free_when_done); // numpy array references this parent
//     });




//     py::class_<reuss::ImageData<DataType, 2>>(m, "ImageData", py::buffer_protocol())
//    .def_buffer([](reuss::ImageData<DataType, 2> &m) -> py::buffer_info {
//         return py::buffer_info(
//             m.data(),                               /* Pointer to buffer */
//             sizeof(DataType),                          /* Size of one scalar */
//             py::format_descriptor<DataType>::format(), /* Python struct-style format descriptor */
//             2,                                      /* Number of dimensions */
//             m.shape(),                              /* Buffer dimensions */
//             m.byte_strides()
//         );
//     });

    py::class_<reuss::FrameAccumulator<DataType>> FrameAccumulator(m, "FrameAccumulator");
    FrameAccumulator.def(py::init())
    .def("accumulate", &reuss::FrameAccumulator<DataType>::accumulate)
    .def("set_pedestal", &reuss::FrameAccumulator<DataType>::set_pedestal);
}
