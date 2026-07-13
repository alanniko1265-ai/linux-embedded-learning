# Linux 嵌入式学习笔记与项目代码

从零开始的 Linux 嵌入式系统学习记录，涵盖编译工具链、构建系统和调试技术。

## 📁 目录结构

```
linux-embedded-learning/
├── notes/                     # 每日学习笔记（Markdown）
│   ├── day01.md               # 环境确认和第一个 Linux C 程序
│   ├── day02.md               # gcc/g++ 编译流程
│   ├── day03.md               # Makefile 入门
│   ├── day04.md               # CMake 入门
│   └── day05.md               # gdb 调试
│
└── linux_projects/            # 对应每日的练习代码
    ├── day01_hello_linux/     # Hello World — 环境验证
    ├── day02_compile_flow/    # 多文件编译 & 静态/动态库
    ├── day03_makefile_basic/  # Makefile 编写练习
    ├── day04_cmake_basic/     # CMake 构建练习
    └── day05_gdb_debug/       # GDB 断点/单步/内存调试
```

## 🗓️ 学习路线

| 天次 | 主题 | 日期 |
|:---:|------|------|
| 01 | 环境确认 + Hello Linux C | 2026-07-08 |
| 02 | gcc/g++ 编译流程（预处理→编译→汇编→链接） | 2026-07-09 |
| 03 | Makefile 编写（变量、自动变量、伪目标） | 2026-07-10 |
| 04 | CMake 入门（CMakeLists.txt、out-of-source build） | 2026-07-13 |
| 05 | GDB 调试（断点、单步、watch、core dump） | 2026-07-13 |

## 🔧 环境

- **编译器**: gcc / g++（MinGW 或 Linux native）
- **构建工具**: GNU Make、CMake ≥ 3.16
- **调试器**: GDB
- **平台**: Windows 11 + MinGW / Linux

## 🚀 快速开始

```bash
# 进入某天的项目目录，以 day04 为例
cd linux_projects/day04_cmake_basic

# CMake 构建
mkdir -p build && cd build
cmake ..
make

# 运行
./calc
```

---

*持续更新中…*
