#include <iostream>
#include <chrono>
#include "matrix.h"
#include "tensor.h"


int main() {
    std::vector<int64_t> v1(3);
    v1 = {100000, 5000};

    Tensor t1(v1, 5);
    Tensor t2(v1, 10);
    Tensor t3(v1);

    auto start_tensor = std::chrono::high_resolution_clock::now();

    t3 = t1+t2;

    auto end_tensor = std::chrono::high_resolution_clock::now();
    auto ms_tensor = std::chrono::duration_cast<std::chrono::milliseconds>(end_tensor - start_tensor);
    std::cout<< ms_tensor.count() / 1000.0 << "s\n";

    Matrix m1(100000, 5000, 5);
    Matrix m2(100000, 5000, 10);
    Matrix m3(100000, 5000);

    auto start_matrix = std::chrono::high_resolution_clock::now();

    m3 = m1 + m2;

    auto end_matrix = std::chrono::high_resolution_clock::now();
    auto ms_matrix = std::chrono::duration_cast<std::chrono::milliseconds>(end_matrix - start_matrix);
    std::cout << ms_matrix.count() / 1000.0 << "s\n";

    std::cout << m3(300, 255) << "\n";

    return 0;
}