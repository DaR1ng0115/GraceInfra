// 
// matrix.cpp
// GraceInfra
//
// Created by DaR1ng on 26-7-9

/*
这里是matrix的源文件，定义了matrix具备的功能和方法
在本章中，我会尽可能显式写出“this”，以便于理解，后续我会省略
*/

#include "../include/matrix.h"
#include <cassert>

// 在命名上，我们可以让数据结构自带的私有成员变量属性带有下标，即variable_
// 这样做可以让自己包括其他人更好地理解哪些变量是本对象的变量

Matrix::Matrix() 
:rows_(1), cols_(1) {
    this->data_ = std::vector<float>(1);
}

// 这里也可以使用更简单一点的初始化方法，直接在初始化列表中data_(rows*cols)

Matrix::Matrix(int rows, int cols)
:rows_(rows), cols_(cols) {
    this->data_ = std::vector<float>(this->rows_*this->cols_);
}

Matrix::Matrix(int rows, int cols, float fill_data)
:rows_(rows), cols_(cols) {
    this->data_ = std::vector<float>(this->rows_*this->cols_, fill_data);
}

Matrix::Matrix(const Matrix& other) 
:rows_(other.rows()), cols_(other.cols()) {
    this -> data_ = std::vector<float>(this->rows_*this->cols_);
    std::copy(other.data_.begin(), other.data_.end(), data_.data());
}

// 下面实现了一些接口函数，请读者进行这样的思考:成员变量中有哪些需要对外提供接口？对外暴露的接口在逻辑上应该是只读还是可修改的？
// 如果对const的使用上有一些困惑，建议阅读《Effective C++》条款03:尽可能使用const

int Matrix::rows() const {
    return this->rows_;
}

int Matrix::cols() const {
    return this->cols_;
}

// 此处我特意选择了length作为方法的名称，因为其专指matrix的“长度”，即无论是几维的matrix，压平成一维数组后的长度

int Matrix::numel() const {
    return this->data_.size();
}

// 在定义和实现一个函数之前，建议做这些思考:1. 是什么功能促使我需要实现一个或多个函数来完成功能？ 2. 遵循"奥卡姆剃刀"，这个
// 功能需要的函数是可复用还是一次性的？如果是后者，尽量避免写多余的函数。 3. 如果决定了需要实现某种函数，请使用"第一性原理"将函数
// 拆解为不可再分或较基础的模块，查阅资料/询问AI，学习和理解朴素以及高效的实现方法。以上是逻辑层面的思考，随后进行语法层面的思考。
// 4. 先思考这个函数应该叫什么，好的命名不仅可读性更好，尤其在深度学习框架这种中大型且跨越极大的项目中，好的命名是消除歧义的根本。
// 5. 思考这个函数的职责:接受什么数据，进行什么处理，输出什么结果。接受什么数据:决定了你的函数的参数怎么设计，重点注意参数的数据类型，
// 是否是指针，是否使用引用，是否使用const等。进行什么处理:决定了你的算法设计逻辑，边界条件，异常处理等。输出什么条件:决定了你的函数
// 的返回类型，并重点思考使用的时候是否符合日常习惯，符合正常的逻辑。

float* Matrix::data() {
    return this->data_.data();
}

const float* Matrix::data() const {
    return this->data_.data();
}

// 此处的加号重载，有一个性能上的问题，即返回值return res;会导致拷贝。
// 现在可以思考一下，浅拷贝(引用)会导致悬挂指针，因为一旦出了函数定义域时，res就会被销毁，那么引用指向的就是一个被销毁的对象，即悬挂指针。
// 深拷贝，即现在的实现方式，如果数据量过大，会限制于内存带宽。
// 那么有没有什么办法解决上面的那些问题呢？
// 解决方法就是"移动语义"，通俗的说，移动语义就是把指针"偷"过来。
// 比如两个vector，vec1[地址:0x1000:1KB数据]，vec2[地址:0x2000:无数据]
// 那么使用移动语义，相当于直接将0x1000地址的所有权给vec2，原来的vec1悬空
// 即变成vec2[地址:0x1000:1KB数据]，vec1 = nullptr
// 不过移动语义不是完美的，它的问题在于，数据从始至终只有一份，原指针被悬空了
// 但在此处是完美适配的，因为res离开函数作用域就会被销毁，也就是说我们本来就只需要一份数据
// 因此，未来使用移动语义的时候，先思考逻辑上到底是只需要一份数据，还是需要实际拷贝出第二份数据
// 移动语义和移动构造的实现，详见tensor.cpp

