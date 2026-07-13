# Day 2 - gcc/g++ 编译流程

日期：2026-07-09

## 今日目标

- [x] 理解预处理。
- [x] 理解编译。
- [x] 理解汇编。
- [x] 理解链接。
- [x] 能手动生成 `.i` 文件。
- [x] 能手动生成 `.s` 文件。
- [x] 能手动生成 `.o` 文件。
- [x] 能把多个 `.o` 链接成可执行文件。
- [x] 能解释头文件和源文件的区别。

## 今日项目

项目目录：

```text
linux_projects/day02_compile_flow
```

源文件：

```text
include/add.h
src/add.c
src/main.c
```

## 命令记录

### 一步到位编译

```bash
gcc -Wall -g -O0 -Iinclude src/main.c src/add.c -o build/calc_demo
./build/calc_demo
```

输出：

```text
20 + 8 = 28
20 - 8 = 12


```

### 预处理

```bash
gcc -E -Iinclude src/main.c -o build/main.i
```

我的理解：替换宏定义，#include被展开

### 编译到汇编

```bash
gcc -S -Iinclude src/main.c -o build/main.s
```

我的理解：.s 是汇编文件，是比 C 更接近 CPU 指令的文本形式，但还不是最终机器码。

### 编译到目标文件

```bash
gcc -c -Wall -g -O0 -Iinclude src/main.c -o build/main.o
gcc -c -Wall -g -O0 -Iinclude src/add.c -o build/add.o
```

我的理解：为下一步的连接做准备

### 链接

```bash
gcc build/main.o build/add.o -o build/calc_demo
./build/calc_demo
```

我的理解：我们连接而不直接编译，在大型项目的开发有至关重要的作用，如果只改动了一小部分编译就太耗费时间了，只需要将改动部分和其他部分连接即可
.o 是目标文件，包含某个源文件编译后的机器码和符号信息，但还需要链接才能变成可执行文件。

## 故意制造的链接错误

命令：

```bash
gcc build/main.o -o build/bad_demo
```

报错：

```text
undefined reference to `add'
undefined reference to `sub'
collect2: error: ld returned 1 exit status
```

为什么报错：因为调用了,add和sub,但是又没有给add.o

## 今日概念

### .c

记录：程序实现

### .h

记录：函数与结构声明

### .i

记录：预处理，替换宏定义和展开include

### .s

记录：汇编，查看编译器的优化和查看代码的底层逻辑

### .o

记录：二进制机器码独立编译，解决动一块至全身的问题

### 可执行文件

记录：将.o缝合到一起

### -Iinclude

记录：在当前目录找不到include "xxx.h",到指定include/文件夹寻找

## 今日总结

今天我学会了：

1.各种后缀的概念

今天我还不理解：

1.无

明天 Day 3 要进入 Makefile：让这些编译命令自动化。
