# Linux 嵌入式学习笔记与项目代码

从零开始的 Linux 嵌入式系统学习记录，涵盖编译工具链、构建系统、调试技术、Qt 工控 HMI 开发。

## 📁 目录结构

```
linux-embedded-learning/
├── notes/                              # 每日学习笔记（Markdown）
│   ├── day01.md                         # 环境确认和第一个 Linux C 程序
│   ├── day02.md                         # gcc/g++ 编译流程
│   ├── day03.md                         # Makefile 入门
│   ├── day04.md                         # CMake 入门
│   ├── day05.md                         # gdb 调试
│   ├── day06.md                         # 静态库和动态库
│   └── day07.md                         # Week 1 复盘：Linux 工具箱 v1
│
├── linux_projects/                      # Linux C/C++ 练习项目
│   ├── day01_hello_linux/               # Hello World — 环境验证
│   ├── day02_compile_flow/              # 多文件编译流程
│   ├── day03_makefile_basic/            # Makefile 编写练习
│   ├── day04_cmake_basic/               # CMake 构建练习
│   ├── day05_gdb_debug/                 # GDB 断点/单步/内存调试
│   ├── day06_static_shared_lib/         # 静态库 (.a) 与动态库 (.so)
│   └── day07_linux_toolbox_v1/          # Week 1 复盘 — 多文件 C 命令行工具
│
├── linux-learning-notes/                # 学习笔记整理版（day01-03）
│   ├── notes/
│   └── projects/
│
├── qt_projects/                         # Qt6 工控 HMI 通信面板
│   ├── CMakeLists.txt                   # Qt6 Widgets / Network / SerialPort
│   ├── mainwindow.h/cpp/ui              # 主窗口（TCP + 串口 + 日志）
│   ├── logmanager.h/cpp                 # 日志管理器
│   ├── tcpclientmanager.h/cpp           # TCP 客户端管理器
│   ├── serialportmanager.h/cpp          # 串口管理器
│   ├── settingsmanager.h/cpp            # 设置持久化 (QSettings)
│   ├── worker.h/cpp                     # 后台 Worker 线程
│   └── qt_day04_first_app/              # Qt 第一个应用（QMainWindow + CMake）
│
├── Linux_Embedded_App_Summer_Plan.md     # 56 天嵌入式 Linux 暑假学习计划
└── Qt_Linux_HMI_Plan_From_Day4.md       # Qt 工控 HMI 进阶路线（Day 4-45）
```

## 🗓️ 学习路线

| 天次 | 主题 | 日期 |
|:---:|------|------|
| 01 | 环境确认 + Hello Linux C | 2026-07-08 |
| 02 | gcc/g++ 编译流程（预处理→编译→汇编→链接） | 2026-07-09 |
| 03 | Makefile 编写（变量、自动变量、伪目标） | 2026-07-10 |
| 04 | CMake 入门（CMakeLists.txt、out-of-source build） | 2026-07-13 |
| 05 | GDB 调试（断点、单步、watch、core dump） | 2026-07-13 |
| 06 | 静态库 (.a) 与动态库 (.so)、ar、ldd、LD_LIBRARY_PATH | 2026-07-14 |
| 07 | Week 1 复盘 — Linux 工具箱 v1（多文件 + Makefile + CLI 参数） | 2026-07-14 |

### Qt 工控 HMI 路线

| 天次 | 主题 |
|:---:|------|
| 04 | 第一个 Qt 应用 — QMainWindow + CMake |
| 05 | 工程结构 — mainwindow.h/cpp/ui |
| 06 | 连接面板 — TCP / 串口 / 日志 / 设置持久化 / QThread |

## 🔧 环境

- **编译器**: gcc / g++（MinGW 或 Linux native）
- **构建工具**: GNU Make、CMake ≥ 3.16
- **调试器**: GDB
- **Qt**: Qt 6.5+ (Widgets / Network / SerialPort)
- **平台**: Windows 11 + MinGW / Linux

## 🚀 快速开始

### Linux C 项目

```bash
# 以 day06 静态/动态库为例
cd linux_projects/day06_static_shared_lib
make
./build/app
```

### Qt 项目

```bash
# 在 Qt Creator 中打开 CMakeLists.txt，或命令行构建：
cd qt_projects
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/mingw_64
cmake --build build
./build/ConnectPanel
```

---

*持续更新中…*