Matrix Matrix::operator+(const Matrix &other) const {
    assert(this->rows() == other.rows() && this->cols() == other.cols());
    Matrix res(this->rows(), this->cols());
    for(int i=0; i<this->numel(); ++i) {
        res.data()[i] = this->data_[i] + other.data()[i];
    }
    return res;
}

Matrix& Matrix::operator=(const Matrix &other) {
    if(this == &other) return *this;
    Matrix temp(other);
    this->rows_ = other.rows();
    this->cols_ = other.cols();
    this->data_ = std::move(temp.data_);
    return *this;
}

// 请重点关注这里对括号的重载，它是决定访问逻辑的关键一环。
// 此处将二维索引映射成了一维索引，并且为了便于理解，我将传入参数命名为了rows_idx和cols_idx。
// 请思考一下，什么是属性，什么是变量。在面向对象编程中，属性是一个对象自身的属性，它描述了该对象的特征和性质。
// 比如对于一个matrix对象，它拥有三个属性，rows(行数),cols(列数),data(数据)。
// 在实际访问一个matrix对象的时候，我们会输入我们想要访问的索引，比如(2, 3)代表第2行第3列的数据是什么
// 在重载运算符中，我们将这个索引抽象成了参数，这也就是rows_idx和cols_idx这两个名字的由来
// 要理解行优先矩阵是如何计算一维索引值的，那我们需要先看看行优先矩阵如何存储在vecotr中的
// 比如一个2x3的矩阵(为了便于区分，我将第一行用中括号包含，第二行用大括号包含):
// [0, 1, 2]
// {3, 4, 5}
// 行优先，也就是说在遍历矩阵的时候，按矩阵的行顺序依次访问所有元素，即先遍历第一行，再遍历第二行，以此类推。
// 在这里，访问顺序就是数字的排列顺序，即 0, 1, 2, 3, 4, 5。并且，在内存的存储中，其顺序也是这个顺序
// 相当于:[0, 1, 2]{3, 4, 5} -> [0, 1, 2, 3, 4, 5]
// 那么索引是怎么计算的呢？ 先思考这么一个情况，如果我想要访问某个矩阵的第m行，第n列的数据，那么我们可以把
// 这个数据的索引拆解成两个部分。首先我们需要跨过前面m行的所有数据，然后跨过第m行，前面n个数据，就到了我们
// 想访问的数据，翻译成数学语言，也就是:rows_idx*cols_ + cols_idx

float& Matrix::operator()(int rows_idx, int cols_idx) {
    assert(rows_idx>=0 && cols_idx>=0 && rows_idx<this->rows_ && cols_idx<this->cols_);
    return this->data_[rows_idx*this->cols_+cols_idx];
}

const float& Matrix::operator()(int rows_idx, int cols_idx) const {
    assert(rows_idx>=0 && cols_idx>=0 && rows_idx<this->rows_ && cols_idx<this->cols_);
    return this->data_[rows_idx*this->cols_+cols_idx];
}

/*
至此，我们实现了一个基础matrix类。
回顾一下，我们从定义一个matrix对象应该拥有什么属性，随后定义了接口函数，到最后重载运算符，使其使用逻辑更自然高效。
涉及的C++知识:
- 初始化列表
- const的使用
- 指针
- 重载运算符
- 异常处理

下一步，我们将跳出二维矩阵，从tensor的视角，直视内存空间和排列，同时，我们将思考实际的工程问题和突发情况。
注意，下一节中的内容，对你于指针的理解提出了较高的要求，如果有些陌生，建议复习一下指针。
如果你准备好了，请移步tensor.h
*/