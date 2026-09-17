// 
// buffer.h
// GraceInfra
// 
// created by DaR1ng on 26-9-16
#pragma once

#include "tensor.h"
#include <cstddef>
class Buffer{
private:
    friend class Tensor;
    size_t numel_ = 0;
    size_t ref_count_ = 1;
    float* data_ = nullptr;

    Buffer() = default;
    explicit Buffer(size_t numel);
    Buffer(size_t numel, float fill_data);
    ~Buffer();            
};