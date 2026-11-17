//
// tensor.cpp
// GraceInfra
// 
// Created by DaR1ng on 26-7-10

#include <cassert>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <limits>
#include "tensor.h"
#include "exceptions.h"
#include "slice.h"

Tensor::Tensor(const std::vector<int64_t> &shape) 
:shape_(shape), strides_(shape.size(), 1) {
    for(auto dim : shape) {
        if(dim < 0) throw poerror::ValueException(
            "Negative dimension is not supported: " + std::to_string(dim));
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
        if(dim < 0) throw poerror::ValueException(
            "Negative dimension is not supported: " + std::to_string(dim));
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
    if(dim < 0 || dim >= static_cast<int64_t>(shape_.size())) throw poerror::IndexException(
        "Illegal dimensions: " + std::to_string(dim));
    return shape_[dim];
}

const std::vector<int64_t>& Tensor::shape() const {
    return shape_;
}

const std::vector<int64_t>& Tensor::strides() const {
    return strides_;
}

int64_t Tensor::ndim() const {
    return shape_.size();
}

float& Tensor::at(const std::vector<int64_t>& indices) {
    if(indices.size() != shape_.size() || indices.size() == 0) throw poerror::IndexException(
        "Number of arguments: " + std::to_string(indices.size()) + " are not matching");
    int dim = 0;
    for(auto idx : indices) {
        if(idx < 0 || idx >= shape_[dim]) throw poerror::IndexException(
            "Illegal dimensions: " + std::to_string(idx));
        dim++;
    }
    size_t idx = 0;
    for(size_t i=0; i<shape_.size(); ++i) {
        idx += indices[i] * strides_[i];
    }
    return buffer_ -> data_[idx+offset_];
}

const float& Tensor::at(const std::vector<int64_t>& indices) const {
    if(indices.size() != shape_.size() || indices.size() == 0) throw poerror::IndexException(
        "Number of arguments: " + std::to_string(indices.size()) + " are not matching");
    int dim = 0;
    for(auto idx : indices) {
        if(idx < 0 || idx >= shape_[dim]) throw poerror::IndexException(
            "Illegal dimensions: " + std::to_string(idx));
        dim++;
    }
    size_t idx = 0;
    for(size_t i=0; i<shape_.size(); ++i) {
        idx += indices[i] * strides_[i];
    }
    return buffer_ -> data_[idx+offset_];
}

Tensor Tensor::slice(std::vector<std::variant<int64_t, Slice, All>> args) const {
    if(args.size() != shape_.size()) throw poerror::IndexException(
        "Number of indices: " + std::to_string(args.size()) + " does not match tensor rank");
    Tensor sliceTensor(*this, AliasTag{});
    std::vector<int64_t> newShape{};
    std::vector<int64_t> newStrides{};
    int64_t dim = 0;
    for(auto each : args) {
        std::visit( overloaded {
            [&](int64_t arg) {
                if(arg >= shape_[dim] || arg < 0) throw poerror::IndexException(
                    "Illegal dimensions: " + std::to_string(arg));
                sliceTensor.offset_ += arg * strides_[dim];
            },
            [&](Slice& arg) {
                if(arg.start_ < 0) throw poerror::IndexException(
                    "Start must be positive: " + std::to_string(arg.start_));
                if(arg.end_ < 0 || arg.end_ > shape_[dim]) throw poerror::IndexException(
                    "Illegal dimensions: " + std::to_string(arg.end_));
                if(arg.start_ > arg.end_) throw poerror::IndexException(
                    "End must be greater than start");
                if(arg.step_ <= 0) throw poerror::StepException("Step must be a positive number");
                int64_t span = arg.end_ - arg.start_;
                int64_t slice_length = span == 0 ? 0 : 1 + (span - 1) / arg.step_;
                newShape.push_back(slice_length);
                newStrides.push_back(strides_[dim] * arg.step_);
                sliceTensor.offset_ += arg.start_ * strides_[dim];
            },
            [&](All) {
                newShape.push_back(shape_[dim]);
                newStrides.push_back(strides_[dim]);
            }
        }, each);
        dim++;
    }
    sliceTensor.shape_ = std::move(newShape);
    sliceTensor.strides_ = std::move(newStrides);
    return sliceTensor;
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

int64_t Tensor::logical2phy(int64_t logical_idx) const {
    assert(logical_idx >= 0 && logical_idx < numel());
    int64_t coordinate = 0;
    int64_t physical_idx = offset_;
    for(int i=static_cast<int64_t>(shape_.size())-1; i>=0; --i) {
        coordinate = logical_idx % shape_[i];
        logical_idx = logical_idx / shape_[i];
        physical_idx += coordinate * strides_[i];
    }

    return physical_idx;
}

int64_t Tensor::logical2phy(int64_t logical_idx, const Tensor& other) const {
    assert(logical_idx >= 0 && logical_idx < numel());
    int64_t res_ndim     = static_cast<int64_t>(shape_.size());
    int64_t other_ndim   = static_cast<int64_t>(other.shape().size());
    int64_t physical_idx = other.offset();
    for(int64_t i=res_ndim-1, j=other_ndim-1; i>=0; --i,--j) {
        int64_t coordinate = logical_idx % shape_[i];
        logical_idx = logical_idx / shape_[i];
        if(j>=0 && other.shape_[j] != 1) {
            physical_idx += coordinate * other.strides_[j];
        }
    }
    return physical_idx;
}

bool Tensor::is_contiguous() const {
    int64_t expect_strides = 1;
    int64_t expect_strides_next = 0;
    if(shape_.size() == 0) return true;
    if(shape_.back() > 1 && strides_.back() != 1) return false;
    for(int64_t i=static_cast<int64_t>(shape_.size())-1; i>0; --i) {
        if(shape_[i-1] > 1) {
            expect_strides_next = expect_strides * shape_[i];
            if(expect_strides_next == strides_[i-1]) expect_strides = expect_strides_next;
            else return false;
        } else {
            expect_strides *= shape_[i];
        }
    }
    return true;
}

Tensor Tensor::operator+(const Tensor& other) const {
    return binary_op(other, [](float a, float b){ return a+b; });
}

Tensor Tensor::operator-(const Tensor& other) const {
    return binary_op(other, [](float a, float b){ return a-b; });
}

Tensor Tensor::operator*(const Tensor& other) const {
    return binary_op(other, [](float a, float b){return a*b; });
}

Tensor Tensor::operator/(const Tensor& other) const {
    return binary_op(other, [](float a, float b){return a/b; });
}

Tensor& Tensor::operator=(const Tensor& other) {
    if(this == &other) return *this;
// 暂时删除拷贝赋值的快速路径，因offset和step逻辑暂时未稳定
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
    if(old_buffer != nullptr) old_buffer -> release();

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