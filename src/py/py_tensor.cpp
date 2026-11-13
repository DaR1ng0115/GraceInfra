//
// py_tensor.cpp
//
// Created by DaR1ng on 26-10-28

#include "exceptions.h"
#include "tensor.h"

#include <cstdint>
#include <cstring>
#include <exception>
#include <vector>
#include <variant>

#include <nanobind/nanobind.h>
#include <Python.h>
#include <pyerrors.h>
#include <nanobind/stl/vector.h>

namespace nb = nanobind;

static std::vector<int64_t> process_negative(const Tensor& t, nb::tuple keys) {
    std::vector<int64_t> indices;
    int64_t dim = 0;
    for(auto each : keys) {
        if(!nb::isinstance<nb::int_>(each)) throw nb::type_error("Indices must be integer");
        int64_t item = nb::cast<int64_t>(each);
        if(item < 0) item += t.shape(dim);
        indices.push_back(item);
        ++dim;
    }  
    return indices;
}

NB_MODULE(grace, m) {
    nb::class_<Tensor>(m, "Tensor")
    .def(nb::init<const std::vector<int64_t>&>())
    .def(nb::init<const std::vector<int64_t>&, float>())
    .def(nb::init<const Tensor&>())
    .def_prop_ro("numel", &Tensor::numel)
    .def_prop_ro("shape", [](const Tensor& t) {
        nb::object dims = nb::cast(t.shape());
        return  nb::tuple(dims);
    })
    .def_prop_ro("ndim", &Tensor::ndim)
    .def("__getitem__", [](const Tensor& t, nb::handle key) -> nb::object {
        nb::tuple keys;
        if(nb::isinstance<nb::tuple>(key)) {
            keys = nb::borrow<nb::tuple>(key);
        } else {
            keys = nb::make_tuple(key);
        }
        bool has_slice = false;
        for(auto item : keys) {
            if(nb::isinstance<nb::int_>(item)) {}
            else if(nb::isinstance<nb::slice>(item)) {
                has_slice = true;
            } else {
                throw nb::type_error("Indices must be integers or slices");
            }
        }

        if(has_slice) {
            int64_t dim = 0;
            std::vector<std::variant<int64_t, Slice, All>> indices;
            for(auto item : keys) {
                if(nb::isinstance<nb::int_>(item)) {
                    int64_t integer = nb::cast<int64_t>(item);
                    if(integer < 0) integer += t.shape(dim);
                    indices.emplace_back(integer);
                } else {
                    nb::slice slc = nb::cast<nb::slice>(item);
                    auto [start, stop, step, slice_length] = slc.compute(t.shape(dim));
                    if(step <= 0) throw nb::value_error("Slice step must be positive");
                    if(start > stop) stop = start;
                    indices.emplace_back(Slice{start, stop, step});
                }
                ++dim;
            }
            return nb::cast(t.slice(indices));
        } else {
            std::vector<int64_t> indices = process_negative(t, keys);
            return nb::cast(t.at(indices));
        }
    })
    .def("__setitem__", [](Tensor& t, nb::handle key, float value) {
        nb::tuple keys;
        if(nb::isinstance<nb::tuple>(key)) {
            keys = nb::borrow<nb::tuple>(key);
        } else {
            keys = nb::make_tuple(key);
        }
        std::vector<int64_t> indices = process_negative(t, keys);
        t.at(indices) = value;
    })
    .def("__add__", [](const Tensor& lhs, const Tensor& rhs) {
        return lhs + rhs;
    });
    nb::register_exception_translator([](const std::exception_ptr& p, void*) {
        try {
            std::rethrow_exception(p);
        } catch(const poerror::IndexException& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch(const poerror::ValueException& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch(const poerror::OverflowException& e) {
            PyErr_SetString(PyExc_OverflowError, e.what());
        } catch(const poerror::MemoryException& e) {
            PyErr_SetString(PyExc_MemoryError, e.what());
        }
    });
}