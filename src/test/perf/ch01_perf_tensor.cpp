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

static void BM_FillDataConstructTensor(benchmark::State& state) {
    for(auto _ : state) {
        Tensor fillDataTensor({128, 64}, 5.0f);

        benchmark::DoNotOptimize(fillDataTensor);
    }
}

BENCHMARK(BM_FillDataConstructTensor)
    ->Repetitions(10);

static void BM_FillDataConstructMatrix(benchmark::State& state) {
    for(auto _ : state) {
        Matrix fillDataMatrix(128, 64, 5.0f);

        benchmark::DoNotOptimize(fillDataMatrix);
    }
}

BENCHMARK(BM_FillDataConstructMatrix)
    ->Repetitions(10);

BENCHMARK_MAIN();