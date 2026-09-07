// 
// exceptions.h
// GraceInfraj
// 
// Created by DaR1ng on 26-9-3

#pragma once

#include <exception>
#include <string>

namespace poerror {
    class AppException : public std::exception {
    private:
        std::string msg_;
    public:
        explicit AppException(const std::string& msg)
        :msg_(msg) {}
        const char * what() const noexcept override {
            return msg_.c_str();
        }
    };

    class MemoryException : public AppException {
    public:
        explicit MemoryException(const std::string& msg)
        :AppException(msg) {}
    };

    class DimensionException : public AppException {
    public:
        explicit DimensionException(const std::string& msg)
        :AppException(msg) {}
    };

    class OverflowException : public AppException {
    public:
        explicit OverflowException(const std::string& msg)
        :AppException(msg) {}
    };
}
