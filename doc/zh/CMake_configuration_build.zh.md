# CMake_configuration_build.md

## CH01

### CMake是什么？
CMake是一个“项目翻译官”，它不是编译器，而是用来生成编译命令的。而CMakeLists.txt
就是用户用来告诉他: 我的源代码在哪里，需要用到哪些库，最终要生成什么。  
[CMake官方网站](https://cmake.org/)

### 为什么要用CMake？
C++是一个编译型语言，它不像Python这类解释型语言一样，C++源文件需要通过编译器编译成二进制文件，才可被电脑运行。
因此我们需要编译器来编译C++程序，但问题在于，一旦项目过大，依赖较多，每次编译将会非常麻烦。  
比如在使用clang++编译本项目时:  
```shell
clang++ -c src/tensor.cpp -o build/tensor.o -I include
clang++ -c src/matrix.cpp -o build/matrix.o -I include
clang++ -c src/main.cpp -o build/main.o -I include
clang++ build/tensor.o build/matrix.o build/main.o -o build/GraceInfra.exe
./build/GraceInfra.exe
```
因此，手动编译及其繁琐，也很容易漏掉文件。就在这样的情况下，CMake完美解决了上面的问题。
你只需要创建一个CMakeLists.txt，在其中写入这些配置:
```cmake
project(GraceInfra)                                                  # 项目名称

set(CMAKE_CXX_STANDARD 17)                                           # 需要使用的C++标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)                                  # 强制开启C++标准检查

add_executable(main src/matrix.cpp src/tensor.cpp src/main.cpp)      # 编译并链接源文件，输出可执行文件
```
虽然目前看起来，也不算特别简便，但随着项目规模变大，CMake的优势会非常明显，而且后续我们使用GoogleTest时，搭配CMake使用也十分方便。  

### 如何安装CMake？

#### Windows平台
在powershell中输入`winget install cmake`，winget会自动下载cmake(一般是msi文件)，安装程序会自动将CMake添加到环境变量。安装完成
后，重启powershell，然后输入`cmake --version`即可验证安装是否成功  

#### macOS平台 & Linux平台
推荐使用homebrew安装，homebrew是一个强大的包管理器，如果你的电脑未安装homebrew，可以在终端中输入`/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`
安装homebrew。  
随后在终端中输入`brew install cmake`即可安装CMake。输入`cmake --version`即可验证是否安装成功

### 如何使用CMake？
首先要使用CMake，你需要在项目根目录中创建一个`CMakeLists.txt`，注意大小写。随后进入该文件进行编辑，对于目前的项目来说，可以这样构建:  
```cmake
cmake_minimum_required(VERSION 3.20)    # 1*
project(GraceInfra VERSION 0.1.0)       # 2*

set(SOURCES
    src/tensor.cpp
    src/matrix.cpp
    src/main.cpp    
)                                       # 3*

add_executable(main ${SOURCES})         # 4*

target_include_directories(main PRIVATE # 5*
    ${CMAKE_SOURCE_DIR}/include         # 6*
)
```
注:  
1. `cmake_minimum_required(VERSION 3.20)`指定所需的CMake的最低版本，比如如果使用C++17标准，建议CMake版本在3.20及以上
2. `project(GraceInfra VERSION 0.1.0)`设置项目名称和版本号
3. `set()`设置变量:  
```cmake
set(SOURCES
    src/matrix.cpp
    src/tensor.cpp
    src/main.cpp
)                               # 设置'SOURCES'变量为后面的三个文件地址，推荐定义变量名为全大写

add_executable(main ${SOURCES}) # 使用变量名时需要加上'${}'将变量名包裹，但定义变量时不需要
```
4. `add_executable`创建可执行文件，比如`add_executable(main main.cpp tensor.cpp)`实现了两件事情，第一件是将main.cpp和tensor.cpp进行编译
，第二件事情是将编译的文件链接，并输出可执行文件  

`CMakeLists.txt`构建完成之后，在终端中输入:  
```shell
mkdir build
cd build
cmake ..   #注意cmake后面是一个空格和两个英文句号
```

如果你是Windows平台，在终端中输入`cmake --build . --config Debug`,注意空格和英文句号。终端中输入`./main.exe`即可运行  
如果你是macOS或Linux平台，在终端中输入`make`即可，随后输入`./main`即可运行
5. `target_include_directories`管理头文件路径，其结构为`target_include_directories(<target> <PRIVATE|PUBLIC|INTERFACE> <目录>)`，其中的PRIVATE|PUBLIC|INTERFACE可以暂时不做了解，使用PRIVATE即可
6. `CMAKE_SOURCE_DIR`代表整个项目的最顶层源代码目录（永远不变）

### CMake进阶用法
