#include "slice.h"
#include "tensor.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <vector>
#include "exceptions.h"

TEST(TensorAddition, BroadcastLeadingDim) {
    Tensor lhs(std::vector<int64_t>{2, 3});
    Tensor rhs(std::vector<int64_t>{1, 3});
    float data = 0;
    for(int64_t i=0; i<2; ++i) {
        for(int64_t j=0; j<3; ++j) {
            lhs(i, j) = data;
            data++;
        }
    }
    for(int64_t i=0; i<1; ++i) {
        for(int64_t j=0; j<3; ++j) {
            rhs(i, j) = data;
            data++;
        }
    }
    Tensor res = lhs + rhs;
    EXPECT_EQ(res.shape(), (std::vector<int64_t>{2, 3}));
    for(int64_t i=0; i<2; ++i) {
        for(int64_t j=0; j<3; ++j) {
            EXPECT_FLOAT_EQ(res(i, j), lhs(i, j) + rhs(0, j));
        }
    }
}

TEST(TensorAddition, BroadcastBothSides) {
    Tensor lhs(std::vector<int64_t>{4, 1});
    Tensor rhs(std::vector<int64_t>{1, 5});
    float data = 0;
    for(int64_t i=0; i<4; ++i) {
        lhs(i, 0) = data;
        data++;
    }
    for(int64_t j=0; j<5; ++j) {
        rhs(0, j) = data;
        data++;
    }
    Tensor res = lhs + rhs;
    EXPECT_EQ(res.shape(), (std::vector<int64_t>{4, 5}));
    for(int64_t i=0; i<4; ++i) {
        for(int64_t j=0; j<5; ++j) {
            EXPECT_FLOAT_EQ(res(i, j), lhs(i, 0)+rhs(0, j));
        }
    }
    
}

TEST(TensorAddition, BroadcastMissingDim) {
    Tensor lhs(std::vector<int64_t>{2, 3, 4});
    Tensor rhs(std::vector<int64_t>{3, 1});
    float data = 0;
    for(int64_t i=0; i<2; ++i) {
        for(int64_t j=0; j<3; ++j) {
            for(int64_t k=0; k<4; ++k) {
                lhs(i, j, k) = data;
                data++;
            }
        }
    }
    for(int64_t i=0; i<3; ++i) {
        rhs(i, 0) = data;
        data++;
    }
    Tensor res = lhs + rhs;
    EXPECT_EQ(res.shape(), (std::vector<int64_t>{2, 3, 4}));
    for(int64_t i=0; i<2; ++i) {
        for(int64_t j=0; j<3; ++j) {
            for(int64_t k=0; k<4; ++k) {
                EXPECT_FLOAT_EQ(res(i, j, k), lhs(i, j, k)+rhs(j, 0));
            }
        }
    }
}

TEST(TensorAddition, NonContiguousViewNoBroadcast) {
    Tensor tensor(std::vector<int64_t>{16, 16, 32});
    Tensor view = tensor(all, Slice{0, 15, 2}, all);
    Tensor continuous(std::vector<int64_t>{16, 1, 32});
    float data = 0;
    for(int64_t i=0; i<16; ++i) {
        for(int64_t j=0; j<8; ++j) {
            for(int64_t k=0; k<32; ++k) {
                view(i, j, k) = data;
                data++;
            }
        }
    }
    for(int64_t i=0; i<16; ++i) {
        for(int64_t k=0; k<32; ++k) {
            continuous(i, 0, k) = data;
            data++;
        }
    }
    Tensor res = view + continuous;
    EXPECT_EQ(res.shape(), (std::vector<int64_t>{16, 8, 32}));
    for(int64_t i=0; i<16; ++i) {
        for(int64_t j=0; j<8; ++j) {
            for(int64_t k=0; k<32; ++k) {
                EXPECT_FLOAT_EQ(res(i, j, k), view(i, j, k)+continuous(i, 0, k));
            }
        }
    }
}

