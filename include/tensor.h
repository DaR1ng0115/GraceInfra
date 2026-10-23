//
// tensor.h
// GraceInfra
//
// Created by DaR1ng on 26-7-10


#pragma once
#include <array>
#include <cstddef>
#include <type_traits>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include "buffer.h"
#include "exceptions.h"
#include "slice.h"

// 此处的is_slice_v会用于后续的“()”重载, 因此建议看到那部分的时候再回头理解这里的is_slice_v
/*
- inline是内联关键字, 它有很多作用, 但在此处则用于避免重定义问题
- constexper则是编译期求值, 由于模版生成需要在编译期执行, 因此这里的is_slice_v也需要在编译期求出
- is_same_v是判断两个类型是否相同, 它是is_same<T, U>::value的简写形式
- decay用于类型退化, 比如const int退化成int, Slice&退化为Slice

因此, 整个逻辑组合起来的意思就是, 判断传入的模版参数T是否是Slice或者All类型, 只要是其中任意一个, 则is_slice_v为true, 否则为false
*/
template<typename T>
inline constexpr bool is_slice_v = std::is_same_v<std::decay_t<T>, Slice> || std::is_same_v<std::decay_t<T>, All>;

class Tensor {
private:
    struct AliasTag{};
    Tensor(const Tensor& other, AliasTag);

    std::vector<int64_t> shape_ = {}; 
    std::vector<int64_t> strides_ = {};
    Buffer* buffer_ = nullptr;
    size_t offset_ = 0;
    static bool safe_multiply(int64_t a, int64_t b, int64_t& result);
    int64_t logical2phy(int64_t logical_idx) const;
    int64_t logical2phy(int64_t logical_idx, const Tensor& other) const;
    bool is_contiguous() const;
    
public:
    Tensor() = default;
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

/*
接下来将使用可变参数模版实现任意维度的张量访问, 如果对于模版知识陌生, 可以先查看./doc/zh/architecture_ch02.md中模版的详解
- enable_if_t是enabel_if<>::value的简写, 其接受两个参数, 第一个参数是条件, 即此处的!(std::is_integral_v<Args> && ...), 其中的“...”
是折叠表达式, 并将条件判断用“&&”连接起来, 比如当Args为int, size_t时, 则相当于是(std::is_integral_v<int> || std::is_integral_v<size_t>).
第二个条件则是控制enable_if_t的第一个参数为真时其替换的类型, 比如当条件为真, 则可以视作<typename... Args, int = 0>, 此处的int = 0
相当于一个占位符, 没有实际作用, 在architecture_ch02.md中有详细解释
- sizeof...是一个固定的运算符, 用于计算参数包中参数的数量
*/

    template<typename... Args, std::enable_if_t<(std::is_integral_v<Args> && ...), int> = 0>
    float& operator()(Args... args) {
        constexpr size_t numOfArgs = sizeof...(args);
        std::array<int64_t, numOfArgs> arr = {args...};
        // 判断参数包里的参数是否合法
        if(numOfArgs == 0 || numOfArgs != shape_.size()) throw poerror::DimensionException("Illegal dimensions");
        for(size_t i=0; i<numOfArgs; ++i) {
            if(arr[i] < 0 || arr[i] >= shape_[i]) throw poerror::DimensionException("Illegal dimensions");
        }
        // 求实际索引值(需要加上offset)
        size_t idx = 0;
        for(size_t i=0; i<numOfArgs; ++i) {
            idx += arr[i] * strides_[i];
        }
        if(idx+offset_>=buffer_->numel_) throw poerror::DimensionException("Illegal dimensions");
        return buffer_->data_[offset_+idx];
    }

    // const重载版本
    template<typename... Args, std::enable_if_t<!(is_slice_v<Args> || ...), int> = 0>
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
        if(idx+offset_>=buffer_->numel_) throw poerror::DimensionException("Illegal dimensions");
        return buffer_->data_[offset_+idx];
    }

