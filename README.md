# Linux 嵌入式学习笔记与项目代码

> 从零开始的 Linux 嵌入式系统编程学习记录 —— 覆盖编译工具链、构建系统、调试技术、文件 IO、进程管理、信号处理、非阻塞 IO、虚拟文件系统、ioctl 与 mmap、文件监控、多线程编程、生产者消费者模型、IPC 进程间通信（pipe / FIFO）、本地命令服务器综合项目与 TCP 网络编程（socket / echo server / 多客户端 / select / poll / epoll IO 多路复用、应用层协议设计、请求-响应协议），通过模块化重构掌握真实嵌入式项目的工程结构，交叉编译部署到 ARM 开发板，接入真实 LED 硬件控制与按键输入，注册为 systemd 系统服务实现开机自启，通过 logrotate 管理日志轮转防止存储耗尽，并通过 diagnostics 模块实现运行诊断接口（diag 命令）。

---

## 📋 目录

- [项目概览](#项目概览)
- [目录结构](#目录结构)
- [学习路线](#学习路线)
- [Week 1：编译工具链](#week-1编译工具链)
- [Week 2：系统编程](#week-2系统编程)
- [Week 3：设备接口与虚拟文件系统](#week-3设备接口与虚拟文件系统)
- [Week 4：多线程编程](#week-4多线程编程)
- [Week 5：IPC 进程间通信](#week-5ipc-进程间通信)
- [Week 6：IPC 综合项目](#week-6ipc-综合项目)
- [Week 7：网络编程](#week-7网络编程)
- [Week 8：应用协议 & 综合实战](#week-8应用协议--综合实战)
- [Week 9：设备网关渐进式迭代](#week-9设备网关渐进式迭代)
- [Week 10：项目整理 & 开发板上板](#week-10项目整理--开发板上板)
- [Week 11：交叉编译 & 开发板上板实战](#week-11交叉编译--开发板上板实战)
- [Week 12：运维与日志管理](#week-12运维与日志管理)
- [环境要求](#环境要求)
- [快速开始](#快速开始)
- [并行学习轨道](#并行学习轨道)
- [相关文档](#相关文档)

---

## 项目概览

本仓库记录了从 **2026-07-08** 开始的 Linux 嵌入式 C 编程自学过程，持续更新中。每天包含：

- 📝 **学习笔记**（`notes/`）：目标清单、命令记录、概念讲解、踩坑记录、每日总结
- 💻 **项目代码**（`linux_projects/`）：完整的 C 项目，含源码、Makefile / CMake 构建脚本、测试数据

**学习方式**：每个概念先理解原理，再动手写代码验证，最后记录踩坑经历和解决思路。所有项目均可独立编译运行。

**技术路线**：从 `gcc` 命令行开始 → Makefile / CMake 自动化构建 → GDB 调试 → 静态/动态库制作 → POSIX 系统调用 → 进程与信号 → 非阻塞 IO → 模块化日志系统 → 虚拟文件系统与设备接口 → 文件监控综合项目 → 多线程与生产者消费者模型 → IPC 进程间通信（pipe / FIFO）→ 本地命令服务器综合项目 → TCP 网络编程（socket / echo server / 多客户端 / select / poll / epoll IO 多路复用）→ TCP 应用层协议设计与请求-响应模型 → epoll + 应用协议单线程命令服务器 → 项目结构重构：protocol / command / server / client 四模块分离 → 设备网关渐进式迭代（日志模块 → 配置文件 → 优雅退出 → 设备状态 → LED 状态管理 → 动态采样 → 统一响应格式与错误码）→ 项目整理与简历版收尾 → 开发板基础环境准备（串口 / USB 网络 / SSH / scp）→ 交叉编译 ARM 程序部署到 i.MX6ULL → 设备网关上板运行 → sysfs LED 控制 → 设备网关接入真实 LED 硬件 → 按键输入（input event）→ 按键状态接入设备网关（pthread 共享状态）→ systemd 服务部署（开机自启）→ logrotate 日志轮转（存储保护）→ diagnostics 运行诊断接口（diag 命令：version/pid/uptime_sec）。

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
│   ├── day17.md                         # ioctl 和 mmap 入门
│   ├── day18.md                         # Week 2 综合：file_monitor_tool 文件监控
│   ├── day19.md                         # pthread 线程基础：创建、join、互斥锁
│   ├── day20.md                         # 线程同步：生产者消费者队列（mutex + cond）
│   ├── day21.md                         # IPC 基础：pipe 父子进程通信 + FIFO 命名管道
│   ├── day22.md                         # 本地命令服务器：FIFO IPC + server 长期运行 + 信号优雅退出
│   ├── day23.md                         # TCP echo server：socket 编程、TCP 字节流、client/server 架构
│   ├── day24.md                         # 多客户端 TCP server：pthread 每连接一线程
│   ├── day25.md                         # select IO 多路复用：单线程管理多客户端
│   ├── day26.md                         # poll IO 多路复用：pollfd 数组与 events/revents
│   ├── day27.md                         # epoll IO 多路复用：epoll_create1/ctl/wait
│   ├── day28.md                         # TCP 应用层协议：长度头 + payload
│   ├── day29.md                         # TCP 请求-响应协议：send_exact/read_exact
│   ├── day30.md                         # epoll + 请求-响应协议服务器（终章）
│   ├── day31.md                         # 设备网关项目结构重构：多文件模块化
│   ├── day32.md                         # 设备网关加入日志模块：logger 多文件集成
│   ├── day33.md                         # 设备网关加入配置文件：key=value 解析
│   ├── day34.md                         # 设备网关优雅退出：sigaction 信号处理
│   ├── day35.md                         # 设备网关加入设备状态模块：command/state 分离
│   ├── day36.md                         # 设备网关 LED 状态管理：状态持久化
│   ├── day37.md                         # 设备状态动态采样：模拟传感器数据
│   ├── day38.md                         # 统一响应格式与错误码：code + msg 规范
│   ├── day39.md                         # 项目整理与简历版收尾：文档化与可展示
│   ├── day40.md                         # 开发板基础环境准备：串口/USB网络/SSH/scp
│   ├── day41.md                         # 交叉编译入门：ARM 工具链、static/dynamic、scp 部署
│   ├── day42.md                         # 设备网关上板运行：交叉编译 + 开发板部署
│   ├── day43.md                         # sysfs LED 控制：/sys/class/leds/*/brightness
│   ├── day44.md                         # 设备网关接入真实 LED：led_control 模块集成
│   ├── day45.md                         # 按键输入：读取 /dev/input/event1 控制 LED
│   ├── day46.md                         # 按键状态接入设备网关：key_input线程 + 共享DeviceState
│   ├── day47.md                         # RS485 接口验证与板级排障
│   ├── day48.md                         # systemd 服务部署：demo-gateway.service 开机自启
│   ├── day53.md                         # 网关日志轮转：logrotate 规则与存储保护
│   ├── day54.md                         # 运行诊断接口：diag 命令、version/pid/uptime_sec
│   └── purchase_reminders.md            # 硬件采购提醒：提前规划采购清单
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
│   ├── day17_ioctl_mmap_intro/          # ioctl 终端查询 + mmap 文件映射
│   ├── day18_file_monitor_tool/         # 文件监控工具：配置解析 + stat + 信号
│   ├── day19_pthread_basic/             # pthread 基础：线程创建、互斥锁
│   ├── day20_thread_queue/              # 线程安全队列：mutex + cond + 生产者消费者
│   ├── day21_ipc_basic/                 # IPC 基础：pipe + FIFO 进程间通信
│   ├── day22_local_command_server/       # 本地命令服务器：FIFO + 信号 + 日志
│   ├── day23_tcp_echo/                  # TCP echo server/client：socket 编程 + 字节流
│   ├── day24_multi_client_server/        # 多客户端 TCP server：pthread 每连接一线程
│   ├── day25_select_server/              # select IO 多路复用：单线程管理多客户端
│   ├── day26_poll_server/                # poll IO 多路复用：单线程管理多客户端
│   ├── day27_epoll_server/               # epoll IO 多路复用：Linux 高效事件通知
│   ├── day28_tcp_protocol/               # TCP 应用层协议：粘包/拆包与长度头
│   ├── day29_request_response/           # TCP 请求-响应协议：命令解析与响应
│   ├── day30_epoll_protocol_server/      # epoll + 应用协议 — 单线程命令服务器
│   └── day31_device_gateway_refactor/    # 设备网关重构：protocol/command/server/client 模块化
│   ├── day32_device_gateway_logger/       # 设备网关 + 日志模块：logger 集成与文件写入
│   ├── day33_device_gateway_config/       # 设备网关 + 配置文件：key=value 解析
│   ├── day34_device_gateway_graceful_shutdown/  # 设备网关 + 信号优雅退出
│   ├── day35_device_gateway_state/        # 设备网关 + 设备状态模块：状态结构体分离
│   ├── day36_device_gateway_led_state/    # 设备网关 + LED 状态管理：状态持久化
│   ├── day37_device_gateway_dynamic_state/  # 设备网关 + 动态采样：模拟传感器
│   ├── day38_device_gateway_response_code/  # 设备网关 + 统一响应格式与错误码
│   ├── day39_device_gateway_final/        # 设备网关最终版：项目整理与简历版收尾
│   ├── day40_board_bringup/              # 开发板基础环境准备：串口/USB网络/SSH/scp
│   ├── day41_cross_compile/              # 交叉编译：ARM 工具链编译 + scp 部署到开发板
│   ├── day42_gateway_on_board/           # 设备网关交叉编译上板：WSL→ARM→开发板运行
│   ├── day43_led_control/               # sysfs LED 控制：C 程序控制开发板真实 LED 亮灭
│   ├── day44_gateway_led_hardware/       # 设备网关 + LED 硬件：led_control 模块集成到网关
│   ├── day45_key_input/                 # 按键输入：读取 input event 控制 LED
│   ├── day46_gateway_key_status/         # 按键状态接入设备网关：key_input线程 + 共享DeviceState
│   ├── day48_gateway_systemd_service/    # systemd 服务部署：开机自启、日志与状态管理
│   ├── day53_gateway_logrotate/          # logrotate 日志轮转：规则文件、copytruncate、compress
│   └── day54_gateway_diagnostics/        # 运行诊断接口：diag 命令、version/pid/uptime_sec
│
├── linux-learning-notes/                # 学习笔记与项目（镜像结构）
│   ├── notes/                           # 笔记副本（day01~day25）
│   └── projects/                        # 项目副本（day01~day25）
│
├── qt_projects/                         # Qt 嵌入式 HMI 项目（并行轨道）
├── Linux_Embedded_App_Summer_Plan.md    # 暑期学习总体计划
├── Qt_Linux_HMI_Plan_From_Day4.md       # Qt / Linux HMI 专项路线图
└── .gitignore
```

---

## 学习路线

### 📅 学习总览

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
| 18 | Week 2 综合项目：文件监控 | 07-21 | `stat`, `fopen`/`fgets`, `sscanf`, `sigaction`, 配置文件解析 |
| 19 | pthread 线程基础 | 07-21 | `pthread_create`, `pthread_join`, `pthread_mutex_lock`/`unlock` |
| 20 | 线程同步：生产者消费者队列 | 07-22 | `pthread_cond_wait`, `pthread_cond_signal`, 环形队列, 生产者消费者模型 |
| 21 | IPC 基础：pipe 与 FIFO | 07-22 | `pipe`, `mkfifo`, `fork`, `read`/`write`, FIFO reader/writer |
| 22 | 本地命令服务器 | 07-23 | FIFO IPC、server 长期运行、keep_fd 技巧、信号优雅退出、日志 |
| 23 | TCP echo server | 07-23 | `socket`, `bind`, `listen`, `accept`, `send`/`recv`, TCP 字节流, client/server 架构 |
| 24 | 多客户端 TCP server | 07-24 | `pthread_create`, `pthread_detach`, `malloc`/`free` 传参, 每连接一线程 |
| 25 | select IO 多路复用 | 07-24 | `select`, `fd_set`, `FD_ZERO`/`FD_SET`/`FD_ISSET`, 单线程管理多客户端 |
| 26 | poll IO 多路复用 | 07-27 | `poll`, `struct pollfd`, `events`/`revents`, `POLLIN` |
| 27 | epoll IO 多路复用 | 07-27 | `epoll_create1`, `epoll_ctl`, `epoll_wait`, `EPOLLIN` |
| 28 | TCP 应用层协议 | 07-28 | `uint32_t`, `htonl`, `ntohl`, `length + payload`, `read_exact` |
| 29 | TCP 请求-响应协议 | 07-29 | `send_exact`, `read_exact`, `send_message`, `read_message`, 命令响应 |
| 30 | epoll + 请求-响应协议 | 07-30 | `epoll` + `length+payload` + 请求-响应, 单线程命令服务器 |
| 31 | 设备网关项目结构重构 | 07-31 | protocol/command/server/client 模块化、头文件声明 vs 源文件实现、多文件 Makefile |
| 32 | 设备网关 + 日志模块 | 08-01 | `logger` 模块集成、`fprintf` + `fflush`、时间戳日志、`logs/` 目录 |
| 33 | 设备网关 + 配置文件 | 08-02 | `config/gateway.conf`、key=value 解析、`GatewayConfig` 结构体 |
| 34 | 设备网关 + 优雅退出 | 08-03 | `sigaction`、`SIGINT`/`SIGTERM`、`g_running` 标志位、资源清理 |
| 35 | 设备网关 + 设备状态模块 | 08-04 | `device_state` 模块、`DeviceState` 结构体、command/state 关注点分离 |
| 36 | 设备网关 + LED 状态管理 | 08-04 | `led on`/`led off` 状态持久化、`status` 查询实时 LED 状态 |
| 37 | 设备网关 + 动态采样 | 08-04 | `device_state_update_sample()`、模拟温度/电压、ADC/I2C 接口预留 |
| 38 | 统一响应格式与错误码 | 08-05 | `OK code=0 msg=...` / `ERR code=1001 msg=...`、数字错误码 |
| 39 | 项目整理与简历版收尾 | 08-05 | 模块职责文档化、README、构建运行说明、嵌入式迁移指南 |
| 40 | 开发板基础环境准备 | 08-05 | 串口登录、USB 网络、SSH 登录、`scp` 文件传输、野火 EBF6ULL S1 Pro |
| 41 | 交叉编译入门 | 08-05 | `arm-linux-gnueabihf-gcc`、`-static` vs `-dynamic`、`file` 命令、`scp` 部署 |
| 42 | 设备网关上板运行 | 08-06 | 交叉编译设备网关、开发板运行 server、PC 连接 client、`EINTR` 信号处理 |
| 43 | sysfs LED 控制 | 08-07 | `/sys/class/leds/*/brightness`、`open`/`write`/`close`、真实 LED 亮灭 |
| 44 | 设备网关 + LED 硬件 | 08-07 | `led_control` 模块、网关集成真实 LED、`ledctl` 命令行工具、硬件控制闭环 |
| 45 | 按键输入：读取 input event 控制 LED | 08-07 | `/dev/input/event1`、`struct input_event`、`EV_KEY`/`KEY_0`、按键驱动 LED |
| 46 | 按键状态接入设备网关 | 08-10 | `pthread` 按键监听线程、共享 `DeviceState`、`key_input` 模块、`status` 返回 key=pressed/released |
| 47 | RS485 接口验证与板级排障 | 08-12 | `/dev/ttymxc1`、RS485-1、USB-RS485 模块、双向通信验证、板级排障流程 |
| 48 | 设备网关 systemd 服务部署 | 08-10 | `systemd` unit 文件、`systemctl` 服务管理、开机自启、`deploy/` 部署脚本 |
| 53 | 网关日志轮转与存储保护 | 08-11 | `logrotate`、规则文件（size/rotate/copytruncate/compress/delaycompress）、`logrotate -d`/`-f`、`/var/lib/logrotate/status` |
| 54 | 网关运行诊断接口（diag） | 08-11 | `diagnostics` 模块、`diag` 命令、version/pid/uptime_sec、`getpid`、`snprintf`、`static` 内部状态、`systemctl stop` 更新流程 |

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

## Week 4：多线程编程

**目标**：掌握 POSIX 线程（pthread）编程，理解线程同步机制，能编写线程安全的数据结构和生产者消费者模型。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 18 | `file_monitor_tool` | Week 2 综合项目：配置文件驱动、stat 监控文件变化、信号优雅退出 |
| 19 | `pthread_basic` | 多线程计数：`pthread_create`/`pthread_join`、`pthread_mutex_t` 保护共享变量 |
| 20 | `thread_queue` | 线程安全环形队列：`pthread_cond_t` 条件变量、生产者消费者模型 |

## Week 5：IPC 进程间通信

**目标**：掌握 Linux 进程间通信（IPC）基础，理解 pipe 父子进程通信和 FIFO 命名管道独立进程通信两种方式，为嵌入式设备网关的多进程数据传递打基础。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 21 | `ipc_basic` | `pipe_demo`（父子进程 pipe 通信）+ `fifo_reader`/`fifo_writer`（独立进程 FIFO 通信）
| 22 | `local_command_server` | 综合项目：FIFO IPC + server/client 架构 + `keep_fd` 技巧 + 信号优雅退出 + 日志 |

## Week 6：IPC 综合项目

**目标**：将 Week 5 的 pipe / FIFO 知识综合运用，构建一个 server/client 架构的本地命令服务器，同时巩固信号处理和日志模块。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 22 | `local_command_server` | FIFO 命令通道、server 长期运行（keep_fd 技巧）、client 命令行参数拼接、SIGINT/SIGTERM 优雅退出、server 日志记录 |

## Week 7：网络编程

**目标**：进入 Linux 网络编程，掌握 TCP socket 编程基础，理解 client/server 架构、TCP 字节流特性、多线程并发模型和 IO 多路复用，为后续嵌入式网络通信打基础。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 23 | `tcp_echo` | TCP echo server（`socket` → `bind` → `listen` → `accept` → `recv`/`send`）+ TCP client（`socket` → `connect` → `send` → `recv`）+ 连续处理多个 client + 理解 TCP 字节流不保留消息边界 |
| 24 | `multi_client_server` | 多客户端 TCP server：主线程 `accept` + `pthread_create` worker 线程 `recv`/`send` + `pthread_detach` 自动回收 + `malloc`/`free` 传参 |
| 25 | `select_server` | select IO 多路复用：`fd_set` 管理 server_fd + 多个 client_fd + `FD_ISSET` 事件分发 + 单线程处理所有客户端 + 对比三种 IO 模型 |
| 26 | `poll_server` | poll IO 多路复用：`pollfd` 数组管理 server_fd + 多个 client_fd + `events`/`revents` 事件分发 |
| 27 | `epoll_server` | epoll IO 多路复用：`epoll_ctl` 注册 server_fd/client_fd + `epoll_wait` 只返回就绪事件 + 单线程处理多客户端 |

## Week 8：应用协议 & 综合实战

**目标**：在 TCP 网络编程基础上，深入理解 TCP 字节流的粘包/拆包问题，设计并实现应用层协议（length + payload），最终将 epoll 高性能 IO 多路复用与应用协议结合，构建单线程多客户端命令服务器。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 28 | `tcp_protocol` | TCP 应用层协议：4 字节长度头 + payload，解决 TCP 字节流消息边界问题 |
| 29 | `request_response` | TCP 请求-响应协议：client 发送命令，server 解析并返回响应，使用 `send_exact`/`read_exact` 保证完整收发 |
| 30 | `epoll_protocol_server` | 终章综合项目：epoll 单线程 + length+payload 协议 + 请求-响应模型，构建高性能多客户端命令服务器 |
| 31 | `device_gateway_refactor` | 项目结构重构：拆分为 protocol / command / server / client 四模块，理解头文件声明与源文件实现分离、多文件 Makefile 链接 |

## Week 9：设备网关渐进式迭代

**目标**：在模块化设备网关基础上，以渐进式工程迭代的方式逐日添加生产级能力 —— 日志记录、配置文件驱动、信号优雅退出、设备状态管理、LED 状态持久化、动态采样、统一响应格式与错误码。每一天都是在前一天基础上的增量改进，模拟真实嵌入式项目的迭代开发流程。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 32 | `device_gateway_logger` | 新增 `logger` 模块：`logger_init`/`logger_info`/`logger_error`/`logger_close`、时间戳日志、`fprintf` + `fflush` 即时落盘 |
| 33 | `device_gateway_config` | 新增 `config` 模块：`config/gateway.conf` 配置文件、`fgets`+`sscanf` 解析 key=value、`GatewayConfig` 结构体 |
| 34 | `device_gateway_graceful_shutdown` | 新增信号处理：`sigaction` 注册 `SIGINT`/`SIGTERM`、`volatile sig_atomic_t g_running` 标志位、`EINTR` 中断 `epoll_wait`、资源清理 |
| 35 | `device_gateway_state` | 新增 `device_state` 模块：`DeviceState` 结构体、`device_state_get_status()`、command 层与 state 层关注点分离 |
| 36 | `device_gateway_led_state` | LED 状态持久化：`led on`/`led off` 命令真正修改 `DeviceState.led_on`、`status` 查询实时 LED 状态 |
| 37 | `device_gateway_dynamic_state` | 动态采样：`device_state_update_sample()` 模拟温度/电压变化、为 ADC/I2C/驱动节点读取预留接口 |
| 38 | `device_gateway_response_code` | 统一响应格式：`OK code=0 msg=...` / `ERR code=1001 msg=...`、数字错误码、机器可解析响应规范 |

## Week 10：项目整理 & 开发板上板

**目标**：将 Week 9 迭代完成的设备网关整理为可展示的项目成果，并完成从 PC/WSL 到真实 ARM 嵌入式 Linux 开发板的最小上板链路打通。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 39 | `device_gateway_final` | 项目整理：7 模块架构文档、README、命令与响应格式规范、构建与运行说明、嵌入式开发板迁移路线图 |
| 40 | `board_bringup` | 开发板上板：串口登录（CH340, 115200 8N1）、USB RNDIS 网络、SSH 登录、`scp` 文件传输、野火 EBF6ULL S1 Pro (i.MX6ULL) |

## Week 11：交叉编译 & 开发板上板实战

**目标**：打通"WSL 编译 → ARM 交叉编译 → 开发板运行"的完整嵌入式 Linux 开发闭环，将设备网关项目部署到 ARM 开发板并接入真实 LED 硬件控制。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 41 | `cross_compile` | 交叉编译入门：`arm-linux-gnueabihf-gcc` 工具链、`-static` vs 动态链接、`file` 命令验证架构、`scp` 部署到 i.MX6ULL |
| 42 | `gateway_on_board` | 设备网关交叉编译上板：将 Day 39 设备网关编译为 ARM 可执行文件，部署到开发板运行 server，PC 端 client 连接验证 |
| 43 | `led_control` | sysfs LED 控制：通过 `/sys/class/leds/<name>/brightness` 接口，C 程序 `open`/`write`/`close` 控制开发板真实 LED 亮灭 |
| 44 | `gateway_led_hardware` | 设备网关 + 真实 LED：新增 `led_control` 模块，将 TCP 网关的 `led on`/`led off` 命令接入真实硬件 LED，完成"网络命令 → 硬件动作"完整闭环 |
| 45 | `key_input` | 按键输入：读取 `/dev/input/event1` 的 `struct input_event`，识别 `KEY_0` 按下/松开，切换并控制 green LED 亮灭 |
| 46 | `gateway_key_status` | 按键状态接入设备网关：key_input 线程阻塞读取按键事件，更新共享 DeviceState，status 命令返回 key=pressed/released |
| 47 | RS485 bring-up | RS485-1 接口验证：设备树确认、跳帽与收发器供电检查、USB-RS485 模块连接、`/dev/ttymxc1` 双向通信测试、板级排障流程文档化 |
| 48 | `gateway_systemd_service` | systemd 服务部署：设备网关注册为系统服务，`systemctl start/stop/status/enable` 管理生命周期，`Restart=on-failure` 自动重启 |

---

## Week 12：运维与日志管理

**目标**：在服务已由 systemd 托管的基础上，引入 Linux 标准日志轮转工具 logrotate，解决嵌入式设备长期运行时日志文件无限增长导致存储耗尽的问题。

| 天次 | 项目 | 核心产出 |
|:---:|------|------|
| 53 | `gateway_logrotate` | logrotate 日志轮转：规则文件编写（size/rotate/copytruncate/compress/delaycompress）、`logrotate -d` 模拟测试、`logrotate -f` 强制轮转验证、`/var/lib/logrotate/status` 状态确认 |
| 54 | `gateway_diagnostics` | 运行诊断接口：`diagnostics` 模块、`diag` 命令、version/pid/uptime_sec、`snprintf` 安全格式化、`getpid` 进程 PID、`static` 模块内部状态 |

---

## 环境要求

| 工具 | 用途 | 最低版本 |
|------|------|------|
| **gcc** | C 编译器 | 任意（推荐 9+） |
| **GNU Make** | 构建自动化 | 3.81+ |
| **CMake** | 跨平台构建（Day 4-5） | 3.16+ |
| **GDB** | 调试器（Day 5） | 任意 |
| **Linux / WSL** | 运行环境 | Ubuntu 20.04+ (WSL2) |

**平台说明**：本项目在 Windows 11 + WSL (Ubuntu) 环境下开发。所有代码使用 POSIX API，在原生 Linux 或 WSL 中均可编译运行。MinGW 可编译部分早期项目，但涉及 `fork`、`sigaction`、`pthread` 等系统调用的项目需要真实 Linux 内核。

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

# === Week 3 ===
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

# === Week 4 ===
# Day 18 — 文件监控工具（Week 2 综合项目）
cd linux_projects/day18_file_monitor_tool
make && ./build/file_monitor_tool
# 在另一个终端追加测试数据：make testdata
# 查看日志：cat logs/monitor.log

# Day 19 — pthread 线程基础
cd linux_projects/day19_pthread_basic
make && ./build/pthread_basic

# Day 20 — 线程安全生产者消费者队列
cd linux_projects/day20_thread_queue
make && ./build/thread_queue

# === Week 5 ===
# Day 21 — IPC 基础：pipe 父子进程通信
cd linux_projects/day21_ipc_basic
make && make runp

# Day 21 — FIFO 独立进程通信（需要两个终端）
# 终端 1：先启动 reader（会阻塞等待 writer）
make runr
# 终端 2：再启动 writer 发送设备数据
make runw

# === Week 6 ===
# Day 22 — 本地命令服务器（FIFO + 信号 + 日志综合项目）
cd linux_projects/day22_local_command_server
make
# 终端 1：启动 server（长期运行，Ctrl+C 优雅退出）
make run
# 终端 2：发送命令
make send1      # 发送 "status"
make send2      # 发送 "set led on"
make send3      # 发送 "reboot device"
# 或直接运行 client：
./build/client status
./build/client set led on
./build/client reboot device
# 查看日志：cat logs/server.log
```

```bash
# === Week 7 ===
# Day 23 — TCP echo server/client（socket 网络编程）
cd linux_projects/day23_tcp_echo
make
# 终端 1：启动 server（监听 0.0.0.0:9000）
make run1
# 终端 2：发送消息
make run2      # 发送 "hello tcp"
make run3      # 发送 "status"
make run4      # 发送 "set led on"
# 或直接运行 client：
./build/tcp_client hello tcp
./build/tcp_client status
./build/tcp_client set led on

# Day 24 — 多客户端 TCP server（pthread 每连接一线程）
cd linux_projects/day24_multi_client_server
make
# 终端 1：启动 server（监听 0.0.0.0:9000，每个 client 独立线程）
make run1
# 终端 2：启动多个 client
make run2      # client #1 发送并接收
# 终端 3：
make run3      # client #2 并发连接
# 或直接运行：
./build/tcp_client hello from client1
./build/tcp_client status

# Day 25 — select IO 多路复用（单线程管理多客户端）
cd linux_projects/day25_select_server
make
# 终端 1：启动 select server（监听 0.0.0.0:9000，select 多路复用）
make run1
# 终端 2：启动多个 client
make run2      # client #1
# 终端 3：
make run3      # client #2 并发连接
# 或直接运行：
./build/tcp_client hello select
./build/tcp_client status

# Day 26 — poll IO 多路复用（单线程管理多客户端）
cd linux_projects/day26_poll_server
make
# 终端 1：启动 poll server
make runse
# 终端 2 / 终端 3：启动多个交互式 client
make runc

# Day 27 — epoll IO 多路复用（Linux 高效事件通知）
cd linux_projects/day27_epoll_server
make
# 终端 1：启动 epoll server
make runse
# 终端 2 / 终端 3：启动多个交互式 client
make runc

# === Week 8 ===
# Day 28 — TCP 应用层协议（长度头 + payload）
cd linux_projects/day28_tcp_protocol
make
# 终端 1：启动协议 server
make runse
# 终端 2：启动协议 client
make runc

# Day 29 — TCP 请求-响应协议（命令解析与响应）
cd linux_projects/day29_request_response
make
# 终端 1：启动 request-response server
make run1
# 终端 2：启动 request-response client
make run2

# Day 30 — epoll + 应用协议单线程命令服务器（终章）
cd linux_projects/day30_epoll_protocol_server
make
# 终端 1：启动 epoll 多客户端协议 server
make runse
# 终端 2 / 终端 3 / 终端 4：启动多个交互式 client
make runc
# 或直接运行：
./build/client hello from client
./build/client status
./build/client set led on

# Day 31 — 设备网关项目结构重构（模块化拆分）
cd linux_projects/day31_device_gateway_refactor
make
# 终端 1：启动重构后的多模块 server
make runse
# 终端 2：启动重构后的多模块 client
make runc
# 或直接运行：
./build/client status
./build/client led on
./build/client reboot
```

```bash
# === Week 9 ===
# Day 32 — 设备网关 + 日志模块（logger 集成 + 文件写入）
cd linux_projects/day32_device_gateway_logger
make
# 终端 1：启动 server（事件写入 logs/server.log）
make runse
# 终端 2：发送命令
./build/client status
./build/client led on
# 查看日志：cat logs/server.log

# Day 33 — 设备网关 + 配置文件（端口/设备名/日志路径可配置）
cd linux_projects/day33_device_gateway_config
make
# 编辑 config/gateway.conf 修改端口、设备名、日志路径
make runse     # server 读取配置启动
make runc      # client 读取配置连接

# Day 34 — 设备网关 + 优雅退出（Ctrl+C 安全关闭）
cd linux_projects/day34_device_gateway_graceful_shutdown
make
make runse     # server 启动，按 Ctrl+C 触发优雅退出流程
# 观察日志中 "server stopped" 与资源清理记录

# Day 35 — 设备网关 + 设备状态模块（command/state 分离）
cd linux_projects/day35_device_gateway_state
make
make runse     # status 命令现在由 device_state 模块生成响应
./build/client status

# Day 36 — 设备网关 + LED 状态持久化
cd linux_projects/day36_device_gateway_led_state
make
make runse
./build/client led on     # LED 状态写入 DeviceState
./build/client status      # status 读取到 led=on
./build/client led off
./build/client status      # status 读取到 led=off

# Day 37 — 设备网关 + 动态采样（温度/电压模拟变化）
cd linux_projects/day37_device_gateway_dynamic_state
make
make runse
./build/client status      # temp=32 voltage=24
./build/client status      # temp=33 voltage=25（每次查询动态更新）

# Day 38 — 设备网关 + 统一响应格式与错误码
cd linux_projects/day38_device_gateway_response_code
make
make runse
./build/client status      # OK code=0 msg=status device=... temp=... voltage=...
./build/client unknown_cmd  # ERR code=1001 msg=unknown_command

# === Week 10 ===
# Day 39 — 设备网关最终版（整理文档化 + 简历版收尾）
cd linux_projects/day39_device_gateway_final
make
make runse                 # 最终版 server：7 模块完整架构
./build/client status
./build/client led on
./build/client reboot
# 查看项目 README：cat README.md

# Day 40 — 开发板基础环境准备（文档）
cd linux_projects/day40_board_bringup
cat board_info.md          # 野火 EBF6ULL S1 Pro 上板记录
cat hello_board.txt        # 通过 scp 传输到开发板的第一个文件

# === Week 11 ===
# Day 41 — 交叉编译入门（ARM 工具链 + scp 部署）
cd linux_projects/day41_cross_compile
make                        # 编译 x86 版本 (hello_x86) + ARM 版本 (hello_arm)
file hello_x86              # 验证 x86-64 架构
file hello_arm              # 验证 ARM 架构
make deploy                 # scp 上传 hello_arm 到开发板
# 在开发板上：./hello_arm

# Day 42 — 设备网关交叉编译上板
cd linux_projects/day42_gateway_on_board
make                        # 交叉编译 server + client 为 ARM 可执行文件
make deploy                 # scp 上传到开发板 /home/debian/apps/day42/
# 开发板终端：./server
# PC 终端：./client status / led on / reboot

# Day 43 — sysfs LED 控制（控制开发板真实 LED）
cd linux_projects/day43_led_control
make                        # 交叉编译 ledctl
make deploy                 # scp 上传到开发板
# 开发板终端：
./ledctl on                 # 点亮 LED
./ledctl off                # 熄灭 LED
./ledctl status             # 读取 LED 当前状态

# Day 44 — 设备网关 + 真实 LED 硬件控制
cd linux_projects/day44_gateway_led_hardware
make                        # 编译 server + client（含 led_control 模块）
# 开发板终端：./server（监听 TCP，led 命令控制真实硬件）
# PC 终端：./client led on / led off / status
# led on → 开发板真实 LED 亮起
# led off → 开发板真实 LED 熄灭

# Day 45 — 按键输入：读取 input event 控制 LED
cd linux_projects/day45_key_input
make                        # 交叉编译 key_monitor
make deploy                 # scp 上传到开发板
# 开发板终端：
./key_monitor               # 监听 /dev/input/event1，按下 KEY_0 切换 green LED
# 按下 KEY_0 → green LED 亮
# 再按 KEY_0 → green LED 灭

# Day 46 — 按键状态接入设备网关（pthread + 共享 DeviceState）
cd linux_projects/day46_gateway_key_status
make                        # 编译 server + client（含 key_input 模块，-pthread）
# 开发板终端：./server（启动 TCP server + key_input 监听线程）
# PC 终端：./client status（查询 key=pressed/released）
# 按下开发板 KEY_0 → status 返回 key=pressed
# 松开开发板 KEY_0 → status 返回 key=released

# Day 47 — RS485 接口验证与板级排障（文档）
cd notes
cat day47.md                # RS485-1 硬件 bring-up 详细记录
# 关键步骤：设备树确认 → 跳帽检查 → USB-RS485 连接 → /dev/ttymxc1 双向通信测试

# Day 48 — 设备网关部署为 systemd 服务
cd linux_projects/day48_gateway_systemd_service
make                        # 交叉编译 server + client
# 部署到开发板：
#   scp build/server build/client debian@192.168.7.2:/home/debian/apps/day48_gateway_systemd_service/
#   scp deploy/demo-gateway.service → /etc/systemd/system/
#   systemctl daemon-reload && systemctl start demo-gateway && systemctl enable demo-gateway
# 开发板操作：
systemctl status demo-gateway   # 查看服务运行状态
systemctl stop demo-gateway     # 停止服务
systemctl restart demo-gateway  # 重启服务
# 开机自启验证：systemctl is-enabled demo-gateway → enabled
```

```bash
# Day 53 — 网关日志轮转（logrotate）
cd linux_projects/day53_gateway_logrotate
# 部署到开发板：
#   scp deploy/demo-gateway-logrotate debian@192.168.7.2:/home/debian/
# 开发板 root 终端：
#   cp /home/debian/demo-gateway-logrotate /etc/logrotate.d/demo-gateway
#   chmod 644 /etc/logrotate.d/demo-gateway
# 模拟测试：
logrotate -d /etc/logrotate.d/demo-gateway    # debug 模式，不真正修改
# 强制轮转测试：
logrotate -f /etc/logrotate.d/demo-gateway    # force 强制执行一次轮转
# 查看轮转结果：
ls -lh /home/debian/apps/day48_gateway_systemd_service/logs
# 查看 logrotate 状态：
grep server.log -n /var/lib/logrotate/status
```

```bash
# Day 54 — 网关运行诊断接口（diag 命令）
cd linux_projects/day54_gateway_diagnostics
make                        # 编译 server + client（含 diagnostics 模块）
# 部署到开发板：
#   systemctl stop demo-gateway
#   scp build/server build/client debian@192.168.7.2:/home/debian/apps/day54_gateway_diagnostics/
#   scp deploy/demo-gateway.service → /etc/systemd/system/
#   systemctl daemon-reload && systemctl start demo-gateway
# 测试 diag 命令：
./build/client diag          # 返回 version/pid/uptime_sec
./build/client status
# 验证 PID 一致：
systemctl status demo-gateway   # Main PID 与 client diag 返回的 pid 一致
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
- **编译选项**：`-Wall -g -O0`（全警告 + 调试符号 + 无优化）；涉及 pthread 的项目加 `-pthread`
- **头文件保护**：所有 `.h` 使用 `#ifndef` / `#define` / `#endif` 守卫
- **错误处理**：系统调用返回值检查 + `perror()` 输出

---

## 相关文档

- **[Linux_Embedded_App_Summer_Plan.md](./Linux_Embedded_App_Summer_Plan.md)** — 暑期学习总体计划
- **[Qt_Linux_HMI_Plan_From_Day4.md](./Qt_Linux_HMI_Plan_From_Day4.md)** — Qt / Linux HMI 专项路线图

---

<p align="center">
  <sub>从编译选项到 epoll 高性能服务器 → 设备网关渐进式迭代 → 开发板上板 → 交叉编译 → 接入真实 LED 硬件与按键输入 → systemd 服务部署 → logrotate 日志轮转，学习计划持续进行中 🚀</sub>
</p>
