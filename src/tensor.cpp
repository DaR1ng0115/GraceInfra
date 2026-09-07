//
// tensor.cpp
// GraceInfra
// 
// Created by DaR1ng on 26-7-10

#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <utility>
#include <limits>
#include "../include/tensor.h"
#include "../include/exceptions.h"


Tensor::Tensor()
:shape_(), strides_(), data_(nullptr) {}

// 在理解下面这些代码之前，请再次回顾一下各个变量的含义。
// strides 用于映射到一维索引,shape 各维度数,data 实际存储数组的一维顺序数组的首地址,length 元素总数
// 接下来我来说明一下难以理解的部分
// strides : 仿照matrix二维映射到一维索引的思考，我们也可以推导出类似的规律。
// 拿个简单的例子，假设一个Tensor的三个维度分别对应时分秒，那么我如果想求第x小时第y分钟第z秒怎么求？
// 我想你应该很快就能说出来，3600x+60y+x，但为什么呢？
// 如第3小时第25分钟第5秒，前面的3个小时肯定都要算上，也就是3x3600，25分钟也要算上，即25x60，最后算上5秒
// 这就是索引映射公式，总结成抽象的规律，也就是说，一个长度为n的shape数组，对应的第x项的索引应该是shape数组的
// 第x项后面所有元素的乘积。
// 比如shape[4, 5, 6, 7] ---> strides[5x6x7=210, 6x7=42, 7, 1]（strides的最后一项总是1）


// 三种情况：空对象，无元素tensor，异常shape
// tensor对象状态约定：
// 1.空对象，shape为空，strides为空，length为0，data为nullptr
// 2.无元素tensor（shape中包含0维度），shape为原始状态，strides为根据公式计算得出，length为0，data为nullptr
Tensor::Tensor(const std::vector<int64_t> &shape) 
:shape_(shape), strides_(shape.size(), 1), data_(nullptr) {
    for(auto dim : shape) {
        if(dim < 0) throw poerror::DimensionException("Illegal dimensions");
    }
    for(int64_t j=static_cast<int64_t>(shape.size())-1; j>0; --j) {
        bool issafe = safe_multiply(strides_[j], shape_[j], strides_[j-1]);
        if(!issafe) throw poerror::OverflowException("Numeric overflow");
    }
// calloc和malloc类似，不同的是，一个是参入的参数不一样，分别是数据的数量和单个数据大小，另一个是默认所有元素填充为0
// 为什么使用static_cast而不使用(float*)?
// （float*）是C语言风格，其权限很大，可能改变变量的const特性，甚至进行一些很危险的类型转换
// 而static_cast则更加安全，并且它的功能在该场景下也能完全胜任，因此选择static_cast
    int64_t numel = this->numel();
    if(numel > 0) {
        data_ = static_cast<float*>(calloc(numel, sizeof(float)));
        if(data_ == nullptr) throw poerror::MemoryException("Memory allocation failed");
    }
}

Tensor::Tensor(const std::vector<int64_t> &shape, float fill_data)
:shape_(shape), strides_(shape.size(), 1), data_(nullptr) {
    for(auto dim : shape) {
        if(dim < 0) throw poerror::DimensionException("Illegal dimensions");
    }
    for(int64_t j=static_cast<int64_t>(shape.size())-1; j>0; --j) {
        bool issafe = safe_multiply(strides_[j], shape_[j], strides_[j-1]);
        if(!issafe) throw poerror::OverflowException("Numeric overflow");
    }
    int64_t numel = this->numel();
    if(numel > 0) {
        data_ = static_cast<float*>(malloc(numel*sizeof(float)));
        if(data_ == nullptr) throw poerror::MemoryException("Memory allocation failed");
        for(int64_t i=0; i<numel; ++i) {
            data_[i] = fill_data;
        }
    }
}

