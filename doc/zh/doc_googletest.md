# doc_googletest.md

## CH01

### 什么是googletest?
googletest是谷歌推出的一个适用于C++的测试工具，其提供了各类方法和结构便于进行测试。

### 为什么需要测试？
这是软件工程中非常重要的一个命题，如果说代码架构是骨架，代码核心功能是灵魂，那么测试则是血肉。没有测试的项目是经不起敲打的，所以测试对于项
目的鲁棒性是必要的。既然如此，我们测试的宗旨和目标是什么呢？测试旨在确保我们正确地构建了项目和我们构建了正确的项目，但测试无法保证项目不存
在Bug，只能证明项目存在Bug。因此，我们在测试中需要寻找那些我们已知存在，但尚未发生的Bug。  

### 为什么选择googletest
googletest在C++领域是非常成熟和全面的测试框架，它被各大企业和个人开发者所使用，其丰富的断言体系和强大的测试组织很适合GraceInfra，因此
我们选择了googletest。

### googletest的安装
这里推荐的方法是使用CMake中FetchContent自动下载安装gtest，具体实现是这样的
```txt
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.18.0/googletest-1.18.0.tar.gz
)
FetchContent_MakeAvailable(googletest)
```
其中`FetchContent_MakeAvailable`意思是确保指定的依赖项已经被获取，并将其添加到主项目中。

### 如何使用googletest？
在本项目中，我们以CMake与goolgetest配合使用。在实现了FetchContent自动安装gtest之后，使用`target_link_libraries(test PRIVATE GTest::gtest_main)`将gtest库与test库进行链接，随后即可进行配置和构建。  
在测试文件中，首先需要`#include "gtest/gtest.h"`引入gtest头文件。测试的代码需要用TEST宏，具体为`TEST(测试套件名，测试用例名)`，比如我想测试构造函数中的默认构造函数，可以这样写`TEST(ConstructorFunction, DefaultConstructor)`。  
在测试代码实现中，有两大类，它们分别是EXPECT_* 和 ASSERT_*，比如EXPECT_EQ表示两个参数是相等的，但ASSERT的区别在于一旦触发，该TEST内的后续测试将被截断，不会继续进行，适合有拓扑顺序的。