TEST(TensorAddition, NonContiguousViewWithBroadcast) {
    Tensor view = Tensor({5, 3})(all, Slice{0, 1});
    Tensor continuous({1, 4});
    float data = 0;
    for(int64_t i=0; i<5; ++i) {
        view(i, 0) = data;
        data++;
    }
    for(int64_t j=0; j<4; ++j) {
        continuous(0, j) = data;
        data++;
    }
    Tensor res = view + continuous;
    EXPECT_EQ(res.shape(), (std::vector<int64_t>{5, 4}));
    for(int64_t i=0; i<5; ++i) {
        for(int64_t j=0; j<4; ++j) {
            EXPECT_FLOAT_EQ(res(i, j), view(i, 0)+continuous(0, j));
        }
    }
}

TEST(TensorAddition, EdgeCases) {
    {
        Tensor normal(std::vector<int64_t>{3, 4});
        Tensor empty{};
        EXPECT_THROW(normal+empty, poerror::DimensionException);
        EXPECT_THROW(empty+normal, poerror::DimensionException);
        EXPECT_THROW(empty+empty , poerror::DimensionException);
    }

    {
        Tensor lhs(std::vector<int64_t>{4, 5});
        Tensor rhs(std::vector<int64_t>{1, 4});
        EXPECT_THROW(lhs+rhs, poerror::DimensionException);
    }

    {
        Tensor lhs(std::vector<int64_t>{0, 5});
        Tensor rhs(std::vector<int64_t>{2, 5});
        EXPECT_THROW(lhs+rhs, poerror::DimensionException);
    }

    {
        Tensor lhs(std::vector<int64_t>{0, 5});
        Tensor rhs(std::vector<int64_t>{1, 5});
        EXPECT_NO_THROW(lhs+rhs);
        Tensor res = lhs+rhs;
        EXPECT_EQ(res.shape(), (std::vector<int64_t>{0, 5}));
        EXPECT_EQ(res.numel(), 0);
    }

    {
        Tensor zeroSlice = Tensor({2, 3})(Slice{1, 1}, all);
        Tensor rhs({3,});
        EXPECT_NO_THROW(zeroSlice+rhs);
        Tensor res = zeroSlice + rhs;
        EXPECT_EQ(res.shape(), (std::vector<int64_t>{0, 3}));
    }
}

TEST(CallOperator, ItemAccessOfNormal) {
    Tensor t1({3, 4, 2}, 1.5);
    int i = 2;
    long long j = 2;
    int k = 1;
    EXPECT_FLOAT_EQ(t1(i, j, k), 1.5);
    EXPECT_THROW(t1(2, 1), poerror::DimensionException);
    EXPECT_THROW(t1(-1, 1, 1), poerror::DimensionException);
    EXPECT_THROW(t1(1, 5, 1), poerror::DimensionException);
}

TEST(CallOperator, ItemAccessOfSlice) {
    Tensor t2 = Tensor({9, 5, 8}, 9.1)(Slice{3, 9}, 2, Slice{0, 8, 2});
    EXPECT_FLOAT_EQ(t2(3, 3), 9.1);
    EXPECT_THROW(t2(2, 3, 1), poerror::DimensionException);
    EXPECT_THROW(t2(-2, 2), poerror::DimensionException);
    EXPECT_THROW(t2(2, 9), poerror::DimensionException);
}

TEST(CallOperator, Silce) {
    Tensor rawTensor({16, 16, 32});
    EXPECT_THROW(rawTensor(Slice{0, 12}), poerror::DimensionException);
    EXPECT_THROW(rawTensor(Slice{0, 20}, all, all), poerror::DimensionException);
    EXPECT_THROW(rawTensor(Slice{0, 12, -1}, all, all), poerror::StepException);
}

TEST(CopyFunc, Constructor) {
    Tensor empty{};
    Tensor zeroShape({2, 0, 3});
    Tensor t1(empty);
    Tensor t2(zeroShape);
    EXPECT_EQ(t1.offset(), 0);
    EXPECT_EQ(t2.offset(), 0);
}

TEST(CopyFunc, Assignment) {
    Tensor t1({2, 3}, 2.3);
    EXPECT_NO_THROW(t1 = t1);
}


