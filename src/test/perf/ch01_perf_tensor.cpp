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

static void DISABLED_BM_DefaultConstructTensor(benchmark::State& state) {
    for(auto _ : state) {
        Tensor defaultTensor{};

        benchmark::DoNotOptimize(defaultTensor);
    }
}

BENCHMARK(DISABLED_BM_DefaultConstructTensor)
    ->Repetitions(10);

static void DISABLED_BM_DefaultConstructMatrix(benchmark::State& state) {
    for(auto _ : state) {
        Matrix defaultMatrix{};

        benchmark::DoNotOptimize(defaultMatrix);
    }
}

BENCHMARK(DISABLED_BM_DefaultConstructMatrix)
    ->Repetitions(10);

static void DISABLED_BM_SingleArgConstructTensor(benchmark::State& state) {
    for(auto _ : state) {
        Tensor signgleArgTensor({128, 64});

        benchmark::DoNotOptimize(signgleArgTensor);
    }
}

BENCHMARK(DISABLED_BM_SingleArgConstructTensor)
    ->Repetitions(10);

static void DISABLED_BM_SingleArgConstructMatrix(benchmark::State& state) {
    for(auto _ : state) {
        Matrix singleArgMatrix(128, 64);

        benchmark::DoNotOptimize(singleArgMatrix);
    }
}

BENCHMARK(DISABLED_BM_SingleArgConstructMatrix)
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

static void DISABLED_BM_CopyConstructTensor(benchmark::State& state) {
    Tensor tensor({256, 128}, 1.0f);
    for(auto _ : state) {
        Tensor copyTensor(tensor);

        benchmark::DoNotOptimize(copyTensor);
    }
}

BENCHMARK(DISABLED_BM_CopyConstructTensor)
    ->Repetitions(10);

static void DISABLED_BM_CopyConstructMatrix(benchmark::State& state) {
    Matrix matrix(256, 128, 1.0f);
    for(auto _ : state) {
        Matrix copyMatrix(matrix);
        
        benchmark::DoNotOptimize(copyMatrix);
    }
}

BENCHMARK(DISABLED_BM_CopyConstructMatrix)
    ->Repetitions(10);

BENCHMARK_MAIN();