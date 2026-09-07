//
// tensor.h
// GraceInfra
//
// Created by DaR1ng on 26-7-10

/*
本文件为tensor.h的头文件，承接matrix类。
从matrix到tensor，其中最重要的区别就是从二维到任意维度张量，这对后续的卷积神经网络以及transformer架构是关键。
在开始之前，请思考:1. 如何让tensor支持任意维度？ 2. 任意维度的tensor相较于二维的matrix，其属性和功能有什么区别？
对于第一个问题，主要有两种思路，第一个最简单，就是嵌套vector，比如vector<vector<vector<float>>> vec1。
但这个方法可以说完全不能用，因为内存不连续，cache miss率非常高。
第二种，即利用strides实现任意维度的需求，其实这里就是当时matrix中二维索引映射一维的思想，拓展到任意维度，就是任意维度的
索引映射到一维，可见，无论是多少维度的张量，其底层都是一个顺序存储的数组。
比如一个tensor(2, 3, 4)，代表一个2行，3列，4管（深度）的张量（顺便说一下，在tensor中一般以第0维，第1维，第2维，以此类推
来表达不同维度，和矩阵中我们叫第0维是行，第1维是列有所不同），你可以思考一下，如何仿照matrix中的映射方式来将这个三维索引映射
到一维，同时也想一下，tensor类应该具有什么属性。
*/

#pragma once
#include <vector>
// int64_t的头文件，int64_t是64位int类型，32位int在深度学习中有概率溢出
#include <cstdint>
#include <cstdlib>
/*
对于顺序存储数组+strides实现tensor，其具体的实现方法也有不少种类，这里就拿我比较熟悉的两类方式谈论一下。
第一种是无论在学习还是工程上都是一个比较推荐和简单的方案，即使用vector来储存数据，strides来实现任意维度，
这其实和我们之前的matrix是差不多的，只是多了strides。
第二种是C风格的float*搭配strides，我选择了这个，为什么:
1. 数组的本质就是Type*，比如float*相当于指向数组首地址的指针，通过指针偏移的方式在使用和修改数组。
2. vector帮我们实现了RAII，为一些特殊情况兜底，但这对学习是不利的，使用float*，就会迫使你思考如何管理内存，如何处理异常，理解RAII的本质。
3. 但对于strides等其他属性，我选择了vector，该偷懒的地方就偷懒。

不过float*数组的确难以理解，我将在tensor.cpp中详细讲解。
*/

class Tensor {
private:
// 同样，按照之前的思维逻辑思考一下tensor需要哪些属性。
// 首先float* data_是必要的，这是核心属性
// 然后std::vector<int64_t> strides_这是实现任意维度的关键，后续我会说明它为什么是一个数组
// 其次是std::vector<int64_t> shape_这个比较好理解，它代表了每个维度的数值，比如Tensor t1(2, 3, 4)的shape_即为[2, 3, 4]。
// 最后是int64_t numel_这个可能算是最不重要的那一个，它代表了当这个张量压缩到一维时，其数据总数量
// 当然，这个numel_可以不要，实现一个numel()函数即可
    std::vector<int64_t> shape_; 
    std::vector<int64_t> strides_;
    float* data_;
public:
// 这里仿照了matrix类定义的格式，但将实际的行和列换成了shape数组
// 同时由于我们选择了float*裸指针来描述tensor的数据，构造函数中会用到malloc来分配堆内存，因而需要完善析构函数的内容，
// 使其可以free对象在构造时申请的堆内存，并将指针悬空
    Tensor();
    explicit Tensor(const std::vector<int64_t> &shape);
    Tensor(const std::vector<int64_t> &shape, float fill_data);
    Tensor(const Tensor& other);
    Tensor(Tensor&& other) noexcept;
    ~Tensor();

    int64_t shape(int64_t dim) const;
    const std::vector<int64_t>& shape() const;
    const std::vector<int64_t>& strides() const;
    int64_t numel() const;
    float* data();
    const float* data() const;
    static bool safe_multiply(int64_t a, int64_t b, int64_t& result);

    Tensor operator+(const Tensor& other) const;
    Tensor& operator=(const Tensor& other);
    Tensor& operator=(Tensor&& other) noexcept;
    float& operator()(int64_t idx0, int64_t idx1);
    const float& operator()(int64_t idx0, int64_t idx1) const;
};