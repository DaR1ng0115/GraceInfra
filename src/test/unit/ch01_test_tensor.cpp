#include "gtest/gtest.h"
#include <cstdint>
#include <gtest/gtest.h>
#include <vector>
#include "tensor.h"
#include "exceptions.h"

TEST(ConstructorFunction, DefaultConstrutor) {
// 默认构造函数
    Tensor DefaultTensor;
    EXPECT_EQ(DefaultTensor.data(), nullptr);
    EXPECT_EQ(DefaultTensor.numel(), 0);
    EXPECT_EQ(DefaultTensor.shape(), std::vector<int64_t>());
    EXPECT_EQ(DefaultTensor.strides(), std::vector<int64_t>());
    EXPECT_THROW(DefaultTensor(0, 0), poerror::AppException);

// 单参数构造函数
    Tensor SingleArgTensor({3, 4, 5});
    EXPECT_NE(SingleArgTensor.data(), nullptr);
    EXPECT_EQ(SingleArgTensor.numel(), 60);
// EXPECT_EQ(SingleArgTensor.shape(), std::vector<int64_t>{3, 4, 5})是不可以的，因为EXPECT_EQ是宏，花括号内部的逗号会被
// 解析为宏参数分隔符
    EXPECT_EQ(SingleArgTensor.shape(), (std::vector<int64_t>{3, 4, 5}));
    EXPECT_EQ(SingleArgTensor.strides(), (std::vector<int64_t>{20, 5, 1}));

// 异常单参数构造函数
    std::vector<int64_t> Empty;
    Tensor EmptyShapeTensor(Empty);
    EXPECT_EQ(EmptyShapeTensor.data(), nullptr);
    EXPECT_EQ(EmptyShapeTensor.numel(), 0);
    EXPECT_EQ(EmptyShapeTensor.shape(), Empty);
    EXPECT_EQ(EmptyShapeTensor.strides(), Empty);

// 双参数构造函数
    Tensor DoubleArgTensor({2, 5, 8}, 5);
    EXPECT_NE(DoubleArgTensor.data(), nullptr);
    EXPECT_EQ(DoubleArgTensor.numel(), 80);
    EXPECT_EQ(DoubleArgTensor.shape(), (std::vector<int64_t>{2, 5, 8}));
    EXPECT_EQ(DoubleArgTensor.strides(), (std::vector<int64_t>{40, 8, 1}));
}