// 拷贝构造函数，使用std::copy实现深拷贝
// 注，浅拷贝和深拷贝:
// 实际上，这里有两层语义，并不是只拷贝指针就是浅拷贝，拷贝指针指向的数据就是深拷贝，第一个是相对意义上的，
// 上面所说的通过std::copy实现深拷贝即为相对意义上的，即它只会拷贝所给对象内存放的数据，如果这个对象存放的
// 数据还是指针，比如vector<float*>，那么严格来说，依然是浅拷贝
// 第二个是绝对意义上的，上述的对于vector<float*>仍然是浅拷贝，就是绝对意义上的。绝对的深拷贝应该是无论有
// 多少的嵌套指针，都会拷贝到最后一层数据，即便最后一层数据也是指针(均为nullptr)。
// 我们常说vector的拷贝和std::copy是深拷贝，其实是相对意义上的，对于无指针嵌套的对象的确是深拷贝。

Tensor::Tensor(const Tensor& other)
:shape_(other.shape()), strides_(other.strides()), data_(nullptr) {
// 判定空对象和无元素对象
    if(other.numel() != 0) {
        data_ = static_cast<float*>(malloc(this->numel()*sizeof(float)));
        if(data_ == nullptr) throw poerror::MemoryException("Memory allocation failed");
        std::copy(other.data(), other.data()+other.numel(), data_);
    }
}

// 此处是移动构造函数
// 移动语义我在matrix.cpp中已说明过，这里借助实际的函数再讲解一下
// "&&"的意思是右值，简单来说就是一个临时的对象，比如int a = 1，此处的a是左值，1是右值，当赋值结束，1就被销毁了
// 这和移动构造有什么关系呢？重点在于，移动构造在于"移动"，不会发生拷贝，也就是说，如果传入的是一个左值（持久对象）
// 那么这个左值对象就变成空对象了。如果想保留传入对象，那就必须用拷贝构造。

Tensor::Tensor(Tensor&& other) noexcept
// std::move()并不是移动本身，而是将一个左值对象转化为右值对象，以完成移动语义
// 移动语义的根本之处其实在于这两行:
// data_ = other.data_;
// other.data_ = nullptr;
// 第一步是所有权转移，第二步就是原指针置空
:shape_(std::move(other.shape_)), strides_(std::move(other.strides_)), data_(other.data_) {
// 不知大家有没有听过这样的说法：对于标准库对象使用std::move之后，会自动释放原对象
// 我当时听了之后，让我误以为标准库保证原对象的data一定会是nullptr
// 但是我今天去查看了cppreference之后，发现并不是这么一回事
// 文档中是这么说的‘除非另有说明，所有被移动的标准库对象都处于“有效但未指定的状态”，
// 这意味着对象的类不变量保持（因此不带前置条件的函数，例如赋值运算符，可以在对象被移动后安全地使用）’
// 针对于“有效但未指定的状态”，其中有效指的是这个对象仍是可以析构的，并且可以被安全的赋予新值
// 而未指定则指的是其内部状态是不保证的，比如你读取它的第0个数据，可能导致访问越界，可能访问到原来的数据
// 也有可能访问到乱码，而该对象的data同样如此，其data在移动语义之后不保证为nullptr，所以上述行为均是未定义行为
// 但在GraceInfra中，我们约定空对象内部数据清空，因此，在这种情况下，我们需要对strides和shape使用.clear()
// 在逻辑上保持约定，需要注意的是，我们仍无法保证原对象的data为nullptr，只是一种逻辑上的约定
    other.strides_.clear();
    other.shape_.clear();
    other.data_ = nullptr;
}

// 此处的析构函数切不可忘记实现，否则会发生内存泄漏，这在深度学习训练中尤其严重
// 注：析构函数在C++11之后默认noexcept，同时在逻辑上析构函数也不该抛出异常
Tensor::~Tensor() {
    free(data_);
    data_ = nullptr;
}

int64_t Tensor::shape(int64_t dim) const {
    if(dim < 0 || dim >= static_cast<int64_t>(shape_.size())) throw poerror::DimensionException("Illegal dimensions");
    return shape_[dim];
}

