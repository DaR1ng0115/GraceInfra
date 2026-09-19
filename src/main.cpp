#include <iostream>
#include <chrono>
#include "tensor.h"

int main() {
    Tensor t1({3, 4, 5}, 2.0f);
    Tensor t1_silce = t1(Slice{1, 3}, 1, all);
    for(auto dim : t1_silce.shape()) {
        std::cout << dim << ",";
    }
    std::cout << "\n";
    for(auto stride : t1_silce.strides()) {
        std::cout << stride << ",";
    }
    std::cout << "\n";
    std::cout << t1_silce.offset() << "\n";
}