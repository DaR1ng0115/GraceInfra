# README.md

## 引言
欢迎来到ch03-Python-API, 本章聚焦于使用nanobind实现框架的Python接口, 领略Python作为胶水语言的便利性, 最后我们将使用pytest复现ch02的GTest, 提升我们在后续的测试中的效率.

## 导论
在正式开始之前, 首先要确保这些事情:
- Python 3.10+ or PyPy 7.3.12+
- CMake 3.20+
- Clang 8+, GCC 8+, MSVC2019+

nanobind的官方GitHub仓库地址: https://github.com/wjakob/nanobind.git


## 预告
下一章为ch04-dispatch, 在第四章中, 我们将实现泛化数据类型, 使用模版和dtype, 实现类型擦除, 最终在Python环境中可以灵活使用各类数据类型, 同时为未来的多device打下基础并预留接口, 实现CUDA和Metal加速的时候将更加便捷而无需大量重构