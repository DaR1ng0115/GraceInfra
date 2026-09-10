#include <limits>
#include <cstdint>
#include <vector>
#include <utility>
#include "gtest/gtest.h"
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
    EXPECT_THROW(defaultTensor(0, 0), poerror::GraceException);
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

{
// 数值溢出检测
    std::vector<int64_t> overFlowShape{std::numeric_limits<int64_t>::max(), 2};
    EXPECT_THROW(Tensor overFlowShapeTensor(overFlowShape), poerror::OverflowException);
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

    for(int64_t i=0; i<doubleArgTensor.numel(); ++i) {
        EXPECT_EQ(doubleArgTensor.data()[i], 5);
    }
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

{
// 数值溢出检测
    std::vector<int64_t> overFlowShape{std::numeric_limits<int64_t>::max(), 2};
    EXPECT_THROW(Tensor overFlowShapeTensor(overFlowShape, 9.5), poerror::OverflowException);
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

    for(int64_t i=0; i<copyByNormalTensor.numel(); ++i) {
        EXPECT_EQ(copyByNormalTensor.data()[i], 24);
        EXPECT_EQ(normalTensor.data()[i], 24);
    }
// 测试深拷贝
    normalTensor.data()[3] = 8;
    EXPECT_EQ(copyByNormalTensor.data()[3], 8);
}

{
// 拷贝构造空对象
    std::vector<int64_t> empty;
    Tensor emptyTensor(empty);
    Tensor copyByEmptyTensor(emptyTensor);
    EXPECT_EQ(copyByEmptyTensor.data(), nullptr);
    EXPECT_EQ(copyByEmptyTensor.numel(), 0);
    EXPECT_EQ(copyByEmptyTensor.shape(), empty);
    EXPECT_EQ(copyByEmptyTensor.strides(), empty);
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

    for(int64_t i=0; i<moveByNormalTensor.numel(); ++i) {
        EXPECT_EQ(moveByNormalTensor.data()[i], 24);
    }
    
    std::vector<int64_t> empty;
    EXPECT_EQ(normalTensor.data(), nullptr);
    EXPECT_EQ(normalTensor.numel(), 0);
    EXPECT_EQ(normalTensor.shape(), empty);
    EXPECT_EQ(normalTensor.strides(), empty);
}

{
// 移动构造空对象
    std::vector<int64_t> empty;
    Tensor emptyTensor(empty);
    Tensor moveByEmptyTensor(std::move(emptyTensor));
    
    EXPECT_EQ(moveByEmptyTensor.data(), nullptr);
    EXPECT_EQ(moveByEmptyTensor.numel(), 0);
    EXPECT_EQ(moveByEmptyTensor.shape(), empty);
    EXPECT_EQ(moveByEmptyTensor.strides(), empty);

    EXPECT_EQ(emptyTensor.data(), nullptr);
    EXPECT_EQ(emptyTensor.numel(), 0);
    EXPECT_EQ(emptyTensor.shape(), empty);
    EXPECT_EQ(emptyTensor.strides(), empty);
}

{
// 移动构造含0shape对象
    std::vector<int64_t> containZeroShape{3, 0 ,2};
    Tensor containZeroShapeTensor(containZeroShape);
    Tensor moveByContainZeroShapeTensor(std::move(containZeroShapeTensor));

    EXPECT_EQ(moveByContainZeroShapeTensor.data(), nullptr);
    EXPECT_EQ(moveByContainZeroShapeTensor.numel(), 0);
    EXPECT_EQ(moveByContainZeroShapeTensor.shape(), containZeroShape);
    EXPECT_EQ(moveByContainZeroShapeTensor.strides(), (std::vector<int64_t>{0*2, 1*2, 1}));

    EXPECT_EQ(containZeroShapeTensor.data(), nullptr);
    EXPECT_EQ(containZeroShapeTensor.numel(), 0);
    EXPECT_EQ(containZeroShapeTensor.shape(), (std::vector<int64_t>{}));
    EXPECT_EQ(containZeroShapeTensor.strides(), (std::vector<int64_t>{}));
}

}

// ===================== ShapeMethod =====================

TEST(TensorMethod, ShapeMethod) {

{
// 测试: int64_t Tensor::shape(int64_t dim) const
    Tensor testNormalShapeMethodTensor({4, 2, 19, 3});
    EXPECT_EQ(testNormalShapeMethodTensor.shape(2), 19);
    EXPECT_THROW(testNormalShapeMethodTensor.shape(100), poerror::DimensionException);
    EXPECT_THROW(testNormalShapeMethodTensor.shape(-3), poerror::DimensionException);
    Tensor testEmptyShapeMethodTensor{};
    EXPECT_THROW(testEmptyShapeMethodTensor.shape(0), poerror::DimensionException);
    Tensor testZeroShapeMethodTensor({32, 0, 3});
    EXPECT_EQ(testZeroShapeMethodTensor.shape(1), 0);
}

}

// ===================== DataMethod =====================

