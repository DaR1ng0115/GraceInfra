//
// buffer.cpp
// GraceInfra
// 
// created by DaR1ng on 26-9-16

#include "buffer.h"
#include "exceptions.h"
#include <limits>
#include <cstddef>
#include <cstdlib>

Buffer::Buffer(size_t numel)
:numel_(numel) {
    if(numel == 0) return;
    if(numel > std::numeric_limits<size_t>::max()/sizeof(float)) throw poerror::OverflowException("Numeric overflow");
    data_ = static_cast<float*>(malloc(numel_*sizeof(float)));
    if(data_ == nullptr) throw poerror::MemoryException("Memory allocation failed");
}

Buffer::Buffer(size_t numel, float fill_data)
:numel_(numel) {
    if(numel == 0) return;
    if(numel > std::numeric_limits<size_t>::max()/sizeof(float)) throw poerror::OverflowException("Numeric overflow");
    data_ = static_cast<float*>(malloc(numel_*sizeof(float)));
    if(data_ == nullptr) throw poerror::MemoryException("Memory allocation failed");
    for(size_t i=0; i<numel; ++i) {
        data_[i] = fill_data;
    }
}

Buffer::Buffer(const Buffer& other)
:numel_(other.numel_) {
    ref_count_ = 1;
    data_ = static_cast<float*>(malloc(numel_*sizeof(float)));
    if(data_ == nullptr) throw poerror::MemoryException("Memory allocation failed");
    std::copy(other.data_, other.data_+other.numel_, data_);
}

Buffer::~Buffer() {
    free(data_);
    data_ = nullptr;
}

void Buffer::release() {
    ref_count_--;
    if(ref_count_ == 0) delete this;
}