//
// tensor.h
// GraceInfra
//
// Created by DaR1ng on 26-7-10


#pragma once
#include <array>
#include <cstddef>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include "buffer.h"
#include "exceptions.h"


class Tensor {
private:
    std::vector<int64_t> shape_ = {}; 
    std::vector<int64_t> strides_ = {};
    Buffer* buffer_ = nullptr;
    size_t offset_ = 0;
    static bool safe_multiply(int64_t a, int64_t b, int64_t& result);
    
public:
    Tensor();
    explicit Tensor(const std::vector<int64_t> &shape);
    Tensor(const std::vector<int64_t> &shape, float fill_data);
    Tensor(const Tensor& other);
    Tensor(Tensor&& other) noexcept;
    ~Tensor();

    int64_t shape(int64_t dim) const;
    const std::vector<int64_t>& shape() const;
    const std::vector<int64_t>& strides() const;
    size_t offset() const;
    int64_t numel() const;

    Tensor operator+(const Tensor& other) const;
    Tensor& operator=(const Tensor& other);
    Tensor& operator=(Tensor&& other) noexcept;

    template<typename... Args>
    float& operator()(Args... args) {
        constexpr size_t numOfArgs = sizeof...(args);
        std::array<int64_t, numOfArgs> arr = {args...};
        if(numOfArgs == 0 || numOfArgs != shape_.size()) throw poerror::DimensionException("Illegal dimensions");
        for(size_t i=0; i<numOfArgs; ++i) {
            if(arr[i] < 0 || arr[i] >= shape_[i]) throw poerror::DimensionException("Illegal dimensions");
        }
        size_t idx = 0;
        for(size_t i=0; i<numOfArgs; ++i) {
            idx += arr[i] * strides_[i];
        }
        if(idx+offset_>numel()) throw poerror::DimensionException("Illegal dimensions");
        return buffer_->data_[offset_+idx];
    }

    template<typename... Args>
    const float& operator()(Args... args) const {
        constexpr size_t numOfArgs = sizeof...(args);
        std::array<int64_t, numOfArgs> arr = {args...};
        if(numOfArgs == 0 || numOfArgs != shape_.size()) throw poerror::DimensionException("Illegal dimensions");
        for(size_t i=0; i<numOfArgs; ++i) {
            if(arr[i] < 0 || arr[i] >= shape_[i]) throw poerror::DimensionException("Illegal dimensions");
        }
        size_t idx = 0;
        for(size_t i=0; i<numOfArgs; ++i) {
            idx += arr[i] * strides_[i];
        }
        if(idx+offset_>numel()) throw poerror::DimensionException("Illegal dimensions");
        return buffer_->data_[offset_+idx];
    }
};