// 返回副本与返回const引用
// 返回副本的优点是用户可以得到独立副本，可以对其进行操作，但缺点是每次调用都会拷贝一次，有额外开销
// 返回const引用解决了这个问题，但用户不可以操作对象

const std::vector<int64_t>& Tensor::shape() const {
    return shape_;
}

const std::vector<int64_t>& Tensor::strides() const {
    return strides_;
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

float* Tensor::data() {
    return data_;
}

const float* Tensor::data() const {
    return data_;
}

bool Tensor::safe_multiply(int64_t a, int64_t b, int64_t& result) {
    if(a>0) {
        if(b>0 && a > std::numeric_limits<int64_t>::max() / b) return false;
        if(b<0 && a < std::numeric_limits<int64_t>::min() / b) return false;
    }
    if(a<0) {
        if(b>0 && a < std::numeric_limits<int64_t>::min() / b) return false;
        if(b<0 && a > std::numeric_limits<int64_t>::max() / b) return false;
    }
    result = a*b;
    return true;
}

Tensor Tensor::operator+(const Tensor& other) const {
    if(shape_ != other.shape()) throw poerror::DimensionException("Dimensions mismatch");
    Tensor res(shape_);
    int64_t numel = this->numel();
    for(int64_t i=0; i<numel; ++i) {
        res.data_[i] = data_[i] + other.data_[i];
    }
    return res;
}

Tensor& Tensor::operator=(const Tensor& other) {
    if(this == &other) return *this;
    int64_t numel = this->numel();
    if(shape_ == other.shape() && numel != 0 && other.numel() != 0) {
        std::copy(other.data(), other.data()+other.numel(), data_);
        assert(strides_ == other.strides());
        assert(numel == other.numel());
    }
    else {
        Tensor temp(other);
        std::swap(shape_, temp.shape_);
        std::swap(strides_, temp.strides_);
        std::swap(data_, temp.data_);
    }
    return *this;
}

// 此处的移动语义可以和移动构造对照着理解，过程是类似的。
// 再次提醒，对于非RAII属性，在进行移动之后，一定要将其清零或置空
// 否则很容易会导致double free

Tensor& Tensor::operator=(Tensor&& other) noexcept {
    if(this == &other) return *this;
    free(data_);
    shape_ = std::move(other.shape_);
    strides_ = std::move(other.strides_);
    data_ = other.data_;
    other.shape_.clear();
    other.strides_.clear();
    other.data_ = nullptr;
    return *this;
}

// 在实现对括号的重载的时候，我也遇到了一个技术选型的问题，也许你可以从我的纠结中得到不一样的思考。
// 选择1: 接收可变的参数，使用时类似于tensor(x, y, z, ...)，这是最自然的使用方式，符合多维张量的访问方式
// 但选择它需要引入可变参模版，这会打乱教学进度，提前进入模版概念
// 选择2: 使用vector<int64_t> shape传参，也可以实现可变参数
// 但使用太麻烦，每次访问都要构造一个vector，而且后续如果重构，成本非常高
// 选择3: 固定二维访问，能更好的衔接前面的学习，并且重构成本不高，改为可变参模版，前面的二维索引访问仍可用
// 因而此处选择第三种方式

float& Tensor::operator()(int64_t idx0, int64_t idx1) {
    if(shape_.size() != 2 || idx0 < 0 || idx1 < 0 || idx0 >= shape_[0] || idx1 >= shape_[1])
        throw poerror::AppException("Unsupport temporarily");
    return data_[idx0*strides_[0] + idx1*strides_[1]];
}

const float& Tensor::operator()(int64_t idx0, int64_t idx1) const {
    if(shape_.size() != 2 || idx0 < 0 || idx1 < 0 || idx0 >= shape_[0] || idx1 >= shape_[1])
        throw poerror::AppException("Unsupport temporarily");
    return data_[idx0*strides_[0] + idx1*strides_[1]];
}



/*
至此，tensor类的定义和功能基本实现
*/