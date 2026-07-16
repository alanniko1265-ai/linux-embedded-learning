# Day 7 - Week 1 复盘小项目

日期：2026-07-14

## 今日目标

- [x] 自己设计一个多文件 C 小项目。
- [x] 自己写头文件和源文件。
- [x] 自己写 Makefile。
- [x] 支持命令行参数。
- [x] 支持版本输出。
- [x] 支持时间输出。
- [x] 支持日志输出。
- [x] 支持 help。
- [x] 能处理未知命令。
- [x] 能写清楚本周学到的工具链能力。

## 今日项目

项目目录：

```text
linux_projects/day07_linux_toolbox_v1
```

项目名：

```text
linux_toolbox_v1
```

## 我设计的目录结构

```text
day07_linux_toolbox_v1/
├── include/
│   ├── log.h
│   └── time_utils.h
├── src/
│   ├── main.c
│   ├── log.c
│   └── time_utils.c
├── Makefile
└── README.md
```

## 我设计的命令

```text
linux_toolbox help
linux_toolbox version
linux_toolbox time
linux_toolbox log <message>
```

## 构建记录

### make

命令：

```bash
make
```

输出：

```text
mkdir -p build
gcc -Wall -g -O0 -Iinclude src/main.c src/log.c src/time_utils.c -o build/linux_toolbox
```

### 运行 version

命令：

```bash
./build/linux_toolbox version
```

输出：

```text
linux_toolbox version 1.0.0
```

### 运行 time

命令：

```bash
./build/linux_toolbox time
```

输出：

```text
2026-07-14 15:50:10
```

### 运行 log

命令：

```bash
./build/linux_toolbox log "device gateway started"
```

输出：

```text
[2026-07-14 15:50:10] [INFO] device gateway started
```

### 运行 help

命令：

```bash
./build/linux_toolbox help
```

输出：

```text
Usage:
  linux_toolbox help
  linux_toolbox version
  linux_toolbox time
  linux_toolbox log <message>
```

### 运行 unknown

命令：

```bash
./build/linux_toolbox unknown
```

输出：

```text
unknown command: abc
Usage:
  linux_toolbox help
  linux_toolbox version
  linux_toolbox time
  linux_toolbox log <message>
```

## 遇到的问题

1. Makefile 命令行必须使用 Tab，不能使用普通空格。
2. `main.c` 调用 `print_log` 时，如果没有 `#include "log.h"`，会出现 `implicit declaration of function` 警告。
3. `printf` 格式控制符数量和参数数量不一致时，`gcc -Wall` 会给出 warning。

## 我如何解决

1. 将 Makefile 命令行前面的空格改成 Tab。
2. 在 `main.c` 中加入 `#include "log.h"`，并在 Makefile 中加入 `-Iinclude`。
3. 检查 `printf` 的格式字符串，保证每个 `%d` / `%s` 都有对应参数。

## Week 1 总结

### gcc/g++/make/cmake/gdb 我分别怎么理解

记录：`gcc/g++` 用来直接编译 C/C++ 源文件；`make` 根据 Makefile 自动执行构建规则；`cmake` 更偏项目描述，可以生成 Makefile 等底层构建系统；`gdb` 用来调试程序，查看断点、变量和调用栈。

### .c/.h/.o/.a/.so/可执行文件之间的关系

记录：`.h` 放函数声明和类型声明；`.c` 放函数实现；`.o` 是单个源文件编译后的目标文件；`.a` 是静态库，由多个 `.o` 打包而成；`.so` 是动态库，运行时加载；可执行文件是源文件或目标文件经过链接后生成的程序。

### Makefile 和 CMake 的区别

记录：Makefile 直接描述具体编译规则和命令；CMake 描述项目结构和构建目标，再自动生成 Makefile 等底层构建系统。CMake 更适合跨平台、多目录、多模块项目。

### gdb 最有用的命令

记录：`break` 设置断点；`run` 运行程序并传参数；`next` 单步执行但不进入函数；`step` 单步执行并进入函数；`print` 查看变量；`backtrace` 查看调用栈；`quit` 退出 gdb。

## 今日总结

今天我学会了：

1. 把一个单文件命令行程序逐步拆成多文件 C 项目。
2. 用 Makefile 编译多个 `.c` 文件并生成可执行程序。
3. 通过 `argc` / `argv` 实现不同命令分支。

今天我还不理解：

1.无
2.

明天进入 Week 2：Linux 文件 IO 和系统调用。
