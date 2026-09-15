// 
// ch01_perf_tensor.cpp
//
// GraceInfra
// Created by DaR1ng on 26-9-10

/*
本文件为tensor类的性能测试，我们将使用Google benchmark进行性能测试
*/

#include "benchmark/benchmark.h"
#include "benchmark/state.h"
#include "benchmark/utils.h"
#include "tensor.h"
#include "matrix.h"
#include <cstdint>

static void BM_DefaultConstructTensor(benchmark::State& state) {
    for(auto _ : state) {
        Tensor defaultTensor{};

        benchmark::DoNotOptimize(defaultTensor);
    }
}

BENCHMARK(BM_DefaultConstructTensor)
    ->Repetitions(10);

static void BM_DefaultConstructMatrix(benchmark::State& state) {
    for(auto _ : state) {
        Matrix defaultMatrix{};

        benchmark::DoNotOptimize(defaultMatrix);
    }
}

BENCHMARK(BM_DefaultConstructMatrix)
    ->Repetitions(10);

static void BM_SingleArgConstructTensor(benchmark::State& state) {
    for(auto _ : state) {
        Tensor signgleArgTensor({128, 64});

        benchmark::DoNotOptimize(signgleArgTensor);
    }
}

BENCHMARK(BM_SingleArgConstructTensor)
    ->Repetitions(10);

static void BM_SingleArgConstructMatrix(benchmark::State& state) {
    for(auto _ : state) {
        Matrix singleArgMatrix(128, 64);

        benchmark::DoNotOptimize(singleArgMatrix);
    }
}

BENCHMARK(BM_SingleArgConstructMatrix)
    ->Repetitions(10);

static void BM_FillDataConstructTensor_4KiB(benchmark::State& state) {
    for(auto _ : state) {
        Tensor fillDataTensor({32, 32}, 5.0f);

        benchmark::DoNotOptimize(fillDataTensor);
    }
}

BENCHMARK(BM_FillDataConstructTensor_4KiB)
    ->Repetitions(10);

static void BM_FillDataConstructTensor_8KiB(benchmark::State& state) {
    for(auto _ : state) {
        Tensor fillDataTensor({32, 64}, 5.0f);

        benchmark::DoNotOptimize(fillDataTensor);
    }
}

BENCHMARK(BM_FillDataConstructTensor_8KiB)
    ->Repetitions(10);

static void BM_FillDataConstructTensor_16KiB(benchmark::State& state) {
    for(auto _ : state) {
        Tensor fillDataTensor({64, 64}, 5.0f);

        benchmark::DoNotOptimize(fillDataTensor);
    }
}

BENCHMARK(BM_FillDataConstructTensor_16KiB)
    ->Repetitions(10);

static void BM_FillDataConstructTensor_32KiB(benchmark::State& state) {
    for(auto _ : state) {
        Tensor fillDataTensor({128, 64}, 5.0f);

        benchmark::DoNotOptimize(fillDataTensor);
    }
}

BENCHMARK(BM_FillDataConstructTensor_32KiB)
    ->Repetitions(10);

static void BM_FillDataConstructTensor_32KiB_ExternalShape(benchmark::State& state) {
    std::vector<int64_t> shape{128, 64};
    for(auto _ : state) {
        Tensor fillDataTensor(shape, 5.0f);

        benchmark::DoNotOptimize(fillDataTensor);
    }
}

BENCHMARK(BM_FillDataConstructTensor_32KiB_ExternalShape)
    ->Repetitions(10);

static void BM_FillDataConstructTensor_64KiB(benchmark::State& state) {
    for(auto _ : state) {
        Tensor fillDataTensor({128, 128}, 5.0f);

        benchmark::DoNotOptimize(fillDataTensor);
    }
}

BENCHMARK(BM_FillDataConstructTensor_64KiB)
    ->Repetitions(10);

static void BM_FillDataConstructTensor_128KiB(benchmark::State& state) {
    for(auto _ : state) {
        Tensor fillDataTensor({256, 128}, 5.0f);

        benchmark::DoNotOptimize(fillDataTensor);
    }
}

BENCHMARK(BM_FillDataConstructTensor_128KiB)
    ->Repetitions(10);

static void BM_FillDataConstructMatrix_4KiB(benchmark::State& state) {
    for(auto _ : state) {
        Matrix fillDataMatrix(32, 32, 5.0f);

        benchmark::DoNotOptimize(fillDataMatrix);
    }
}

BENCHMARK(BM_FillDataConstructMatrix_4KiB)
    ->Repetitions(10);

static void BM_FillDataConstructMatrix_8KiB(benchmark::State& state) {
    for(auto _ : state) {
        Matrix fillDataMatrix(32, 64, 5.0f);

        benchmark::DoNotOptimize(fillDataMatrix);
    }
}

BENCHMARK(BM_FillDataConstructMatrix_8KiB)
    ->Repetitions(10);

