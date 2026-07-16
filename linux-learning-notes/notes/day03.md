# Day 3 - Makefile 入门

日期：2026-07-10

## 今日目标

- [x] 理解 Makefile 的目标。
- [x] 理解 Makefile 的依赖。
- [x] 理解 Makefile 的命令。
- [x] 能使用 `make` 构建项目。
- [x] 能使用 `make run` 运行项目。
- [x] 能使用 `make clean` 清理编译产物。
- [x] 能观察增量编译。
- [x] 能解释 `.PHONY` 的作用。

## 今日项目

项目目录：

```text
linux_projects/day03_makefile_basic
```

项目结构：

```text
Makefile
include/calc.h
src/calc.c
src/main.c
```

## 命令记录

### 第一次构建

```bash
make
```

输出：

```text
mkdir -p build
gcc -Wall -g -O0 -Iinclude -c src/main.c -o build/main.o
gcc -Wall -g -O0 -Iinclude -c src/calc.c -o build/calc.o
gcc build/main.o build/calc.o -o build/calc_tool

```

### 运行程序

```bash
make run
```

输出：

```text
./build/calc_tool
calc tool
12 + 4 = 16
12 - 4 = 8
12 * 4 = 48

```

### 第二次 make

```bash
make
```

输出：

```text
make: Nothing to be done for 'all'.
```

为什么第二次可能没有重新编译：make 会根据目标文件和依赖文件的修改时间判断是否需要重新编译。如果源文件没有变化，而 build/calc_tool 已经是最新的，就不会重复执行编译命令。

### 修改 main.c 后重新 make

我修改了哪里：

```text
加上printf("程序已经完成");

```

再次执行：

```bash
make
make run
```

输出：

```text
gcc -Wall -g -O0 -Iinclude -c src/main.c -o build/main.o
gcc -Wall -g -O0 -Iinclude -c src/calc.c -o build/calc.o
gcc build/main.o build/calc.o -o build/calc_tool
./build/calc_tool
calc tool
12 + 4 = 16
12 - 4 = 8
12 * 4 = 48
程序已完成

```

这次重新编译了哪些文件：只重新编译了 src/main.c，生成 build/main.o；然后重新链接 build/calc_tool。src/calc.c 没有修改，所以 build/calc.o 没有重新生成。

### clean

```bash
make clean
```

输出：

```text
rm -rf build

```

clean 做了什么：删除 build 目录，也就是删除 .o 目标文件和最终可执行程序，但不会删除 src、include、Makefile 等源文件。

## 今日概念

### 目标

记录：Makefile 中要生成的东西，或者要执行的动作。比如 build/calc_tool、run、clean。

### 依赖

记录：生成目标之前需要先存在或更新的文件。比如 build/calc_tool 依赖 build/main.o 和 build/calc.o。

### 命令

记录：真正执行的 shell 命令。Makefile 中命令行前面必须用 Tab。

### 增量编译

记录：make 会根据文件修改时间判断哪些文件需要重新编译。只改 main.c 时，只重新编译 main.o，再重新链接最终程序。

### .PHONY

记录：声明伪目标，表示 all、run、clean、rebuild 不是普通文件名，而是动作目标。

### $@

记录：build/main.o记录：$@ 表示当前规则的目标。比如在 build/main.o: src/main.c 这条规则中，$@ 就是 build/main.o。

### $<

记录：记录：$< 表示当前规则的第一个依赖。比如在 build/main.o: src/main.c 这条规则中，$< 就是 src/main.c。

## 今日总结

今天我学会了：

1.关于自动化构造的一些知识

今天我还不理解：

1.无

明天 Day 4 要进入 CMake：学习更现代、更适合跨平台和大型项目的构建方式。
