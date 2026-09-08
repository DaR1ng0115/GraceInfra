#include "gtest/gtest.h"
#include <cstdint>
#include <vector>
#include "tensor.h"
#include "exceptions.h"

// ===================== DefaultConstructor =====================

TEST(ConstructorFunction, DefaultConstructor) {

{
// 默认构造函数
    Tensor defaultTensor;
    EXPECT_EQ(defaultTensor.data(), nullptr);
    EXPECT_EQ(defaultTensor.numel(), 0);
    EXPECT_EQ(defaultTensor.shape(), std::vector<int64_t>());
    EXPECT_EQ(defaultTensor.strides(), std::vector<int64_t>());
    EXPECT_THROW(defaultTensor(0, 0), poerror::AppException);
}

}

// ===================== SingleArgConstructor =====================

TEST(ConstructorFunction, SingleArgConstructor) {

{
// 单参数构造函数
    Tensor singleArgTensor({3, 4, 5});
    EXPECT_NE(singleArgTensor.data(), nullptr);
    EXPECT_EQ(singleArgTensor.numel(), 60);
// EXPECT_EQ(SingleArgTensor.shape(), std::vector<int64_t>{3, 4, 5})是不可以的，因为EXPECT_EQ是宏，花括号内部的逗号会被
// 解析为宏参数分隔符
    EXPECT_EQ(singleArgTensor.shape(), (std::vector<int64_t>{3, 4, 5}));
    EXPECT_EQ(singleArgTensor.strides(), (std::vector<int64_t>{4*5, 1*5, 1}));
}

{
// 空shape单参数构造函数
    std::vector<int64_t> empty;
    Tensor singleEmptyShapeTensor(empty);
    EXPECT_EQ(singleEmptyShapeTensor.data(), nullptr);
    EXPECT_EQ(singleEmptyShapeTensor.numel(), 0);
    EXPECT_EQ(singleEmptyShapeTensor.shape(), empty);
    EXPECT_EQ(singleEmptyShapeTensor.strides(), empty);
}

{
// 含0shape单参数构造函数
    std::vector<int64_t> containZeroShape{3, 0, 2};
    Tensor singleContainZeroShapeTensor(containZeroShape);
    EXPECT_EQ(singleContainZeroShapeTensor.data(), nullptr);
    EXPECT_EQ(singleContainZeroShapeTensor.numel(), 0);
    EXPECT_EQ(singleContainZeroShapeTensor.shape(), containZeroShape);
    EXPECT_EQ(singleContainZeroShapeTensor.strides(), (std::vector<int64_t>{0*2, 1*2, 1}));
}

{
// 含负数shape单参数构造函数
    std::vector<int64_t> negativeShape{3, -2, 1};
    EXPECT_THROW(Tensor singleNegativeShapeTensor(negativeShape), poerror::DimensionException);
}

}

// ===================== DoubleArgConstructor =====================

TEST(ConstructorFunction, DoubleArgConstructor) {

{
// 双参数构造函数
    Tensor doubleArgTensor({2, 5, 8}, 5);
    EXPECT_NE(doubleArgTensor.data(), nullptr);
    EXPECT_EQ(doubleArgTensor.numel(), 80);
    EXPECT_EQ(doubleArgTensor.shape(), (std::vector<int64_t>{2, 5, 8}));
    EXPECT_EQ(doubleArgTensor.strides(), (std::vector<int64_t>{5*8, 1*8, 1}));
}

{
// 空shape双参数构造函数
    std::vector<int64_t> empty;
    Tensor doubleEmptyShapeTensor(empty, 10);
    EXPECT_EQ(doubleEmptyShapeTensor.data(), nullptr);
    EXPECT_EQ(doubleEmptyShapeTensor.numel(), 0);
    EXPECT_EQ(doubleEmptyShapeTensor.shape(), empty);
    EXPECT_EQ(doubleEmptyShapeTensor.strides(), empty);   
}

{
// 含0shape双参数构造函数
    std::vector<int64_t> containZeroShape{3, 0, 2};
    Tensor doubleContainZeroShapeTensor(containZeroShape, 10);
    EXPECT_EQ(doubleContainZeroShapeTensor.data(), nullptr);
    EXPECT_EQ(doubleContainZeroShapeTensor.numel(), 0);
    EXPECT_EQ(doubleContainZeroShapeTensor.shape(), containZeroShape);
    EXPECT_EQ(doubleContainZeroShapeTensor.strides(), (std::vector<int64_t>{0*2, 1*2, 1}));    
}

{
// 含负数shape双参数构造函数
    std::vector<int64_t> negativeShape{3, -2 ,1};
    EXPECT_THROW(Tensor doubleNegativeShapeTensor(negativeShape, 10), poerror::DimensionException);
}

}

