#include "tensor.h"

#include <cstdint>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>

namespace nb = nanobind;

NB_MODULE(grace, m) {
    nb::class_<Tensor>(m, "Tensor")
    .def(nb::init<const std::vector<int64_t>&>())
    .def(nb::init<const std::vector<int64_t>&, float>())
    .def(nb::init<const Tensor&>())
    .def("numel", &Tensor::numel)
    .def_prop_ro("shape", [](const Tensor& t) {
        nb::object dims = nb::cast(t.shape());
        return  nb::tuple(dims);
    })
    .def("__getitem__", [](const Tensor& t, nb::tuple key) {
        auto indices = nb::cast<std::vector<int64_t>>(key);
        return t.at(indices);
    })
    .def("__setitem__", [](Tensor& t, nb::tuple key, float value) {
        auto indices = nb::cast<std::vector<int64_t>>(key);
        t.at(indices) = value;
    });
}