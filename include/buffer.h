// 
// buffer.h
// GraceInfra
// 
// created by DaR1ng on 26-9-16
#pragma once

class Tensor;

// buffer是tensor的底层数据，因此buffer不应暴露给外界使用，这里采用了友元和private的方式来实现

#include <cstddef>
class Buffer{
private:
    friend class Tensor;
    size_t numel_ = 0;
    size_t ref_count_ = 1;
    float* data_ = nullptr;

    Buffer() = delete;
    explicit Buffer(size_t numel);
    Buffer(size_t numel, float fill_data);
    Buffer(const Buffer& other);
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(Buffer&&) = delete;
    ~Buffer();        
    
    void release();
};