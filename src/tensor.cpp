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

// 在实现了切片之后，每个tensor的索引不一定是实际在buffer中的索引，我们改变的是tensor所携带的信息，但当我们需要直接
// 读取或修改buffer的数据的时候，比如在加法的运算符重载中，直接写buffer_[i+offset_]是不对的，因为在此处我们并没有考
// 虑到shape和step。
// 因此需要将逻辑索引先转化为坐标的形式，然后通过strides计算在buffer上的实际一维索引

int64_t Tensor::logical2phy(int64_t logical_idx) const {
    if(logical_idx < 0 || logical_idx >= numel()) throw poerror::DimensionException("Idx out of range");
    int64_t coordinate = 0;
    int64_t physical_idx = offset_;
    for(int i=static_cast<int64_t>(shape_.size())-1; i>=0; --i) {
        coordinate = logical_idx % shape_[i];
        logical_idx = logical_idx / shape_[i];
        physical_idx += coordinate * strides_[i];
    }

    return physical_idx;
// 不知道读者看到这里的时候，会不会想到:如果在加法运算符重载的循环中，每次都调用这个函数计算物理索引，可能会显著减慢运行速度
// 但我们暂且不谈这个算法本身是否可以优化到更好，我们可以思考一下，哪些情况下实际上不需要转化
// 最先能想到的大概是完整的tensor，内部没有经过任何切片；其次被切片了，但内部有大块的连续索引，尤其是无step的，这实际上也能避免一些转换
// 我们现在先只对整块完全连续的tensor进行优化，简单来说，就是用一个条件判断来判断其是否拥有一个完整的连续数据块
// 比如对一个5x5的矩阵，经过切片之后，变成了3x5的矩阵，offset变为10，形状变为(3, 5)，但是你可以发现，(5, 5)和(3, 5)计算出来的
// strides是完全一样的，因此我们可以借助这个性质，通过计算expect_strides是否符合预期
}

int64_t Tensor::logical2phy(int64_t logical_idx, std::vector<int64_t> &shape) const {
    int64_t numel = 1;
    for(auto dim : shape) {
        numel *= dim;
    }
    if(logical_idx < 0 || logical_idx >= numel) throw poerror::DimensionException("Idx out of range");
}

bool Tensor::is_contiguous() const {
    int64_t expect_strides = 1;
    int64_t expect_strides_next = 0;
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

// 这一章中，我们需要实现张量加法的广播机制，此处不再解释广播的概念，从哪些情况可以广播开始
// 1.如(4, 5),(1, 5); 2.如(4, 5),(5,) 但注意，因为对齐顺序是从右往左对齐，所以(4, 5),(4,)不符合广播，原因后续会解释
// 3.如(4, 1),(1, 5); 4.如(4, 5),2（标量）

// 为什么对齐顺序是从右往左？
// 首先我们需要找到源头，在张量的维度中，外层维度（即左边的维度）的语义是容器，内层维度是内容。
// 举个例子:一个(2, 3)的矩阵
// [
//  [1, 2, 3]
//  [4, 5, 6]
// ]
// 在我们使用习惯中，[1, 2, 3],[4, 5, 6]是两个同类型的向量数据，而'2'则是描述有多少个这样的向量
// 那么如果有一个形状为(3,)的向量，比如[1, 3, 5]，我们是不是可以理解为：将这一个向量“复制”两份，加到原来的矩阵中
// 因此，这个从右往左对齐的逻辑是由人为约定的容器和内容的定义所决定的

// 现在，我们可以梳理一下，在进行广播的时候，应该有哪些步骤
// 首先是整个流程需要由遍历循环决定，即遍历进行加法的张量的shape
// 在该循环中，应进行两种操作:1.右对齐，检查对应的维度是否相等，如果不相等，其中一个维度是否是1。
// 2.当均存在的维度遍历完成后且符合广播时，按照广播规则进行广播。
Tensor Tensor::operator+(const Tensor& other) const {
    const auto& lhs_tensor = *this;
    const auto& rhs_tensor = other;
    size_t max = std::max(lhs_tensor.shape_.size(), rhs_tensor.shape_.size());
    size_t lhs_size = lhs_tensor.shape_.size();
    size_t rhs_size = rhs_tensor.shape_.size();
    std::vector<int64_t> shape(max, 1);
    for(int64_t k=0; k<max; ++k) {
        int64_t lhs_dim = k<lhs_size ? lhs_tensor.shape_[lhs_size-1-k] : 1;
        int64_t rhs_dim = k<rhs_size ? rhs_tensor.shape_[rhs_size-1-k] : 1;
        if(lhs_dim != rhs_dim && lhs_dim != 1 && rhs_dim != 1) throw poerror::DimensionException("Dimensions can not be broadcast");
        shape[max-1-k] = lhs_dim == 1 ? rhs_dim : lhs_dim;
    }
    Tensor res(shape);
    int64_t numel = res.numel();
    bool lhs_is_contiguous = this->is_contiguous();
    bool rhs_is_contiguous = other.is_contiguous();
    if(rhs_is_contiguous && lhs_is_contiguous) {
        for(int64_t i=0; i<numel; ++i) {
            res.buffer_->data_[i] = buffer_->data_[i+offset_] + other.buffer_->data_[i+other.offset_];
        }
    } else if(rhs_is_contiguous) {
        for(int64_t i=0; i<numel; ++i) {
            int64_t lhs_phy_idx = this->logical2phy(i);
            res.buffer_->data_[i] = buffer_->data_[lhs_phy_idx] + other.buffer_->data_[i+other.offset_];
        }
    } else if(lhs_is_contiguous) {
        for(int64_t i=0; i<numel; ++i) {
            int64_t rhs_phy_idx = other.logical2phy(i);
            res.buffer_->data_[i] = buffer_->data_[i+offset_] + other.buffer_->data_[rhs_phy_idx];
        }
    } else {
        for(int64_t i=0; i<numel; ++i) {
            int64_t lhs_phy_idx = this->logical2phy(i);
            int64_t rhs_phy_idx = other.logical2phy(i);
            res.buffer_->data_[i] = buffer_->data_[lhs_phy_idx] + other.buffer_->data_[rhs_phy_idx];
        }
    }
    return res;
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