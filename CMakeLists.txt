cmake_minimum_required(VERSION 3.20) # CMake版本要求                                                                                                                                                                                                                          

project(GraceInfra VERSION 0.1.0) # 项目名称和版本号

# 全局配置 : C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 自动生成 compile_commands.json

# 创建可执行文件
set(SOURCES
    src/tensor.cpp
    src/matrix.cpp
    src/main.cpp
)

add_executable(main ${SOURCES})

# 头文件路径
target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 使用FetchContent模块自动下载googletest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.18.0/googletest-1.18.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

# 创建测试代码的可执行文件
add_executable(test_tensor
    src/tensor.cpp
    src/test/unit/ch01_test_tensor.cpp
)

# 测试代码的头文件路径
target_include_directories(test_tensor PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 将测试文件与googletest库进行链接
target_link_libraries(test_tensor GTest::gtest_main)

# 设置是否开启内存测试参数
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

# 设置内存测试
if(ENABLE_ASAN)
    target_compile_options(test_tensor PRIVATE
        -fsanitize=address
        -fno-omit-frame-pointer
        -g
        -O0
    )

    target_link_options(test_tensor PRIVATE
        -fsanitize=address
    )
endif()