static void BM_FillDataConstructMatrix_16KiB(benchmark::State& state) {
    for(auto _ : state) {
        Matrix fillDataMatrix(64, 64, 5.0f);

        benchmark::DoNotOptimize(fillDataMatrix);
    }
}

BENCHMARK(BM_FillDataConstructMatrix_16KiB)
    ->Repetitions(10);


static void BM_FillDataConstructMatrix_32KiB(benchmark::State& state) {
    for(auto _ : state) {
        Matrix fillDataMatrix(128, 64, 5.0f);

        benchmark::DoNotOptimize(fillDataMatrix);
    }
}

BENCHMARK(BM_FillDataConstructMatrix_32KiB)
    ->Repetitions(10);

static void BM_FillDataConstructMatrix_64KiB(benchmark::State& state) {
    for(auto _ : state) {
        Matrix fillDataMatrix(128, 128, 5.0f);

        benchmark::DoNotOptimize(fillDataMatrix);
    }
}

BENCHMARK(BM_FillDataConstructMatrix_64KiB)
    ->Repetitions(10);

static void BM_FillDataConstructMatrix_128KiB(benchmark::State& state) {
    for(auto _ : state) {
        Matrix fillDataMatrix(256, 128, 5.0f);

        benchmark::DoNotOptimize(fillDataMatrix);
    }
}

BENCHMARK(BM_FillDataConstructMatrix_128KiB)
    ->Repetitions(10);

static void BM_CopyConstructTensor(benchmark::State& state) {
    Tensor tensor({256, 128}, 1.0f);
    for(auto _ : state) {
        Tensor copyTensor(tensor);

        benchmark::DoNotOptimize(copyTensor);
    }
}

BENCHMARK(BM_CopyConstructTensor)
    ->Repetitions(10);

static void BM_CopyConstructMatrix(benchmark::State& state) {
    Matrix matrix(256, 128, 1.0f);
    for(auto _ : state) {
        Matrix copyMatrix(matrix);
        
        benchmark::DoNotOptimize(copyMatrix);
    }
}

BENCHMARK(BM_CopyConstructMatrix)
    ->Repetitions(10);

static void BM_AddOperatorTensor_4KiB(benchmark::State& state) {
    Tensor tensor_1({32, 32}, 2.0f);
    Tensor tensor_2({32, 32}, 4.0f);
    Tensor tensor_3;
    for(auto _ : state) {
        tensor_3 = tensor_1 + tensor_2;

        benchmark::DoNotOptimize(tensor_3);
    }
}

BENCHMARK(BM_AddOperatorTensor_4KiB)
    ->Repetitions(10);

static void BM_AddOperatorTensor_8KiB(benchmark::State& state) {
    Tensor tensor_1({64, 32}, 2.0f);
    Tensor tensor_2({64, 32}, 4.0f);
    Tensor tensor_3;
    for(auto _ : state) {
        tensor_3 = tensor_1 + tensor_2;

        benchmark::DoNotOptimize(tensor_3);
    }
}

BENCHMARK(BM_AddOperatorTensor_8KiB)
    ->Repetitions(10);

static void BM_AddOperatorTensor_16KiB(benchmark::State& state) {
    Tensor tensor_1({64, 64}, 2.0f);
    Tensor tensor_2({64, 64}, 4.0f);
    Tensor tensor_3;
    for(auto _ : state) {
        tensor_3 = tensor_1 + tensor_2;

        benchmark::DoNotOptimize(tensor_3);
    }
}

BENCHMARK(BM_AddOperatorTensor_16KiB)
    ->Repetitions(10);

static void BM_AddOperatorMatrix_4KiB(benchmark::State& state) {
    Matrix matrix_1(32, 32, 2.0f);
    Matrix matrix_2(32, 32, 4.0f);
    Matrix matrix_3;
    for(auto _ : state) {
        matrix_3 = matrix_1 + matrix_2;

        benchmark::DoNotOptimize(matrix_3);
    }
}

BENCHMARK(BM_AddOperatorMatrix_4KiB)
    ->Repetitions(10);

static void BM_AddOperatorMatrix_8KiB(benchmark::State& state) {
    Matrix matrix_1(64, 32, 2.0f);
    Matrix matrix_2(64, 32, 4.0f);
    Matrix matrix_3;
    for(auto _ : state) {
        matrix_3 = matrix_1 + matrix_2;

        benchmark::DoNotOptimize(matrix_3);
    }
}

BENCHMARK(BM_AddOperatorMatrix_8KiB)
    ->Repetitions(10);

static void BM_AddOperatorMatrix_16KiB(benchmark::State& state) {
    Matrix matrix_1(64, 64, 2.0f);
    Matrix matrix_2(64, 64, 4.0f);
    Matrix matrix_3;
    for(auto _ : state) {
        matrix_3 = matrix_1 + matrix_2;

        benchmark::DoNotOptimize(matrix_3);
    }
}

BENCHMARK(BM_AddOperatorMatrix_16KiB)
    ->Repetitions(10);

BENCHMARK_MAIN();