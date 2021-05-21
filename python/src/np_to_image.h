/* offer translation between numpy and ImageData/DataSpan classes */

#pragma once

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "reuss/DataSpan.h"
#include "reuss/ImageData.h"

namespace py = pybind11;

// accept a numpy array and pass a view to a c++ function
template <typename T, ssize_t Ndim>
py::array_t<T> return_image_data(reuss::ImageData<T, Ndim> *image) {

    py::capsule free_when_done(image, [](void *f) {
        reuss::ImageData<T, Ndim> *foo =
            reinterpret_cast<reuss::ImageData<T, Ndim> *>(f);
        // std::cerr << "Element [0] = " << foo[0] << "\n";
        // std::cerr << "freeing memory @ " << f << "\n";
        delete foo;
    });

    return py::array_t<T>(
        image->shape(),              // shape
        image->byte_strides(), // C-style contiguous strides for double
        image->data(),                            // the data pointer
        free_when_done);                // numpy array references this parent
}
