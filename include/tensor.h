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
#include <variant>
#include "buffer.h"
#include "exceptions.h"
#include "slice.h"

template<typename T>
inline constexpr bool is_slice_v = std::is_same_v<std::decay_t<T>, Slice> || std::is_same_v<std::decay_t<T>, All>;

class Tensor {
private:
    struct AliasTag{};
    Tensor(const Tensor& other, AliasTag);

    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...;};
    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

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
    float& at(const std::vector<int64_t>& indices);
    const float& at(const std::vector<int64_t>& indices) const;
    Tensor slice(std::vector<std::variant<int64_t, Slice, All>> args) const;
    size_t offset() const;
    int64_t numel() const;

    Tensor operator+(const Tensor& other) const;
    Tensor& operator=(const Tensor& other);
    Tensor& operator=(Tensor&& other) noexcept;

    template<typename... Args, std::enable_if_t<(std::is_integral_v<Args> && ...), int> = 0>
    float& operator()(Args... args) {
        constexpr size_t numOfArgs = sizeof...(args);
        std::array<int64_t, numOfArgs> arr = {args...};
        // 判断参数包里的参数是否合法
        if(numOfArgs == 0 || numOfArgs != shape_.size()) throw poerror::DimensionException("Illegal dimensions");
        for(size_t i=0; i<numOfArgs; ++i) {
            if(arr[i] < 0) arr[i] += shape_[i];
            if(arr[i] < 0 || arr[i] >= shape_[i]) throw poerror::DimensionException("Illegal dimensions");
        }
        // 求实际索引值(需要加上offset)
        size_t idx = 0;
        for(size_t i=0; i<numOfArgs; ++i) {
            idx += arr[i] * strides_[i];
        }
        return buffer_->data_[offset_+idx];
    }

    // const重载版本
    template<typename... Args, std::enable_if_t<(std::is_integral_v<Args> && ...), int> = 0>
    const float& operator()(Args... args) const {
        constexpr size_t numOfArgs = sizeof...(args);
        std::array<int64_t, numOfArgs> arr = {args...};
        if(numOfArgs == 0 || numOfArgs != shape_.size()) throw poerror::DimensionException("Illegal dimensions");
        for(size_t i=0; i<numOfArgs; ++i) {
            if(arr[i] < 0) arr[i] += shape_[i];
            if(arr[i] < 0 || arr[i] >= shape_[i]) throw poerror::DimensionException("Illegal dimensions");
        }
        size_t idx = 0;
        for(size_t i=0; i<numOfArgs; ++i) {
            idx += arr[i] * strides_[i];
        }
        return buffer_->data_[offset_+idx];
    }
 
    template<typename... Args, std::enable_if_t<(is_slice_v<Args> || ...), int> = 0>
    Tensor operator()(Args... args) const {
        constexpr size_t numOfArgs = sizeof...(args);
        if(numOfArgs != shape_.size()) throw poerror::DimensionException("Number of indices does not match tensor rank");
        Tensor sliceTensor(*this, AliasTag{});
        std::vector<int64_t> newShape{};
        std::vector<int64_t> newStrides{};
        size_t dim = 0;
        auto process = [&](const auto& arg) {
            using argType = std::decay_t<decltype(arg)>;
            if constexpr (std::is_integral_v<argType>) {
                int64_t idx = arg;
                if(idx < 0) idx += shape_[dim];
                if(idx >= shape_[dim] || idx < 0) throw poerror::DimensionException("Illegal dimensions");
                sliceTensor.offset_ += idx * strides_[dim];
            } else if constexpr (std::is_same_v<argType, Slice>) {
                Slice slc{arg.start_, arg.end_, arg.step_};
                if(slc.start_ < 0) slc.start_ = shape_[dim] + slc.start_ >= 0 ? shape_[dim] + slc.start_ : 0;
                if(slc.end_   < 0) slc.end_   = shape_[dim] + slc.end_   >= 0 ? shape_[dim] + slc.end_   : 0;
                if(slc.start_ > shape_[dim]) slc.start_ = shape_[dim];
                if(slc.end_   > shape_[dim]) slc.end_   = shape_[dim];
                if(slc.start_ > slc.end_) slc.end_ = slc.start_;

// 暂时不支持负步长
                if(arg.step_ <= 0) throw poerror::StepException("Step must be a positive number");
                int64_t span = slc.end_ - slc.start_;
                int64_t slice_length = span == 0 ? 0 : 1 + (span - 1) / slc.step_;
                newShape.push_back(slice_length);
                newStrides.push_back(strides_[dim] * slc.step_);
                sliceTensor.offset_ += slc.start_ * strides_[dim];
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