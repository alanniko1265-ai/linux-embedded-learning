# Linux 嵌入式学习笔记与项目代码

从零开始的 Linux 嵌入式系统学习记录，涵盖编译工具链、构建系统、调试技术、文件 IO、系统编程、多线程、IPC、网络编程及 TCP 协议设计。

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
│   ├── day12.md                         # 进程基础：fork + execvp + waitpid
│   ├── day13.md                         # 信号基础：signal_guard
│   ├── day14.md                         # 日志模块：logger
│   ├── day15.md                         # fcntl 和非阻塞 IO
│   ├── day16.md                         # /proc、/sys、/dev 基础
│   ├── day17.md                         # ioctl 和 mmap 入门
│   ├── day18.md                         # Week 2 综合：file_monitor_tool
│   ├── day19.md                         # pthread 线程基础
│   ├── day20.md                         # 线程同步 & 生产者消费者队列
│   ├── day21.md                         # IPC 基础（管道、FIFO、共享内存、消息队列）
│   ├── day22.md                         # Week 3 综合：local_command_server
│   ├── day23.md                         # TCP echo server 基础
│   ├── day24.md                         # 多客户端 TCP server（多线程）
│   ├── day25.md                         # select 多客户端 TCP server
│   ├── day26.md                         # poll 多客户端 TCP server
│   ├── day27.md                         # epoll 多客户端 TCP server
│   ├── day28.md                         # TCP 粘包/拆包 & 应用层协议设计
│   ├── day29.md                         # TCP 请求-响应协议服务器
│   └── day30.md                         # epoll + 请求-响应协议服务器（终章）
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
│   ├── day12_process_runner/            # 进程基础 — fork + execvp + waitpid
│   ├── day13_signal_guard/              # 信号处理 — sigaction、signal mask
│   ├── day14_logger_module/             # 日志模块 — 级别、输出、线程安全
│   ├── day15_nonblock_io/               # fcntl 非阻塞 IO — 轮询读写
│   ├── day16_proc_sys_dev/              # /proc /sys /dev 文件系统探索
│   ├── day17_ioctl_mmap_intro/          # ioctl 设备控制 & mmap 内存映射
│   ├── day18_file_monitor_tool/         # Week 2 综合 — inotify 文件监控
│   ├── day19_pthread_basic/             # pthread 创建/join/detach
│   ├── day20_thread_queue/              # 互斥锁 + 条件变量 — 线程安全队列
│   ├── day21_ipc_basic/                 # 管道/FIFO/共享内存/消息队列
│   ├── day22_local_command_server/      # Week 3 综合 — 本地 IPC 命令服务
│   ├── day23_tcp_echo/                  # TCP socket 基础 — echo server
│   ├── day24_multi_client_server/       # 多线程 TCP server — 并发客户端
│   ├── day25_select_server/             # select IO 多路复用 — 单线程多客户端
│   ├── day26_poll_server/               # poll IO 多路复用 — 无 fd 上限
│   ├── day27_epoll_server/              # epoll 高性能 IO 多路复用
│   ├── day28_tcp_protocol/              # TCP 粘包拆包 — length+payload 协议
│   ├── day29_request_response/          # 请求-响应协议 — 命令分发
│   └── day30_epoll_protocol_server/     # epoll + 应用协议 — 单线程命令服务器
│
├── linux-learning-notes/                # 早期笔记镜像（day01~day12）
│   ├── notes/
│   └── projects/
│
├── qt_projects/                         # Qt 嵌入式 HMI 项目（并行轨道）
├── Linux_Embedded_App_Summer_Plan.md    # 暑期学习总体计划
├── Qt_Linux_HMI_Plan_From_Day4.md       # Qt/Linux HMI 专项路线
└── .gitignore
```

## 🗓️ 学习路线

### Week 1 — 工具链 & 构建系统

| 天次 | 主题 | 日期 |
|:---:|------|------|
| 01 | 环境确认 + Hello Linux C | 2026-07-08 |
| 02 | gcc/g++ 编译流程（预处理→编译→汇编→链接） | 2026-07-09 |
| 03 | Makefile 编写（变量、自动变量、伪目标） | 2026-07-10 |
| 04 | CMake 入门（CMakeLists.txt、out-of-source build） | 2026-07-13 |
| 05 | GDB 调试（断点、单步、watch、core dump） | 2026-07-13 |
| 06 | 静态库 (.a) 与动态库 (.so)、ar、ldd、LD_LIBRARY_PATH | 2026-07-14 |
| 07 | Week 1 复盘 — Linux 工具箱 v1（多文件 + Makefile + CLI 参数） | 2026-07-14 |

### Week 2 — 文件 IO & 系统编程

| 天次 | 主题 | 日期 |
|:---:|------|------|
| 08 | 文件 IO 基础 — open/read/write/close、my_cat、my_cp | 2026-07-15 |
| 09 | 文件 IO — 从零实现 write_all 安全写入、safe_cp | 2026-07-15 |
| 10 | 文件属性 — stat、文件类型识别、权限位解析、ls -l mode 输出 | 2026-07-16 |
| 11 | 目录遍历 — opendir/readdir/closedir、过滤 . 和 ..、结合 stat | 2026-07-16 |
| 12 | 进程基础 — fork + execvp + waitpid、WIFEXITED、proc_runner | 2026-07-17 |
| 13 | 信号基础 — sigaction、signal mask、signal_guard 实战 | 2026-07-17 |
| 14 | 日志模块 — 日志级别、格式化输出、模块化设计 | 2026-07-18 |

### Week 3 — 高级 IO & 系统接口

| 天次 | 主题 | 日期 |
|:---:|------|------|
| 15 | fcntl 和非阻塞 IO — 轮询读写、O_NONBLOCK | 2026-07-18 |
| 16 | /proc、/sys、/dev 基础 — 内核信息接口探索 | 2026-07-20 |
| 17 | ioctl 和 mmap 入门 — 设备控制 & 内存映射 IO | 2026-07-20 |
| 18 | Week 2 复盘 — file_monitor_tool（inotify 文件监控） | 2026-07-21 |
| 19 | pthread 线程基础 — 创建/join/detach、线程生命周期 | 2026-07-21 |
| 20 | 线程同步 — 互斥锁 + 条件变量、生产者消费者队列 | 2026-07-22 |
| 21 | IPC 基础 — 管道/FIFO/共享内存/消息队列 | 2026-07-22 |

### Week 4 — 网络编程

| 天次 | 主题 | 日期 |
|:---:|------|------|
| 22 | Week 3 复盘 — local_command_server（本地 IPC 命令服务） | 2026-07-23 |
| 23 | TCP echo server 基础 — socket/bind/listen/accept | 2026-07-23 |
| 24 | 多客户端 TCP server — 多线程并发模型 | 2026-07-24 |
| 25 | select IO 多路复用 — 单线程管理多客户端 | 2026-07-24 |
| 26 | poll IO 多路复用 — 无 fd 数量限制的并发方案 | 2026-07-27 |
| 27 | epoll IO 多路复用 — Linux 高性能事件驱动 | 2026-07-27 |
| 28 | TCP 粘包/拆包 — length+payload 应用层协议设计 | 2026-07-28 |

### Week 5 — 协议 & 综合实战

| 天次 | 主题 | 日期 |
|:---:|------|------|
| 29 | TCP 请求-响应协议 — 命令分发与结构化通信 | 2026-07-29 |
| 30 | 终章 — epoll + 应用协议单线程命令服务器 | 2026-07-30 |

## 🔧 环境

- **编译器**: gcc / g++（MinGW 或 Linux native）
- **构建工具**: GNU Make、CMake ≥ 3.16
- **调试器**: GDB
- **平台**: Windows 11 + MinGW / Linux

## 🚀 快速开始

```bash
# 文件 IO — my_cat / my_cp
cd linux_projects/day08_file_io
make
./build/my_cat data/input.txt
./build/my_cp data/input.txt data/output.txt

# 文件属性 — stat 信息查看器
cd linux_projects/day10_file_stat
make
./build/file_probe /home

# 目录遍历 — 递归目录扫描
cd linux_projects/day11_dir_scan
make
./build/dir_scan ./data

# 进程管理 — fork + execvp + waitpid
cd linux_projects/day12_process_runner
make
./build/proc_runner /bin/ls data
./build/proc_runner /bin/echo hello linux

# pthread — 线程安全队列
cd linux_projects/day20_thread_queue
make
./build/thread_queue

# TCP echo server
cd linux_projects/day23_tcp_echo
make
./build/tcp_echo_server 8080
# 另开终端: nc localhost 8080

# epoll 高性能服务器
cd linux_projects/day27_epoll_server
make
./build/epoll_server 8080

# epoll + 应用协议 — 终极命令服务器
cd linux_projects/day30_epoll_protocol_server
make
./build/epoll_protocol_server 8080
# 另开终端: nc localhost 8080 然后输入协议格式的命令
```

---

*30 天学习计划已完成 🎉*
