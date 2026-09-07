cmake_minimum_required(VERSION 3.20) # CMake版本要求                                                                                                                                                                                                                          

project(GraceInfra VERSION 0.1.0) # 项目名称和版本号

# 全局配置 : C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 自动生成 compile_commands.json

# 全局头文件路径 : 使用接口库管理所有公共头文件
add_library(GraceInfra_headers INFERFACE)
target_include_directories(GraceInfra_headers INFERFACE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

include(FetchContent) 

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.18.0/googletest-1.18.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(ch01-test src/matrix.cpp src/tensor.cpp src/test/unit/ch01_test_tensor.cpp)
target_link_libraries(ch01-test GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(ch01-test)
