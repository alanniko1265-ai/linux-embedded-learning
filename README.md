# Linux 嵌入式学习笔记与项目代码

> 从零开始的 Linux 嵌入式系统编程学习记录 —— 覆盖编译工具链、构建系统、调试技术、文件 IO、进程管理、信号处理、非阻塞 IO、虚拟文件系统、ioctl 与 mmap。

---

## 📋 目录

- [项目概览](#项目概览)
- [目录结构](#目录结构)
- [学习路线](#学习路线)
- [Week 1：编译工具链](#week-1编译工具链)
- [Week 2：系统编程](#week-2系统编程)
- [Week 3：设备接口与虚拟文件系统](#week-3设备接口与虚拟文件系统)
- [环境要求](#环境要求)
- [快速开始](#快速开始)
- [并行学习轨道](#并行学习轨道)
- [相关文档](#相关文档)

---

## 项目概览

本仓库记录了从 **2026-07-08** 开始的三周 Linux 嵌入式 C 编程自学过程。每天包含：

- 📝 **学习笔记**（`notes/`）：目标清单、命令记录、概念讲解、踩坑记录、每日总结
- 💻 **项目代码**（`linux_projects/`）：完整的 C 项目，含源码、Makefile / CMake 构建脚本、测试数据

**学习方式**：每个概念先理解原理，再动手写代码验证，最后记录踩坑经历和解决思路。所有项目均可独立编译运行。

**技术路线**：从 `gcc` 命令行开始 → Makefile / CMake 自动化构建 → GDB 调试 → 静态/动态库制作 → POSIX 系统调用 → 进程与信号 → 非阻塞 IO → 模块化日志系统。

---

## 目录结构

```
linux-embedded-learning/
│
├── notes/                              # 📝 每日学习笔记（Markdown）
│   ├── day01.md                         # 环境确认和第一个 Linux C 程序
│   ├── day02.md                         # gcc/g++ 编译流程（预处理→汇编→链接）
│   ├── day03.md                         # Makefile 入门（变量、自动变量、伪目标）
│   ├── day04.md                         # CMake 入门（CMakeLists.txt、out-of-source build）
│   ├── day05.md                         # gdb 调试（断点、单步、watch、core dump）
│   ├── day06.md                         # 静态库和动态库（ar、ldd、LD_LIBRARY_PATH）
│   ├── day07.md                         # Week 1 复盘：Linux 工具箱 v1
│   ├── day08.md                         # 文件 IO 基础：open/read/write/close
│   ├── day09.md                         # 安全文件复制：write_all 封装、短写入处理
│   ├── day10.md                         # 文件属性：stat、权限位、类型识别、ls -l mode
│   ├── day11.md                         # 目录遍历：opendir/readdir/closedir + stat
│   ├── day12.md                         # 进程基础：fork + execvp + waitpid
│   ├── day13.md                         # 信号处理：sigaction、SIGINT、优雅退出
│   ├── day14.md                         # 日志模块：时间戳、分级日志、模块封装
│   ├── day15.md                         # 非阻塞 IO：fcntl、O_NONBLOCK、EAGAIN
│   ├── day16.md                         # /proc、/sys、/dev — 虚拟文件系统与设备文件
│   └── day17.md                         # ioctl 和 mmap 入门
│
├── linux_projects/                      # 💻 Linux C 练习项目
│   ├── day01_hello_linux/               # Hello World — 环境验证
│   ├── day02_compile_flow/              # 多文件编译四阶段（预处理→汇编→链接）
│   ├── day03_makefile_basic/            # Makefile 编写练习
│   ├── day04_cmake_basic/               # CMake 构建练习
│   ├── day05_gdb_debug/                 # GDB 断点/单步/内存调试
│   ├── day06_static_shared_lib/         # 静态库 (.a) 与动态库 (.so)
│   ├── day07_linux_toolbox_v1/          # Week 1 复盘 — 多文件 C CLI 工具
│   ├── day08_file_io/                   # 文件 IO — my_cat / my_cp 系统调用
│   ├── day09_file_io_utils/             # 文件 IO — write_all 封装 / safe_cp
│   ├── day10_file_stat/                 # 文件属性 — stat、类型识别、权限解析
│   ├── day11_dir_scan/                  # 目录遍历 — opendir/readdir/closedir
│   ├── day12_process_runner/            # 进程管理 — fork + execvp + waitpid
│   ├── day13_signal_guard/              # 信号处理 — SIGINT 捕获与优雅关闭
│   ├── day14_logger_module/             # 日志模块 — 多文件 C 项目 + 时间戳日志
│   ├── day15_nonblock_io/               # 非阻塞 IO — fcntl + O_NONBLOCK + EAGAIN
│   ├── day16_proc_sys_dev/              # 系统探测 — /proc、/dev/null、/dev/zero
│   └── day17_ioctl_mmap_intro/          # ioctl 终端查询 + mmap 文件映射
│
├── linux-learning-notes/                # 学习笔记与项目（镜像结构）
│   ├── notes/                           # 笔记副本（day01~day17）
│   └── projects/                        # 项目副本（day01~day17）
│
├── qt_projects/                         # Qt 嵌入式 HMI 项目（Day 4+ 并行轨道）
├── Linux_Embedded_App_Summer_Plan.md    # 暑期学习总体计划
├── Qt_Linux_HMI_Plan_From_Day4.md       # Qt / Linux HMI 专项路线图
└── .gitignore
```

---

## 学习路线

### 📅 全 17 天总览

| 天次 | 主题 | 日期 | 关键 API / 工具 |
|:---:|------|:---:|------|
| 01 | 环境确认 + Hello Linux C | 07-08 | `gcc`, `printf`, `-Wall -g -o` |
| 02 | gcc 编译流程 | 07-09 | `gcc -E/-S/-c`, 预处理→汇编→链接 |
| 03 | Makefile 入门 | 07-10 | `make`, 目标/依赖/命令, `$@` `$<`, `.PHONY` |
| 04 | CMake 入门 | 07-13 | `CMakeLists.txt`, `add_executable`, out-of-source build |
| 05 | GDB 调试 | 07-13 | `break`, `run`, `next`, `step`, `print`, `backtrace` |
| 06 | 静态库与动态库 | 07-14 | `ar rcs`, `-fPIC -shared`, `ldd`, `LD_LIBRARY_PATH` |
| 07 | Week 1 复盘 | 07-14 | `argc/argv`, `strcmp`, `time()`, 多文件项目 |
| 08 | 文件 IO 基础 | 07-15 | `open`, `read`, `write`, `close`, `perror`, `O_CREAT` |
| 09 | 文件 IO 进阶 | 07-15 | `write_all` 封装, 短写入处理, `const void *` |
| 10 | 文件属性 | 07-16 | `stat`, `struct stat`, `S_ISREG`, `S_ISDIR`, 权限位 |
| 11 | 目录遍历 | 07-16 | `opendir`, `readdir`, `closedir`, `struct dirent` |
| 12 | 进程管理 | 07-17 | `fork`, `execvp`, `waitpid`, `WIFEXITED`, `WEXITSTATUS` |
| 13 | 信号处理 | 07-17 | `sigaction`, `SIGINT`, `sig_atomic_t`, 优雅退出 |
| 14 | 日志模块 | 07-18 | `fopen`, `fprintf`, `strftime`, 多文件模块封装 |
| 15 | 非阻塞 IO | 07-18 | `fcntl`, `F_GETFL`/`F_SETFL`, `O_NONBLOCK`, `EAGAIN` |
| 16 | /proc、/sys、/dev | 07-20 | `/proc/cpuinfo`, `/proc/meminfo`, `/dev/null`, `/dev/zero` |
| 17 | ioctl 与 mmap 入门 | 07-20 | `ioctl`, `TIOCGWINSZ`, `mmap`, `munmap`, `MAP_PRIVATE` |

---

## Week 1：编译工具链

**目标**：掌握从源代码到可执行文件的完整路径，具备独立搭建 C 项目构建环境的能力。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 01 | `hello_linux` | 第一个 Linux C 程序，理解 gcc 基本用法 |
| 02 | `compile_flow` | 手动执行预处理→编译→汇编→链接四阶段 |
| 03 | `makefile_basic` | 手写 Makefile，理解增量编译和伪目标 |
| 04 | `cmake_basic` | 编写 CMakeLists.txt，掌握 out-of-source build |
| 05 | `gdb_debug` | 用 GDB 定位段错误和逻辑 bug |
| 06 | `static_shared_lib` | 制作 `.a` 静态库和 `.so` 动态库 |
| 07 | `linux_toolbox_v1` | 综合项目：多文件 + Makefile + CLI 参数解析 |

## Week 2：系统编程

**目标**：掌握 POSIX 系统调用，理解 Linux 进程模型和信号机制，具备编写模块化 C 程序的能力。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 08 | `file_io` | `my_cat`（读取并输出文件）、`my_cp`（复制文件） |
| 09 | `file_io_utils` | `safe_cp`（安全的文件复制，处理短写入） |
| 10 | `file_stat` | `file_probe`（输出文件大小、类型、权限 mode 字符串） |
| 11 | `dir_scan` | `dir_scan`（递归风格遍历目录，逐项输出类型和大小） |
| 12 | `process_runner` | `proc_runner`（fork 子进程执行任意命令，报告退出码） |
| 13 | `signal_guard` | `signal_guard`（捕获 Ctrl+C，信号驱动优雅关闭+日志） |
| 14 | `logger_module` | `logger_demo`（多文件模块：init → info/warn/error → close） |
| 15 | `nonblock_io` | `nonblock_demo`（fcntl 设置 stdin 非阻塞，处理 EAGAIN） |

## Week 3：设备接口与虚拟文件系统

**目标**：理解 Linux 设备文件模型，掌握 ioctl 设备控制和 mmap 内存映射两种关键接口，为后续嵌入式驱动开发打基础。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 16 | `proc_sys_dev` | `system_probe`（读取 /proc/cpuinfo/meminfo，读写 /dev/null/zero） |
| 17 | `ioctl_mmap_intro` | `ioctl_mmap_intro`（ioctl 查询终端窗口大小，mmap 映射文件到内存） |

---

## 环境要求

| 工具 | 用途 | 最低版本 |
|------|------|------|
| **gcc** | C 编译器 | 任意（推荐 9+） |
| **GNU Make** | 构建自动化 | 3.81+ |
| **CMake** | 跨平台构建（Day 4-5） | 3.16+ |
| **GDB** | 调试器（Day 5） | 任意 |
| **Linux / WSL** | 运行环境 | Ubuntu 20.04+ (WSL2) |

**平台说明**：本项目在 Windows 11 + WSL (Ubuntu) 环境下开发。所有代码使用 POSIX API，在原生 Linux 或 WSL 中均可编译运行。MinGW 可编译部分早期项目，但涉及 `fork`、`sigaction` 等系统调用的项目需要真实 Linux 内核。

---

## 快速开始

每个项目目录结构统一，使用方式一致：

```bash
# 通用构建流程（以 day10 为例）
cd linux_projects/day10_file_stat
make                # 编译，产物在 build/
./build/file_probe /home   # 运行

# 或一键编译并运行
make run
```

### 各项目速览

```bash
# === Week 1 ===
# Day 01 — Hello World
cd linux_projects/day01_hello_linux
gcc -Wall -g -O0 -o hello_linux hello_linux.c && ./hello_linux

# Day 03 — Makefile
cd linux_projects/day03_makefile_basic
make && ./build/calculator

# Day 07 — 工具箱（命令行参数：version / time / log / help）
cd linux_projects/day07_linux_toolbox_v1
make && ./build/linux_toolbox time

# === Week 2 ===
# Day 08 — 文件 IO（my_cat / my_cp）
cd linux_projects/day08_file_io
make && ./build/my_cat data/input.txt

# Day 09 — 安全复制（write_all 防短写入）
cd linux_projects/day09_file_io_utils
make && ./build/safe_cp data/input.txt data/output.txt

# Day 10 — 文件属性探测
cd linux_projects/day10_file_stat
make && ./build/file_probe /home

# Day 11 — 目录扫描
cd linux_projects/day11_dir_scan
make && ./build/dir_scan ./data

# Day 12 — 进程运行器
cd linux_projects/day12_process_runner
make
./build/proc_runner /bin/ls data
./build/proc_runner /bin/echo hello linux

# Day 13 — 信号守卫（按 Ctrl+C 触发优雅退出）
cd linux_projects/day13_signal_guard
make && ./build/signal_guard

# Day 14 — 日志模块演示
cd linux_projects/day14_logger_module
make && ./build/logger_demo
cat logs/app.log   # 查看带时间戳的分级日志

# Day 15 — 非阻塞 IO（stdin 不卡住，输入 quit 退出）
cd linux_projects/day15_nonblock_io
make && ./build/nonblock_demo

# Day 16 — 系统探测（读取 /proc 和 /dev 设备文件）
cd linux_projects/day16_proc_sys_dev
make && ./build/system_probe cpu
./build/system_probe mem
./build/system_probe null
./build/system_probe zero

# Day 17 — ioctl 与 mmap（终端窗口查询 + 文件内存映射）
cd linux_projects/day17_ioctl_mmap_intro
make && ./build/ioctl_mmap_intro winsize
./build/ioctl_mmap_intro mmap
```

---

## 并行学习轨道

除主线 Linux C 系统编程外，本仓库还包含两条并行轨道：

### Qt 嵌入式 HMI（`qt_projects/`）

从 Day 4 开始并行的 Qt/C++ 学习线，面向嵌入式 Linux HMI 应用开发。涵盖 Qt Widgets、信号与槽、串口通信、TCP 客户端、多线程 Worker 等。

详见 [Qt_Linux_HMI_Plan_From_Day4.md](./Qt_Linux_HMI_Plan_From_Day4.md)

### 学习笔记镜像（`linux-learning-notes/`）

笔记与项目的完整镜像副本，保持与主目录同步更新。

---

## 项目规范

- **代码风格**：C11 标准，统一使用 `include/` + `src/` + `build/` 目录结构
- **构建约定**：Makefile 提供 `all`（默认）、`run`、`clean` 三个标准目标
- **编译选项**：`-Wall -g -O0`（全警告 + 调试符号 + 无优化）
- **头文件保护**：所有 `.h` 使用 `#ifndef` / `#define` / `#endif` 守卫
- **错误处理**：系统调用返回值检查 + `perror()` 输出

---

## 相关文档

- **[Linux_Embedded_App_Summer_Plan.md](./Linux_Embedded_App_Summer_Plan.md)** — 暑期学习总体计划
- **[Qt_Linux_HMI_Plan_From_Day4.md](./Qt_Linux_HMI_Plan_From_Day4.md)** — Qt / Linux HMI 专项路线图

---

<p align="center">
  <sub>从编译选项到进程信号，每天进步一点点 🚀</sub>
</p>
