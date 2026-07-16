# Day 4 - CMake 入门

日期：2026-07-13

## 今日目标

- [x] 理解 `CMakeLists.txt` 的作用。
- [x] 理解 out-of-source build。
- [x] 能使用 `cmake ..` 配置项目。
- [x] 能使用 `cmake --build .` 构建项目。
- [x] 能运行 CMake 构建出的程序。
- [x] 能解释 `add_executable`。
- [x] 能解释 `target_include_directories`。
- [x] 能说清楚 Makefile 和 CMake 的区别。

## 今日项目

项目目录：

```text
linux_projects/day04_cmake_basic
```

项目结构：

```text
CMakeLists.txt
include/calc.h
src/calc.c
src/main.c
```

## 命令记录

### 进入目录

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day04_cmake_basic
```

### 创建 build 目录

```bash
mkdir -p build
cd build
```

我的理解：创建并进入build文件，注意-p的作用是表示如果目录已存在就不报错；如果中间目录不存在，可以递归创建。

### 配置项目

```bash
cmake ..
```

输出中的关键信息：

```text
1. 编译器诊断信息（环境检查）
-- The C compiler identification is GNU 11.4.0
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
特性检查信息（能力检查）
-- Detecting C compile features
-- Detecting C compile features - done
蓝图生成信息（最终宣告）
-- Configuring done
-- Generating done
-- Build files have been written to: .../build

```

生成了哪些文件：

```text
Makefile
CMakeCache.txt
CMakeFiles/
cmake_install.cmake

```

我的理解：检查环境，并生成施工文件

### 构建项目

```bash
cmake --build .
```

输出：

```text
[ 33%] Building C object CMakeFiles/calc_tool.dir/src/main.c.o
[ 66%] Building C object CMakeFiles/calc_tool.dir/src/calc.c.o
[100%] Linking C executable calc_tool
[100%] Built target calc_tool

```

我的理解：执行Makefile的规定程序

### 运行程序

```bash
./calc_tool
```

输出：

```text
CMake calc tool
24 + 6 = 30
24 - 6 = 18
24 * 6 = 144
24 / 6 = 4


```

## CMake 概念

### CMakeLists.txt

记录：CMakeLists.txt 用来描述项目结构、源文件、头文件路径、编译选项和构建目标。CMake 根据它生成 Makefile 等底层构建文件。

### cmake ..

记录：向上一级的目录找寻Cmake的配置文件，然后在当前文件夹配置cmake

### cmake --build .

记录：cmake --build . 使用当前 build 目录中的构建系统真正执行编译和链接，生成可执行程序。

### out-of-source build

记录：编译产生的文件全放到一个独立文件夹里

### add_executable

记录：add_executable 定义一个可执行目标，说明最终程序名以及参与构建的源文件。

### target_include_director
记录：头文件路径

### target_compile_options

记录：编译参数

## Makefile 和 CMake 的区别

我的理解：

```text
Makefile 直接描述具体编译规则和命令；CMake 描述项目结构和构建目标，再自动生成 Makefile 等底层构建系统。CMake 更适合跨平台、多目录、多模块项目。

```

## 今日总结

今天我学会了：

1.Cmake基本命令

今天我还不理解：

1.无

明天 Day 5 要进入 gdb 调试：学习如何定位程序崩溃和查看变量。
