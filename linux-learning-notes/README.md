# Linux 学习笔记

Linux 系统编程学习记录，包含项目实践和理论笔记。

## 目录结构

```
linux-learning-notes/
├── projects/                    # 项目实践
│   ├── day01_hello_linux/       # Linux 下的第一个程序
│   ├── day02_compile_flow/      # 编译流程：预处理、编译、汇编、链接
│   ├── day03_makefile_basic/    # Makefile 基础：自动化构建
│   ├── day04_cmake_basic/       # CMake 构建系统入门
│   ├── day05_gdb_debug/         # GDB 调试：断点、单步、内存检查
│   ├── day06_static_shared_lib/ # 静态库 (.a) 与动态库 (.so)
│   ├── day07_linux_toolbox_v1/  # Week 1 复盘：Linux 工具箱 v1
│   ├── day08_file_io/           # 文件 IO 基础：my_cat / my_cp
│   ├── day09_file_io_utils/     # 文件 IO：safe_cp / write_all 封装
│   └── day10_file_stat/         # 文件属性：stat、权限、类型识别
└── notes/                       # 学习笔记
    ├── day01.md                 # Linux 基础入门笔记
    ├── day02.md                 # 编译流程笔记
    ├── day03.md                 # Makefile 笔记
    ├── day04.md                 # CMake 笔记
    ├── day05.md                 # GDB 调试笔记
    ├── day06.md                 # 静态库与动态库笔记
    ├── day07.md                 # Week 1 复盘笔记
    ├── day08.md                 # 文件 IO 基础笔记
    ├── day09.md                 # 文件 IO 安全封装笔记
    └── day10.md                 # 文件属性与目录识别笔记
```

## 学习进度

| 天数 | 主题 | 内容 |
|------|------|------|
| Day 01 | Linux 入门 | 第一个 Linux C 程序，gcc 基本使用 |
| Day 02 | 编译流程 | 预处理 → 编译 → 汇编 → 链接 四阶段实践 |
| Day 03 | Makefile | Makefile 基础语法，自动化构建管理 |
| Day 04 | CMake | CMake 构建系统入门，out-of-source build |
| Day 05 | GDB 调试 | 断点、单步、watch、core dump 调试 |
| Day 06 | 库文件 | 静态库 (.a) 与动态库 (.so) 制作与使用 |
| Day 07 | Week 1 复盘 | 多文件 C 命令行工具箱项目 |
| Day 08 | 文件 IO | open/read/write/close 系统调用，my_cat/my_cp |
| Day 09 | 文件 IO 进阶 | write_all 安全封装，safe_cp 实现 |
| Day 10 | 文件属性 | stat、文件类型识别、权限位解析、ls -l mode 输出 |