// ===================== CopyConstructor =====================

TEST(ConstructorFunction, CopyConstrutor) {

{
// 拷贝构造正常对象
    Tensor normalTensor({4, 7, 9}, 24);
    Tensor copyByNormalTensor(normalTensor);
    EXPECT_NE(copyByNormalTensor.data(), normalTensor.data());
    EXPECT_NE(copyByNormalTensor.data(), nullptr);
    EXPECT_EQ(copyByNormalTensor.numel(), 4*7*9);
    EXPECT_EQ(copyByNormalTensor.shape(), (std::vector<int64_t>{4, 7, 9}));
    EXPECT_EQ(copyByNormalTensor.strides(), (std::vector<int64_t>{7*9, 1*9, 1}));
}

{
// 拷贝构造空对象
    std::vector<int64_t> emptyShape;
    Tensor emptyTensor(emptyShape);
    Tensor copyByEmptyTensor(emptyTensor);
    EXPECT_EQ(copyByEmptyTensor.data(), nullptr);
    EXPECT_EQ(copyByEmptyTensor.numel(), 0);
    EXPECT_EQ(copyByEmptyTensor.shape(), emptyShape);
    EXPECT_EQ(copyByEmptyTensor.strides(), emptyShape);
}

{
// 拷贝构造含0shape对象
    std::vector<int64_t> containZeroShape{3, 0, 2};
    Tensor containZeroShapeTensor(containZeroShape);
    Tensor copyByContainZeroShapeTensor(containZeroShapeTensor);
    EXPECT_EQ(copyByContainZeroShapeTensor.data(), nullptr);
    EXPECT_EQ(copyByContainZeroShapeTensor.numel(), 0);
    EXPECT_EQ(copyByContainZeroShapeTensor.shape(), containZeroShape);
    EXPECT_EQ(copyByContainZeroShapeTensor.strides(), (std::vector<int64_t>{0*2, 1*2, 1}));
}

}

// ===================== MoveConstructor =====================

TEST(ConstructorFunction, MoveConstructor) {

{
// 移动构造正常对象
    Tensor normalTensor({4, 7, 9}, 24);
    float* originData = normalTensor.data();
    int64_t originNumel = normalTensor.numel();
    std::vector<int64_t> originShape = normalTensor.shape();
    std::vector<int64_t> originStrides = normalTensor.strides();
    Tensor moveByNormalTensor(std::move(normalTensor));

    EXPECT_EQ(moveByNormalTensor.data(), originData);
    EXPECT_EQ(moveByNormalTensor.numel(), originNumel);
    EXPECT_EQ(moveByNormalTensor.shape(), originShape);
    EXPECT_EQ(moveByNormalTensor.strides(), originStrides);
    
    std::vector<int64_t> empty;
    EXPECT_EQ(normalTensor.data(), nullptr);
    EXPECT_EQ(normalTensor.numel(), 0);
    EXPECT_EQ(normalTensor.shape(), empty);
    EXPECT_EQ(normalTensor.strides(), empty);
}

}