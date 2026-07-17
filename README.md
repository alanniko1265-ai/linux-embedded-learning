# Linux 嵌入式学习笔记与项目代码

从零开始的 Linux 嵌入式系统学习记录，涵盖编译工具链、构建系统、调试技术、文件 IO 及后续系统编程。

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
│   ├── day07.md                         # Week 1 复盘：Linux 工具箱 v1
│   ├── day08.md                         # 文件 IO 基础：open/read/write/close
│   ├── day09.md                         # 从零实现安全文件复制（write_all 封装）
│   ├── day10.md                         # 文件属性：stat、权限位、类型识别
│   ├── day11.md                         # 目录遍历：opendir/readdir/closedir + stat
│   └── day12.md                         # 进程基础：fork + execvp + waitpid
│
├── linux_projects/                      # Linux C/C++ 练习项目
│   ├── day01_hello_linux/               # Hello World — 环境验证
│   ├── day02_compile_flow/              # 多文件编译流程
│   ├── day03_makefile_basic/            # Makefile 编写练习
│   ├── day04_cmake_basic/               # CMake 构建练习
│   ├── day05_gdb_debug/                 # GDB 断点/单步/内存调试
│   ├── day06_static_shared_lib/         # 静态库 (.a) 与动态库 (.so)
│   ├── day07_linux_toolbox_v1/          # Week 1 复盘 — 多文件 C 命令行工具
│   ├── day08_file_io/                   # 文件 IO — my_cat / my_cp 系统调用练习
│   ├── day09_file_io_utils/             # 文件 IO — safe_cp / write_all 封装
│   ├── day10_file_stat/                 # 文件属性 — stat、类型识别、权限解析
│   ├── day11_dir_scan/                  # 目录遍历 — opendir/readdir/closedir
│   └── day12_process_runner/            # 进程基础 — fork + execvp + waitpid
│
├── linux-learning-notes/                # 学习笔记与项目（镜像结构）
│   ├── notes/                           # 笔记副本（day01~day12）
│   └── projects/                        # 项目副本（day01~day12）
│
├── qt_projects/                         # Qt 嵌入式 HMI 项目（Day 4+ 并行轨道）
├── Linux_Embedded_App_Summer_Plan.md    # 暑期学习总体计划
├── Qt_Linux_HMI_Plan_From_Day4.md       # Qt/Linux HMI 专项路线
└── .gitignore
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
| 08 | 文件 IO 基础 — open/read/write/close、my_cat、my_cp | 2026-07-15 |
| 09 | 文件 IO — 从零实现 write_all 安全写入、safe_cp | 2026-07-15 |
| 10 | 文件属性 — stat、文件类型识别、权限位解析、ls -l mode 输出 | 2026-07-16 |
| 11 | 目录遍历 — opendir/readdir/closedir、过滤 . 和 ..、结合 stat | 2026-07-16 |
| 12 | 进程基础 — fork + execvp + waitpid、WIFEXITED、proc_runner | 2026-07-17 |

## 🔧 环境

- **编译器**: gcc / g++（MinGW 或 Linux native）
- **构建工具**: GNU Make、CMake ≥ 3.16
- **调试器**: GDB
- **平台**: Windows 11 + MinGW / Linux

## 🚀 快速开始

```bash
# 以 day08 文件 IO 为例
cd linux_projects/day08_file_io
make
./build/my_cat data/input.txt
./build/my_cp data/input.txt data/output.txt

# 以 day10 文件属性为例
cd linux_projects/day10_file_stat
make
./build/file_probe /home

# 以 day11 目录遍历为例
cd linux_projects/day11_dir_scan
make
./build/dir_scan ./data

# 以 day12 进程管理为例
cd linux_projects/day12_process_runner
make
./build/proc_runner /bin/ls data
./build/proc_runner /bin/echo hello linux
```

---

*持续更新中…*
