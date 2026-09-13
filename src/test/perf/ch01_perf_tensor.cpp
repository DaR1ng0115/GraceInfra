// 
// ch01_perf_tensor.cpp
//
// GraceInfra
// Created by DaR1ng on 26-9-10

/*
本文件为tensor类的性能测试，在第一章的性能测试中，仅使用C++标准库的chrono进行性能评估。
*/

#include <chrono>
#include <iostream>
#include <cstdlib>
#include "../../../include/tensor.h"

int main() {
// ===================== warm-up =====================
{
for(int i=0; i<1'000'000; ++i) {
    void* p = malloc(274176);
    free(p);
}
}

// ===================== 未初始化分配性能测试 =====================
{
    auto startTime_1 = std::chrono::steady_clock::now();
    for(int i=0; i<10'000'000; ++i) {
        Tensor uninitializedAllocationTensor_1({28, 102, 24}); // numel = 68544
    }
    auto endTime_1 = std::chrono::steady_clock::now();
    auto nsTime_1 = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime_1 - startTime_1);
    std::cout << "形状为(28, 102, 24)未初始化构造耗时: " << nsTime_1.count()/10'000'000.0 << " ns"<< "\n";

//     auto startTime_2 = std::chrono::steady_clock::now();
//     Tensor uninitializedAllocationTensor_2({302, 6, 3}); // numel = 5436
//     auto endTime_2 = std::chrono::steady_clock::now();
//     std::cout << "形状为(302, 6, 3)未初始化构造耗时: " << "\n";

//     auto startTime_3 = std::chrono::steady_clock::now();
//     Tensor uninitializedAllocationTensor_3({3, 4, 56, 70}); // numel = 47040
//     auto endTime_3 = std::chrono::steady_clock::now();
//     std::cout << "形状为(3, 4, 56, 70)未初始化构造耗时: " << "\n";
 }

}