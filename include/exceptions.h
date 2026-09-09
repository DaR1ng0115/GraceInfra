// 
// exceptions.h
// GraceInfraj
// 
// Created by DaR1ng on 26-9-3

#pragma once

#include <stdexcept>

namespace poerror {
    class GraceException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class MemoryException : public GraceException {
    public:
        using GraceException::GraceException;
    };

    class DimensionException : public GraceException {
    public:
        using GraceException::GraceException;
    };

    class OverflowException : public GraceException {
    public:
        using GraceException::GraceException;
    };
}
