# README.md

## 引言
欢迎来到ch02-buffer&template，这一章将聚焦于用于存储实际数据的buffer结构，类型模版化以及完善tensor的最终武器:可变参模版。在本章中我们将会深入重构tensor，将tensor打造为能够真正支撑深度学习训练和推理的数据类型。同时，我们将会引入更多内存对齐和SIMD的内容