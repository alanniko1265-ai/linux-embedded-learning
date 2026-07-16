# Day 4 - CMake 入门

今日目标：用 CMake 构建一个多文件 C 项目，并理解它和 Makefile 的关系。

## 核心理解

Makefile 更像是直接写“怎么编译”：

```text
gcc -Wall -g -O0 -Iinclude -c src/main.c -o build/main.o
gcc build/main.o build/calc.o -o build/calc_tool
```

CMake 更像是描述“项目是什么”：

```text
项目名是什么
使用什么语言
有哪些源文件
生成什么目标
头文件目录在哪里
编译参数是什么
```

CMake 本身通常不直接编译代码，它会生成底层构建系统，比如 Makefile，然后再由 `make` 或 `cmake --build` 真正编译。

## 项目结构

```text
day04_cmake_basic/
├── CMakeLists.txt
├── include/
│   └── calc.h
├── src/
│   ├── calc.c
│   └── main.c
└── README.md
```

## 进入目录

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day04_cmake_basic
```

## 第一步：查看项目

```bash
find . -maxdepth 3 -type f
cat CMakeLists.txt
cat include/calc.h
cat src/calc.c
cat src/main.c
```

## 第二步：创建 build 目录

```bash
mkdir -p build
cd build
```

这叫 out-of-source build。

意思是：

```text
源代码放在项目根目录
编译产物放在 build 目录
源代码目录保持干净
```

## 第三步：配置项目

```bash
cmake ..
```

这里的 `..` 表示上一级目录，也就是 `CMakeLists.txt` 所在目录。

配置完成后，查看生成了什么：

```bash
ls
```

你会看到类似：

```text
CMakeCache.txt
CMakeFiles
Makefile
cmake_install.cmake
```

重点：这个 `Makefile` 是 CMake 自动生成的，不是我们手写的。

## 第四步：构建项目

```bash
cmake --build .
```

或者：

```bash
make
```

更推荐先习惯：

```bash
cmake --build .
```

因为它不绑定具体底层构建工具。

## 第五步：运行程序

```bash
./calc_tool
```

预期输出：

```text
CMake calc tool
24 + 6 = 30
24 - 6 = 18
24 * 6 = 144
24 / 6 = 4
```

## 第六步：重新构建

不改代码时再执行：

```bash
cmake --build .
```

你会发现它不会完整重编所有东西。

修改 `../src/main.c` 后再执行：

```bash
cmake --build .
./calc_tool
```

观察它重新编译了哪些文件。

## 第七步：清理构建目录

回到项目根目录：

```bash
cd ..
```

删除 build 目录：

```bash
rm -rf build
```

重新完整构建：

```bash
mkdir build
cd build
cmake ..
cmake --build .
./calc_tool
```

## CMakeLists.txt 逐行理解

```cmake
cmake_minimum_required(VERSION 3.16)
```

要求最低 CMake 版本。

```cmake
project(day04_cmake_basic VERSION 1.0 LANGUAGES C)
```

定义项目名、版本和语言。

```cmake
set(CMAKE_C_STANDARD 11)
```

指定使用 C11 标准。

```cmake
add_executable(calc_tool
    src/main.c
    src/calc.c
)
```

生成一个可执行程序 `calc_tool`，它由 `main.c` 和 `calc.c` 编译链接而成。

```cmake
target_include_directories(calc_tool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

给 `calc_tool` 添加头文件搜索路径。

等价理解：

```bash
-Iinclude
```

```cmake
target_compile_options(calc_tool PRIVATE
    -Wall
    -g
    -O0
)
```

给 `calc_tool` 添加编译参数。

## 今日验收

你能回答：

1. `CMakeLists.txt` 是做什么的？
2. 为什么推荐在 `build` 目录里执行 `cmake ..`？
3. `cmake ..` 和 `cmake --build .` 分别做什么？
4. `add_executable` 的作用是什么？
5. `target_include_directories` 和 `-Iinclude` 有什么关系？
6. Makefile 和 CMake 的区别是什么？
