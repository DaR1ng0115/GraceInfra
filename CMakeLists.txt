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


