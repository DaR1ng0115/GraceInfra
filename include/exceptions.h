// 
// exceptions.h
// GraceInfraj
// 
// Created by DaR1ng on 26-9-3

#pragma once

#include <exception>
#include <new>
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

    class IndexException : public GraceException {
    public:
        using GraceException::GraceException;
    };

    class ValueException : public GraceException {
    public:
        using GraceException::GraceException;
    };

    class OverflowException : public GraceException {
    public:
        using GraceException::GraceException;
    };

    class StepException : public GraceException {
    public:
        using GraceException::GraceException;
    };

    class ArgsException : public GraceException {
    public:
     using GraceException::GraceException;
    };
}
