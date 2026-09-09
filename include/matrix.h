// 
// matrix.h
// GraceInfra
//
// Created by DaR1ng on 26-7-9

/*
本文件为matrix头文件，其定义了matrix数据类型及其主要的方法（函数）。
在正式开始之前，希望你可以思考以下的这些问题:
1. matrix是什么？
2. matrix在深度学习中有什么作用？
3. 在深度学习中，matrix应该具备哪些功能？
接下来，带着这些问题学习后续的内容
*/

#pragma once
#include <cstddef>
#include <vector>
#include <cstdlib>

class Matrix {
private:
// 二维矩阵是简单且易懂的一种matrix实现方式，但对于大于二维的张量，表示会略显复杂，比如在卷积神经网络中，彩色图片有这些参数：数量Batch，
// 通道channels，图片尺寸height和width。因此对于彩色图片，二维矩阵的表达形式为(Batch*hight*width, channels)(通道优先)
// 但无论实现方式如何，本质是相同，均为内存上一段连续的一维数组。
// 此处先实现二维矩阵，以便读者能更好的理解和学习

    int rows_;
    int cols_;
    std::vector<float> data_;
public:
// 思考：在构造matrix的时候，你可能需要哪些构造方式？这决定了你如何定义构造函数。

    Matrix();
    Matrix(int rows, int cols);
    Matrix(int rows, int cols, float fill_data);
    Matrix(const Matrix& other);

// 此处是读取/修改matrix的方法，同样思考，你可能会如何使用一个matrix，你希望它具备什么功能？
// 如果没有头绪，可以参照此处的函数，但强烈建议你尽可能自己思考：不是函数催生功能，而是功能催生函数

    int rows() const;
    int cols() const;
    size_t numel() const;
    float* data();
    const float* data() const;

// 此处是重载运算符，是支撑matrix逻辑运算的重要组成部分

    Matrix operator+(const Matrix &other) const;
    Matrix& operator=(const Matrix &other);
    float& operator()(int rows_idx, int cols_idx);
    const float& operator()(int rows_idx, int cols_idx) const;
};