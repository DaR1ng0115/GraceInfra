#include <cstdint>
#include <gtest/gtest.h>
#include "tensor.h"
#include <vector>

TEST(SliceFunc, NegativeSlice) {
    Tensor t1({16, 32}, 1.5);
    EXPECT_EQ(t1(Slice{3, -1}, all).shape(), (std::vector<int64_t>{12, 32}));
    EXPECT_EQ(t1(Slice{5, 100}, all).shape(), (std::vector<int64_t>{11, 32}));
    EXPECT_EQ(t1(Slice{0, -20}, all).shape(), (std::vector<int64_t>{0, 32}));
    EXPECT_EQ(t1(Slice{-3, 16}, all).shape(), (std::vector<int64_t>{3, 32}));
}

TEST(IndicesFunc, NegativeIndices) {
    Tensor t1({12, 8}, 2.8);
    EXPECT_FLOAT_EQ(t1(11, -1), 2.8);
    t1(11, -1) = 5.1;
    EXPECT_FLOAT_EQ(t1(11, -1), 5.1);
}