/*
这里是本章中遇到的第一个难点，即张量切片。我们实现了buffer，这让我们有了利用共享数据来实现切片的机会，在此之前，我们先来看看切片是什么
先从一维的开始，若有array[0, 1, 2, 3, 4]一维数组，那么我如果只想要其中的一部分，比如[1, 2, 3]，那么也就是array[1:4]，这就是切片
其中切片是从0开始的，范围是左闭右开，比如array[1:4]表示[1, 4)。
拓展到多维的张量切片也是一样的，我们只需要对每个维度分别进行切片操作，最后返回的tensor就是我们想要的。
既然维度数量不限，那我们很自然的可以想到继续使用可变参数模版，我们之前使用了std::array来转换参数包，但此处由于参数包中的参数类型不一
我们需要其他方式来使用参数包，比如最直接的方式是模版递归，缺点在于代码量较多，还有一个方式是借助variant，这个在此不展开说。
最终我们选择泛型lambda，这个方式比较便捷，同时是一个比较新颖的语法，不过理解起来有一定难度。
当然，我们也推荐你使用模版递归，这是更直观的方式，在注释中我也会实现模版递归的具体代码  
*/  
    template<typename... Args, std::enable_if_t<(is_slice_v<Args> || ...), int> = 0>
    Tensor operator()(Args... args) {
        constexpr size_t numOfArgs = sizeof...(args);
        if(numOfArgs != shape_.size()) throw poerror::DimensionException("Number of indices does not match tensor rank");
        Tensor sliceTensor(*this, AliasTag{});
        std::vector<int64_t> newShape{};
        std::vector<int64_t> newStrides{};
        size_t dim = 0;
        auto process = [&](const auto& arg) {
            using argType = std::decay_t<decltype(arg)>;
/*
为什么使用constexpr?
首先我们需要知道，constexpr关键字全称constant expression，意思是常量表达式。简单来说，使用constexpr声明的变量必须是编译期可
确定的，而此处的if constexpr意思则是编译期分支。
由于我们需要处理三种参数类型，int，Slice，All。如果我们实现为运行期分支判断，那么在编译阶段就会有问题。
比如我们会使用arg.start，当然我们知道这里的arg其实是Slice类型的对象，但由于没有使用constexpr，编译器并不知道这是什么类型
实例化就会出现问题，当使用constexpr时，如果编译时判断到这个arg时Slice对象，那么其他的分支就根本不会被编译
*/
            if constexpr (std::is_integral_v<argType>) {
                if(arg >= shape_[dim] || arg < 0) throw poerror::DimensionException("Illegal dimensions");
                sliceTensor.offset_ += arg * strides_[dim];
            } else if constexpr (std::is_same_v<argType, Slice>) {
                if(arg.start_ < 0 || arg.end_ < 0 || arg.end_ > shape_[dim] ||
                   arg.start_ > arg.end_) throw poerror::DimensionException("Illegal dimensions");

// 暂时不支持负步长
             if(arg.step_ <= 0) throw poerror::StepException("Step must be a positive number");
/*
这里需要考虑有step的情况，比如对于(1, 2, 3, 4, 5)，如果我令step=2，那么得到的则是(1, 3, 5)；如果是(1, 2, 3, 4)，令step=2，得到(1, 3)
显然在step为2的情况下，元素数量为奇数时需要加1再除以2，元素数量为偶数时直接除以2
那step为任意的情况下呢？对于一个元素数量为n的数列，如果step=t，从a0开始，中间间隔t-1个元素，然后就是at，a2t, a3t...akt，其中kt<=n-1
也就是最后一个元素不能超过数列的最后一项，因此k<=(n-1)/t，由于是从a0开始，所以最后取得的数量就是(n-1)/t + 1
*/
                int64_t span = arg.end_ - arg.start_;
                int64_t slice_length = span == 0 ? 0 : 1 + (span - 1) / arg.step_;
                newShape.push_back(slice_length);
                newStrides.push_back(strides_[dim] * arg.step_);
                sliceTensor.offset_ += arg.start_ * strides_[dim];
            } else if constexpr (std::is_same_v<argType, All>) {
                newShape.push_back(shape_[dim]);
                newStrides.push_back(strides_[dim]);
            } else {
                throw poerror::ArgsException("Illegal arguments");
            }
            ++dim;
        };

        (..., process(args));

        sliceTensor.shape_ = std::move(newShape);
        sliceTensor.strides_ = std::move(newStrides);

        return sliceTensor;
    }
};