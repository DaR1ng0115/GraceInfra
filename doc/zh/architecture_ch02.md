# architecture_ch02.md

## 引言
由于框架开始变得庞大，各种逻辑，设定也隐藏于代码之中，难以阅读和理解，因此我决定从本章开始撰写这个名为architecture，即体系结构的文档，在该文档中，我将重点阐述整体的设计逻辑，框架约定，以及部分的设计取舍等，同时其中也会记录我的一些思考与想法。

## 整体设计
本章承接上一章的内容，将Tensor的底层数据完全的剥离了出来，成为了一个独立的Buffer类。在这样的设计中，Tensor成为了一种语义逻辑解释，因而我们可以很好的实现view机制，但这也显著增加了复杂性，比如区分逻辑索引和物理索引。  
这里我将用一个图展示本章的结构:

Tensor - shape, strides, offset -> 无填充构造，有填充构造，拷贝构造，移动构造，切片构造，numel接口（逻辑元素数量），加法运算符重载，等号运算符重载  
  |                                 
  V  
Buffer - ref_count, numel, data -> 无填充构造，有填充构造，拷贝构造，retain增加引用计数，release减少引用计数

## 约定
1. Tensor状态约定，第一种正常构造，shape，strides，offset均正常，buffer指针指向一个buffer对象；第二种为构造空对象，shape，strides均为空，offset为0，buffer指针为nullptr；第三种为构造包含0维度的对象，shape中包含0维度，strides由shape正常根据公式计算得出，offset为0，buffer指针为nullptr。此外，需要注意的是，如果对一个非空Tensor对象使用例如tensor(Slice{1, 1})，返回的对象状态为shape中包含0维度，strides根据公式计算所得，offset为0，buffer指针指向原先Tensor对象的buffer对象，因此上述状态约定仅为构造状态，不一定为最终状态。

   
## 实现详解
1. 关于buffer
buffer作为Tensor的底层数据类型，从逻辑上来说不应该暴露给外界，也就是不允许在外界直接构造和使用buffer。因此使用了类的私有化和友元的形式，既保证了私有特性，又保证了Tensor可以使用buffer，不过需要注意的是，我们仍需要防止Tensor通过某些方式间接暴露buffer给外界。而禁用buffer的移动构造，拷贝赋值和移动赋值也是一定程度上减少可能存在的隐患。  
2. 什么是view，为什么需要view？
view机制是相当重要的一个概念，简单来说，view就是数据不变，“看法”变了。对于同一个连续一维数组，我们可以以不同的的规则来看，比如改变形状的操作，reshape，将一个3x4的矩阵变成2x6的矩阵，flatten，将张量展平成一维，transpose，矩阵转置或多维度张量交换维度；Slice切片，只需要部分维度或者部分长度。  
3. 为什么需要一个切片构造？
这个切片构造是由问题催生出来的，之前实现的构造函数中，拷贝构造函数复制数据，创建新tensor和buffer对象；移动构造转移所有权，而在对括号运算符重载为切片功能的时候，就需要复制该张量的属性，但是不复制buffer，直接指向同一个buffer，并增加该buffer的引用计数，当然也可以不实现切片构造，直接在函数内实现逻辑也是可行的。  

## 语法详解
本章中使用了相当多的C++的新特性，在注释中进行了大概的解释，在此处则进行更深入的理解
1. 模版
template \<typename T>是模版的基础语法，这个结构可以放在函数和类前，比如:
```cpp
template <typename T>
T add(T a, T b) {
  return a+b;
}

template <typename T>
class Couple{
private:
  T a;
  T b;

public:
  T multiply(const Matrix& matrix) {
    return matrix.a * matrix.b;
  }
};
```
实际上模版的本质就是编译器帮你生成不同类型的函数或类，比如当你调用上面的add函数时，使用了float和double，其实就等价于:
```cpp
float add(float a, float b) {
  return a+b;
}

double add(double a, double b) {
  return a+b;
}
// 这些都是编译器帮你自动生成的
```
但是，既然编译器会根据模版自动生成代码，那编译器就必须在编译阶段就能“看到”你到底使用了什么类型，编译器才能对应生成类型的代码，实例化发生的位置必须能看到模版定义，所以要么在头文件中完整实现代码逻辑，要么**显式实例化**，比如:
```h
// add.h
template <typename T>
T add(T a, T b);
```
```cpp
// add.cpp
template float add<float>(float, float);
template double add<double>(double, double);
```
可变参数模版则是模版语法的延伸，其解决了我们在上一章中遇到的困境: 参数数量不固定，参数类型不固定。可变参数模版的基础语法如下:
```cpp
template <typename... Args>
void print(Args... args){
// 函数内容
}
```
在这段代码示例中，`...`的意思是**声明参数包**，它还有另一个用法，如`print(args...)`，其含义为**展开参数包**。而`typename... Args`中的Args是一个**模版参数包**，代表一串类型。`Args... args`中的args是一个**函数参数包**，代表一串函数实参。  
实际上C++中的可变参数模版和Python中的`*args`相似，如果你对于Python中的这个语法熟悉，那么可以对比理解并迁移到C++中
```python
# Python
def print_all(first=None, *rest):
  if first is None: return
  print(first, end=' ')
  print_all(*rest)
```
```cpp
// C++
void print_all() {
  std::cout << "\n";
}

template<typename T, typename... Rest>
void print_all(T first, Rest... rest) {
  std::cout << first << ' ';
  print_all(rest...); 
}
```


