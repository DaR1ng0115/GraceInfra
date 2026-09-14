# doc_googlebenchmark.md

## 什么是googlebenchmark
googlebenchmark是谷歌推出的一个性能测试工具，可适用于C++，用于精确测量代码片段的性能，和gtest是类似的  

## 为什么使用googlebenchmark
googlebenchmark(后续简称为benchmark)使用较为便捷，功能丰富强大，对于本框架来说，通用型也很强，相较于perf这类Linux专属的性能分析工具，benchmark配合CMake可以在各类平台使用，比如我正在使用Windows笔记本和Mac双端开发，benchmark完全可以迁移使用。

## 如何安装googlebenchmark
安装benchmark的方法和gtest非常相似
```txt
FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        main
    )

    FetchContent_MakeAvailable(googlebenchmark)
```
随后将benchmark库与perf库进行链接

## 如何使用googlebenchmark
benchmark的使用方法和gtest不同一样，在使用benchmark时，你需要定义一个静态函数，然后注册BENCHMARK宏
```cpp
static void BM_FillDataConstructTensor_8KiB(benchmark::State& state) {
    for(auto _ : state) {
        Tensor fillDataTensor({32, 64}, 5.0f);

        benchmark::DoNotOptimize(fillDataTensor);
    }
}

BENCHMARK(BM_FillDataConstructTensor_8KiB)
    ->Repetitions(10);
```
函数的参数是固定的`benchmark::State& state`，性能测试的主体在for循环中，你需要实现一个范围for循环，即`for(auto _ : state)`供benchmark自动控制循环次数，for循环体内则是测试的主体代码，比如此处测试的是构造一个形状为(32, 64)，所有数据均为5.0f的张量。函数结束后，需要注册宏，即`BENCHMARK(BM_FillDataConstructTensor_8KiB)`，后面的`->Repetitions(10)`的意思是整个函数的重复次数，这次需要区分的两个核心概念是重复次数和迭代次数，迭代次数就是for循环的次数，由benchmark自动控制，让每一轮的时间足够长，再取平均值，减少因时间太短导致的误差，而重复次数则是手动的，控制跑多少轮，每一轮都会刷新状态，让数据的可信度更高。
