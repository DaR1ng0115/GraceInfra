//
// tensor.cpp
// GraceInfra
// 
// Created by DaR1ng on 26-7-10

#include <cassert>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <limits>
#include "tensor.h"
#include "exceptions.h"

Tensor::Tensor(const std::vector<int64_t> &shape) 
:shape_(shape), strides_(shape.size(), 1) {
    for(auto dim : shape) {
        if(dim < 0) throw poerror::DimensionException("Illegal dimensions");
    }
    for(int64_t j=static_cast<int64_t>(shape.size())-1; j>0; --j) {
        bool issafe = safe_multiply(strides_[j], shape_[j], strides_[j-1]);
        if(!issafe) throw poerror::OverflowException("Numeric overflow");
    }
    int64_t numel = this->numel();
    if(numel > 0)  buffer_ = new Buffer(numel);
}

Tensor::Tensor(const std::vector<int64_t> &shape, float fill_data)
:shape_(shape), strides_(shape.size(), 1) {
    for(auto dim : shape) {
        if(dim < 0) throw poerror::DimensionException("Illegal dimensions");
    }
    for(int64_t j=static_cast<int64_t>(shape.size())-1; j>0; --j) {
        bool issafe = safe_multiply(strides_[j], shape_[j], strides_[j-1]);
        if(!issafe) throw poerror::OverflowException("Numeric overflow");
    }
    int64_t numel = this->numel();
    if(numel > 0)  buffer_ = new Buffer(numel, fill_data);
}


Tensor::Tensor(const Tensor& other)
:shape_(other.shape()), strides_(other.strides()), offset_(other.offset()) {
    if(other.numel() != 0) {
        buffer_ = new Buffer(*other.buffer_);
    }
    if(other.numel() == 0) {
        offset_ = 0;
    }
}

Tensor::Tensor(const Tensor& other, AliasTag) 
:shape_(other.shape()), strides_(other.strides()), offset_(other.offset()) {
    if(other.buffer_ != nullptr) {
        buffer_ = other.buffer_;
        buffer_ -> retain();
    }
}

Tensor::Tensor(Tensor&& other) noexcept
:shape_(std::move(other.shape_)), strides_(std::move(other.strides_)), buffer_(other.buffer_), offset_(other.offset()) {
    other.strides_.clear();
    other.shape_.clear();
    other.buffer_ = nullptr;
    other.offset_ = 0;
}

Tensor::~Tensor() {
    if(buffer_ != nullptr) {
        buffer_ -> release();
        buffer_ = nullptr;
    }
}

int64_t Tensor::shape(int64_t dim) const {
    if(dim < 0 || dim >= static_cast<int64_t>(shape_.size())) throw poerror::DimensionException("Illegal dimensions");
    return shape_[dim];
}

const std::vector<int64_t>& Tensor::shape() const {
    return shape_;
}

const std::vector<int64_t>& Tensor::strides() const {
    return strides_;
}

size_t Tensor::offset() const {
    return offset_;
}

int64_t Tensor::numel() const {
    if(shape_.empty()) return 0;
    int64_t res = 1;
    for(auto dim : shape_) {
        bool issafe = safe_multiply(res, dim, res);
        if(!issafe) throw poerror::OverflowException("Numeric overflow");
    }
    return res;
}

bool Tensor::safe_multiply(int64_t a, int64_t b, int64_t& result) {
    if(a>0 && b != 0) {
        if(b>0 && a > std::numeric_limits<int64_t>::max() / b) return false;
    }
    if(b == 0) {
        result = 0;
        return true;
    }
    result = a*b;
    return true;
}

Tensor Tensor::operator+(const Tensor& other) const {
    if(shape_ != other.shape()) throw poerror::DimensionException("Dimensions mismatch");
    Tensor res(shape_);
    int64_t numel = this->numel();
    for(int64_t i=0; i<numel; ++i) {
        res.buffer_->data_[i] = buffer_->data_[i+offset_] + other.buffer_->data_[i+other.offset_];
    }
    return res;
}

Tensor& Tensor::operator=(const Tensor& other) {
    if(this == &other) return *this;
// 暂时删除拷贝赋值的快速路径，因offset和step逻辑暂时未稳定
    int64_t numel = this->numel();   
    Tensor temp(other);
    std::swap(shape_, temp.shape_);
    std::swap(strides_, temp.strides_);
    std::swap(buffer_, temp.buffer_);
    std::swap(offset_, temp.offset_);
    return *this;
}

Tensor& Tensor::operator=(Tensor&& other) noexcept {
    if(this == &other) return *this;
    Buffer* old_buffer = buffer_;
    buffer_ = nullptr;
    old_buffer -> release();
    buffer_ = other.buffer_;
    other.buffer_ = nullptr;
    shape_ = std::move(other.shape_);
    strides_ = std::move(other.strides_);
    offset_ = other.offset();
    other.offset_ = 0;
    other.shape_.clear();
    other.strides_.clear();
    return *this;
}