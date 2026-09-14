# notes_ch01.md

## 2026-9-13
今天在学习如何使用Google benchmark，同时由于构建选项太多，我决定在build文件夹中分层，即build/main build/test build/testWithASan build/perf
,因此也将旧的build目录给删除了。在随后重新配置和构建的过程中，却发生了很多问题，第一个问题是test使用了ASan，但gtest,gtest_main,data均未配置
ASan，将这些库补上ASan之后，可以正常构建和运行了  
第二个问题是，ASan报错，但奇怪的是报错并没有任何的文件和行号提示:  
```shell
=================================================================
==24144==ERROR: AddressSanitizer: attempting free on address which was not malloc()-ed: 0x021dcd311d00 in thread T0
    #0 0x7ff93b4dcb76  (C:\Users\yfx91\scoop\apps\llvm\22.1.8\lib\clang\22\lib\windows\clang_rt.asan_dynamic-x86_64.dll+0x18004cb76)
    #1 0x7ff993ef0479  (C:\WINDOWS\SYSTEM32\ucrtbased.dll+0x180080479)
    #2 0x7ff993eef6e8  (C:\WINDOWS\SYSTEM32\ucrtbased.dll+0x18007f6e8)
    #3 0x7ff993eef7bd  (C:\WINDOWS\SYSTEM32\ucrtbased.dll+0x18007f7bd)
    #4 0x7ff993f12866  (C:\WINDOWS\SYSTEM32\ucrtbased.dll+0x1800a2866)
    #5 0x7ff993f12624  (C:\WINDOWS\SYSTEM32\ucrtbased.dll+0x1800a2624)
    #6 0x7ff993f126a6  (C:\WINDOWS\SYSTEM32\ucrtbased.dll+0x1800a26a6)
    #7 0x7ff993f12e0d  (C:\WINDOWS\SYSTEM32\ucrtbased.dll+0x1800a2e0d)
    #8 0x7ff9941014a9  (C:\WINDOWS\SYSTEM32\MSVCP140D.dll+0x1800814a9)
    #9 0x7ff9941014e2  (C:\WINDOWS\SYSTEM32\MSVCP140D.dll+0x1800814e2)
    #10 0x7ff99408198b  (C:\WINDOWS\SYSTEM32\MSVCP140D.dll+0x18000198b)
    #11 0x7ff993f124f7  (C:\WINDOWS\SYSTEM32\ucrtbased.dll+0x1800a24f7)
    #12 0x7ff994102486  (C:\WINDOWS\SYSTEM32\MSVCP140D.dll+0x180082486)
    #13 0x7ff994102396  (C:\WINDOWS\SYSTEM32\MSVCP140D.dll+0x180082396)
    #14 0x7ff994102634  (C:\WINDOWS\SYSTEM32\MSVCP140D.dll+0x180082634)
    #15 0x7ff994102791  (C:\WINDOWS\SYSTEM32\MSVCP140D.dll+0x180082791)
    #16 0x7ffa0449ead9  (C:\WINDOWS\SYSTEM32\ntdll.dll+0x18011ead9)
    #17 0x7ffa04390c32  (C:\WINDOWS\SYSTEM32\ntdll.dll+0x180010c32)
    #18 0x7ffa0438e76b  (C:\WINDOWS\SYSTEM32\ntdll.dll+0x18000e76b)
    #19 0x7ffa04407dc9  (C:\WINDOWS\SYSTEM32\ntdll.dll+0x180087dc9)
    #20 0x7ffa04407df5  (C:\WINDOWS\SYSTEM32\ntdll.dll+0x180087df5)
    #21 0x7ffa0440dfbe  (C:\WINDOWS\SYSTEM32\ntdll.dll+0x18008dfbe)
    #22 0x7ffa0440c0f7  (C:\WINDOWS\SYSTEM32\ntdll.dll+0x18008c0f7)
    #23 0x7ffa0440bee1  (C:\WINDOWS\SYSTEM32\ntdll.dll+0x18008bee1)
    #24 0x7ffa0446142d  (C:\WINDOWS\SYSTEM32\ntdll.dll+0x1800e142d)

Address 0x021dcd311d00 is a wild pointer inside of access range of size 0x000000000001.
SUMMARY: AddressSanitizer: bad-free (C:\WINDOWS\SYSTEM32\ucrtbased.dll+0x180080479) 
==24144==ABORTING
```  
这个问题我是从来没遇到过的，然后我第一时间问了我的gpt，他刚开始猜测是环境变量的问题，可能ASan无法正常调用llvm-symbolizer(地址符号化工具)，然后
排除出来环境变量是没有问题的，之后验证:
```shell
.\build\testWithASan\test.exe `
      --gtest_filter=NoSuchTest `
      --gtest_color=no
```
依然报相同的错误，这段命令意思是使用一个不存在的过滤器，即运行空测试，结果依然报错，这说明测试代码本身是没有问题的，问题出在外部。最后gpt推测是Windows下的LLVM ASan和Debug CRT(调试运行库)不兼容，简单来说，ASan和Debug CRT均在监测内存数据，但它们在这个过程中"打架"冲突了，解决办法就是使用RelWithDebInfo模式进行配置，在这个模式下链接的是Release CRT，也就避免了兼容性问题。

## 2026-9-14
今天在实现性能测试，覆盖了默认构造函数，单参数构造函数和双参数构造函数。不过在双参数构造函数这里出现了有趣的现象，tensor在该项目上显著慢
于(大概3倍)matrix，于是我生成了汇编文件，发现tensor该部分是标量填充，没有向量化，随后我使用了clang++分析了编译，输出表明确实没有向量
化，原因是cannot identify array bounds(无法识别数组边界)，不过解决的办法却看起来很矛盾:float* __restrict__ p = data_;定义一个局部变量和数组边界有什么关系呢？我猜测原因可能是由于限定了在p的生命周期内，只会通过p来访问其所指向的内存，既然没有别的修改途径，那么编译器就认为这个数组边界确定了，因此可以进行向量化。  
但更有意思的是，在未加float* __restrict__ p = data_;的情况下，Windows上运行，使用clang++ -O3编译，循环未向量化，而在我的Mac上面，使用的AppleClang 21.0.0，却向量化了，说明即便都是clang++编译器，在不同的平台上表现也会截然不同。
