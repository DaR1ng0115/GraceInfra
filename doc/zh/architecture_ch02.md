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
1. Tensor状态约定:
- 正常构造，shape，strides，offset均正常，buffer指针指向一个buffer对象；
- 构造空对象，shape，strides均为空，offset为0，buffer指针为nullptr；
- 构造包含0维度的对象，shape中包含0维度，strides由shape正常根据公式计算得出，offset为0，buffer指针为nullptr.
此外，需要注意的是，如果对一个非空Tensor对象使用例如tensor(Slice{1, 1})，返回的对象状态为shape中包含0维度，strides根据公式计算所得，offset为0，buffer指针指向原先Tensor对象的buffer对象，因此上述状态约定仅为构造状态，不一定为最终状态。

   
## 实现详解
1. 关于buffer
buffer作为Tensor的底层数据类型，从逻辑上来说不应该暴露给外界，也就是不允许在外界直接构造和使用buffer。因此使用了类的私有化和友元的形式，既保证了私有特性，又保证了Tensor可以使用buffer，不过需要注意的是，我们仍需要防止Tensor通过某些方式间接暴露buffer给外界。而禁用buffer的移动构造，拷贝赋值和移动赋值也是一定程度上减少可能存在的隐患。  
2. 什么是view，为什么需要view？
view机制是相当重要的一个概念，简单来说，view就是数据不变，“看法”变了。对于同一个连续一维数组，我们可以以不同的的规则来看，比如改变形状的操作，reshape，将一个3x4的矩阵变成2x6的矩阵，flatten，将张量展平成一维，transpose，矩阵转置或多维度张量交换维度；Slice切片，只需要部分维度或者部分长度。  
3. 为什么需要一个切片构造？
这个切片构造是由问题催生出来的，之前实现的构造函数中，拷贝构造函数复制数据，创建新tensor和buffer对象；移动构造转移所有权，而在对括号运算符重载为切片功能的时候，就需要复制该张量的属性，但是不复制buffer，直接指向同一个buffer，并增加该buffer的引用计数，当然也可以不实现切片构造，直接在函数内实现逻辑也是可行的。  

## 语法详解
本章中使用了相当多的C++的新特性，在注释中进行了大概的解释，在此处则进行更深入的理解
### 模版
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
  T multiply() {
    return a * b;
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
#include "add.h"

template<typename T>
T add(T a, T b) {
  return a + b;
}

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

### enable_if_v
先从enabel_if开始, 其简易版本如下:
```cpp
// 条件为false时, 不提供type
template<bool B, typename T = void>
struct enabel_if {};

// 条件为true时, 提供type
template<typename T>
struct enable_if<true, T> {
  using type = T;
};

// 简写: 直接取出enable_if中的type
template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;
```
所以, `typename std::enable_if<条件, int>::type`和`std::enable_if_v<条件, int>`是等价的.  
在此处, `typename`用于声明type是类型. 
于是, 在`template<typename... Args, std::enable_if_v<(std::is_integarl_v<Args> && ...), int> = 0>`中, 当条件成立时, 模版就会被替换为`template<typename... Args, int = 0>`, `int = 0`可看作占位符. 而如果没有指定类型, `std::enable_if_v`在条件成立时则会转化为void类型, 而`void = 0`是错误的, 如果不填=0, 这个未命名参数就没有默认值了, 普通调用无法使用这个重载, 也就是说, 你需要在调用时显式指定该未命名参数的值, 这是不必要的. 
反之, 如果条件不成立, 就会触发SFINAE, 全称是'Substitution Failure Is Not An Error', 意思是*替换失败不是错误*, 这个模版规则即为在特定位置替换模版失败时, 排除这个模版, 寻找其他重载. 

### lambda
先了解什么是函数对象(function object): 可以像函数一样用`()`调用的对象叫函数对象. 比如
```cpp
struct Add {
  int operator()(int a, int b) const { return a + b; }
};

Add add;
int res = add(1, 2);
```
函数对象有两个很重要的特性, 第一个是其可以携带状态, 也就是成员变量, 第二个则是性能更好, 它可以被编译器内联.
```cpp
struct Counter {
  int count = 0;
  int operator() () { count++; }
};

Counter c1, c2;
c1(); c1(); // c1.count == 2
c2(); // c2.count == 1
```

而lambda就是编译器根据表达式自动生成函数对象.
比如下面的这个lambda表达式
```cpp
auto f = [](int a, int b) { return a + b; };
```
lambda的语法分为四个部分: [捕获列表]\(参数列表) -> 返回类型 { 函数体 }. 其中返回类型可以省略.  
编译器在遇到lambda的时候, 就会生成一个匿名结构体, 比如对于上面的lambda:
```cpp
struct __lambda_1 {
  int operator()(int a, int b) const { return a + b; } // 生成的函数方法默认是const的
};
__lambda_1{}
```
如果想要把外部变量捕获到函数体中使用, 则需要使用捕获列表.
```cpp
int temp = 10;
auto is_bigger = [temp](int x) { return x > temp; };
```
捕获的方式有四种:
1. [x] 按值捕获x
2. [&x] 按引用捕获x
3. [=] 用到的变量均使用按值捕获
4. [&] 用的变量均使用按引用捕获

有两个需要注意的地方
- 按值捕获的变量无法修改, 因为函数方法默认是const, 如果想修改, 需要写`mutable: [x]`, 这样就可以修改了, 但改的仍然是lambda内部的拷贝
- 按引用捕获需要注意悬空引用, 假如捕获的变量提前释放了, 那引用的就会是一个悬空的.

但lambda的局限则在于参数类型不可变, 解决方法其实就是template, 但它的名字叫泛型lambda.
```cpp
auto f = [](auto a, auto b) { return a + b; };
// 可以接受int, float, double等多种数据类型
```
本质上就是生成了一个成员函数的模版.
```cpp
struct __lambda_2 {
  template<typename T1, typename T2>
  auto operator()(T1 a, T2 b) { return a + b; }
};
```
在C++20中, 还允许直接写出模版lambda.
```cpp
[]<typename T>(T a, T b) { return a + b; };
// 强制要求a和b的类型一致
```
不过由于该项目使用的C++标准为C++17, 所以只能使用泛型lambda
