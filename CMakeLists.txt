cmake_minimum_required(VERSION 3.20) # CMake版本要求 

project(GraceInfra VERSION 0.1.0) # 项目名称和版本号

# 全局配置 : C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 自动生成 compile_commands.json

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# 编译目标选项
option(ENABLE_MAIN "配置main程序" OFF)
option(ENABLE_TEST "配置test程序" OFF)
option(ENABLE_ASAN "开启内存检测" OFF)
option(ENABLE_PYTHON_API "构建Python拓展" OFF)
# option(ENABLE_PERF "配置perf程序" OFF)

# 设置底层数据静态库
set(DATA_SOURCES
    src/tensor.cpp
    src/buffer.cpp
)

add_library(data STATIC ${DATA_SOURCES})

target_include_directories(data PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(data PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# main选项编译
if(ENABLE_MAIN)

    add_executable(main src/main.cpp)

    target_link_libraries(main PRIVATE data)

    # 头文件路径
    target_include_directories(main PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

endif()

# 测试选项编译
if(ENABLE_TEST)
# 使用FetchContent模块自动下载googletest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/releases/download/v1.18.0/googletest-1.18.0.tar.gz
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(test src/test/unit/ch02_test_tensor.cpp)

# 测试代码的头文件路径
    target_include_directories(test PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    target_link_libraries(test PRIVATE data)

# 将测试文件与googletest库进行链接
    target_link_libraries(test PRIVATE GTest::gtest_main)

# 设置内存测试
    if(ENABLE_ASAN)
        target_compile_options(test PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
            -g
            -O0
        )

        target_link_options(test PRIVATE
            -fsanitize=address
        )

        target_compile_options(gtest_main PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
            -g
            -O0
        )

        target_compile_options(gtest PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
            -g
            -O0
        )

        target_compile_options(data PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
            -g
            -O0
        )

    endif()
endif()

# 配置Python API拓展
if(ENABLE_PYTHON_API)
    # 查找 Python 解释器并构建所需模块
    find_package(Python 3.10 REQUIRED
        COMPONENTS Interpreter Development.Module
    )

    execute_process(
        COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
        OUTPUT_VARIABLE nanobind_ROOT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE nanobind_result
    )

    find_package(nanobind CONFIG REQUIRED)

    set(PYTHON_SOURCES
        src/py/py_tensor.cpp
    )

    nanobind_add_module(grace ${PYTHON_SOURCES})

    install(TARGETS grace
        LIBRARY DESTINATION .
        RUNTIME DESTINATION .
    )

    nanobind_add_stub(
        grace_stub
        INSTALL_TIME
        MODULE grace
        OUTPUT grace.pyi
        PYTHON_PATH "."
    )

    target_include_directories(grace PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    target_link_libraries(grace PRIVATE data)

endif()