TEST(TensorMethod, DataMethod) {

{
// 测试: float* Tensor::data()
    Tensor testDataMethodTensor({3, 1});
    testDataMethodTensor.data()[2] = 5;
    EXPECT_EQ(testDataMethodTensor(2, 0), 5);
}

}

// ===================== Operator =====================

TEST(Operator, PlusOperator) {

{
    Tensor testPlusOperatorTensor_1({4, 2, 9, 2}, 3.50);
    Tensor testPlusOperatorTensor_2({4, 2, 9, 2}, 1.92);
    Tensor testPlusOperatorTensor_3({4, 2, 9, 2}, 10.412);
    Tensor res_1 = testPlusOperatorTensor_1 + testPlusOperatorTensor_2;
// 对于浮点数，单纯使用EXPECT_EQ会有精度损失，导致测试失败，故使用EXPECT_FLOAT_EQ
    for(int64_t i=0; i<res_1.numel(); ++i) {
        EXPECT_FLOAT_EQ(res_1.data()[i], 3.50+1.92);
    }
    Tensor res_2 = testPlusOperatorTensor_1 + testPlusOperatorTensor_2 + testPlusOperatorTensor_3;
    for(int64_t i=0; i<res_2.numel(); ++i) {
        EXPECT_FLOAT_EQ(res_2.data()[i], 3.50+1.92+10.412);
    }
}

}

// ===================== CopyEqualOperator =====================

TEST(Operator, CopyEqualOperator) {

{
    Tensor normalTensor({2, 3}, 2.5);
    Tensor emptyTensor{};
    Tensor containZeroTensor({2, 0, 5});

    Tensor copyEqualTensor_normal({3, 2}, 1.2);
    Tensor copyEqualTensor_empty({2, 3, 0});
    Tensor copyEqualTensor_zero({1, 3, 2, 5});

    copyEqualTensor_normal = normalTensor;
    copyEqualTensor_empty = emptyTensor;
    copyEqualTensor_zero = containZeroTensor;

    EXPECT_NE(copyEqualTensor_normal.data(), nullptr);
    EXPECT_EQ(copyEqualTensor_normal.numel(), 2*3);
    EXPECT_EQ(copyEqualTensor_normal.shape(), (std::vector<int64_t>{2, 3}));
    EXPECT_EQ(copyEqualTensor_normal.strides(), (std::vector<int64_t>{3, 1}));
    for(int64_t i=0; i<copyEqualTensor_normal.numel(); ++i) {
        EXPECT_EQ(copyEqualTensor_normal.data()[i], 2.5);
    }

    EXPECT_EQ(copyEqualTensor_empty.data(), nullptr);
    EXPECT_EQ(copyEqualTensor_empty.numel(), 0);
    EXPECT_EQ(copyEqualTensor_empty.shape(), (std::vector<int64_t>{}));
    EXPECT_EQ(copyEqualTensor_empty.strides(), (std::vector<int64_t>{}));

    EXPECT_EQ(copyEqualTensor_zero.data(), nullptr);
    EXPECT_EQ(copyEqualTensor_zero.numel(), 0);
    EXPECT_EQ(copyEqualTensor_zero.shape(), (std::vector<int64_t>{2, 0, 5}));
    EXPECT_EQ(copyEqualTensor_zero.strides(), (std::vector<int64_t>{0*5, 1*5, 1}));
}

}

//  ===================== MoveEqualOperator =====================

TEST(Operator, MoveEqualOperator) {

{
    Tensor normalTensor({3, 5, 2}, 9.2);
    Tensor emptyTensor{};
    Tensor containZeroTensor({3, 0, 4});

    Tensor moveEqualTensor_normal({2,3}, 2.9);
    Tensor moveEqualTensor_empty({4, 2, 12}, 4.20);
    Tensor moveEqualTensor_zero({2}, 2.3);

    moveEqualTensor_normal = std::move(normalTensor);
    moveEqualTensor_empty = std::move(emptyTensor);
    moveEqualTensor_zero = std::move(containZeroTensor);

    EXPECT_NE(moveEqualTensor_normal.data(), nullptr);
    EXPECT_EQ(moveEqualTensor_normal.numel(), 3*5*2);
    EXPECT_EQ(moveEqualTensor_normal.shape(), (std::vector<int64_t>{3, 5, 2}));
    EXPECT_EQ(moveEqualTensor_normal.strides(), (std::vector<int64_t>{5*2, 1*2, 1}));

    EXPECT_EQ(moveEqualTensor_empty.data(), nullptr);
    EXPECT_EQ(moveEqualTensor_empty.numel(), 0);
    EXPECT_EQ(moveEqualTensor_empty.shape(), (std::vector<int64_t>{}));
    EXPECT_EQ(moveEqualTensor_empty.strides(), (std::vector<int64_t>{}));

    EXPECT_EQ(moveEqualTensor_zero.data(), nullptr);
    EXPECT_EQ(moveEqualTensor_zero.numel(), 0);
    EXPECT_EQ(moveEqualTensor_zero.shape(), (std::vector<int64_t>{3, 0, 4}));
    EXPECT_EQ(moveEqualTensor_zero.strides(), (std::vector<int64_t>{0*4, 1*4, 1}));
}

}



