# Linux 嵌入式学习笔记与项目代码

> 从 Linux C 基础出发，逐步学习构建系统、系统调用、并发、网络编程、嵌入式网关、ARM 交叉编译和开发板硬件控制。

当前学习进度：**Day 44**

最后整理：**2026-08-07**

## 目录

- [项目概览](#项目概览)
- [仓库结构](#仓库结构)
- [快速开始](#快速开始)
- [学习路线](#学习路线)
- [Day 43：通过 sysfs 控制开发板 LED](#day-43通过-sysfs-控制开发板-led)
- [Day 44：设备网关与 LED 硬件扩展](#day-44设备网关与-led-硬件扩展)
- [开发环境](#开发环境)
- [项目规范](#项目规范)
- [后续计划](#后续计划)

## 项目概览

这是一个以实践为主的 Linux 嵌入式学习仓库，记录从第一个 Linux C 程序到设备网关和开发板硬件控制的完整过程。

| 内容 | 位置 | 当前状态 |
| --- | --- | --- |
| 每日学习笔记 | [notes/](./notes) | Day 01–43 |
| Linux C 项目 | [linux_projects/](./linux_projects) | Day 01–31、35–44，共 41 个项目目录 |
| Qt 并行学习 | [qt_projects/](./qt_projects) | 已建立 Qt 学习目录 |
| 主线方向 | Linux C、POSIX、TCP、epoll、交叉编译、i.MX6ULL | 持续迭代 |

学习路径大致为：

Linux C 基础 → gcc / Makefile / CMake → GDB 调试 → 文件与进程 → 信号与非阻塞 IO → 多线程与 IPC → TCP 网络编程 → epoll 与应用层协议 → 设备网关模块化 → 配置、日志、状态和 LED → i.MX6ULL 上板 → ARM 交叉编译 → sysfs 硬件控制。

说明：

- Day 32–34 目前主要保留在 notes/ 中，没有对应的独立项目目录。
- Day 43 的 ledctl 已实现通过 Linux LED 子系统控制开发板上的真实 LED。
- Day 44 的网关代码目前仍维护软件 LED 状态；真实 sysfs 控制仍是下一步集成内容，不能把 Day 44 误写成已经完成 GPIO 接入。

## 仓库结构

~~~text
linux-embedded-learning/
├── README.md
├── .gitignore
├── notes/                                  # 每日学习笔记
│   ├── day01.md
│   ├── ...
│   └── day43.md
├── linux_projects/                         # Linux C 练习和综合项目
│   ├── day01_hello_linux/
│   ├── day02_compile_flow/
│   ├── ...
│   ├── day31_device_gateway_refactor/
│   ├── day35_device_gateway_state/
│   ├── day36_device_gateway_led_state/
│   ├── day37_device_gateway_dynamic_state/
│   ├── day38_device_gateway_response_code/
│   ├── day39_device_gateway_final/
│   ├── day40_board_bringup/
│   ├── day41_cross_compile/
│   ├── day42_gateway_on_board/
│   ├── day43_led_control/
│   └── day44_gateway_led_hardware/
└── qt_projects/                            # Qt / 嵌入式 HMI 并行轨道
    └── qt_day04_first_app/
~~~

大多数多文件项目使用以下结构：

~~~text
project/
├── include/                                # 头文件
├── src/                                    # C 源文件
├── config/                                 # 配置文件（如果需要）
├── data/                                   # 测试数据（如果需要）
├── build/                                  # 构建产物
├── Makefile 或 CMakeLists.txt
└── README.md
~~~

## 快速开始

### 1. 获取仓库

~~~bash
git clone https://github.com/alanniko1265-ai/linux-embedded-learning.git
cd linux-embedded-learning
~~~

### 2. 构建普通 Makefile 项目

~~~bash
cd linux_projects/day10_file_stat
make
./build/file_probe /home
make clean
~~~

每个项目的目标名称可能不同，运行前请先查看对应目录中的 Makefile 或 README.md。

### 3. 构建 CMake 项目

~~~bash
cd linux_projects/day04_cmake_basic
cmake -S . -B build
cmake --build build
~~~

### 4. 运行网关项目

Day 22、Day 23–30 以及 Day 39、Day 42、Day 44 都包含需要多个终端配合的服务端/客户端实验。以 Day 44 为例：

~~~bash
cd linux_projects/day44_gateway_led_hardware
make
~~~

在支持对应架构的 Linux 环境中分别启动服务端和客户端：

~~~bash
# 终端 1
make run1

# 终端 2
make run2
~~~

客户端可输入：

~~~text
status
led on
led off
reboot
quit
~~~

Day 41、Day 43 和 Day 44 的 Makefile 使用 ARM 交叉编译器时，生成的程序不能直接在 x86_64 WSL 中运行；需要上传到匹配的开发板，或将编译器改为本机 gcc 进行主机端逻辑测试。

## 学习路线

| Day | 学习主题 | 项目 | 笔记 |
| ---: | --- | --- | --- |
| 01 | 环境确认与第一个 Linux C 程序 | [day01_hello_linux](./linux_projects/day01_hello_linux) | [day01.md](./notes/day01.md) |
| 02 | gcc/g++ 编译流程：预处理、汇编、链接 | [day02_compile_flow](./linux_projects/day02_compile_flow) | [day02.md](./notes/day02.md) |
| 03 | Makefile：变量、依赖、自动变量和伪目标 | [day03_makefile_basic](./linux_projects/day03_makefile_basic) | [day03.md](./notes/day03.md) |
| 04 | CMake 基础与 out-of-source build | [day04_cmake_basic](./linux_projects/day04_cmake_basic) | [day04.md](./notes/day04.md) |
| 05 | GDB：断点、单步、变量和内存调试 | [day05_gdb_debug](./linux_projects/day05_gdb_debug) | [day05.md](./notes/day05.md) |
| 06 | 静态库与动态库：ar、ldd、运行时库搜索 | [day06_static_shared_lib](./linux_projects/day06_static_shared_lib) | [day06.md](./notes/day06.md) |
| 07 | Linux 工具箱：多文件 C CLI 项目 | [day07_linux_toolbox_v1](./linux_projects/day07_linux_toolbox_v1) | [day07.md](./notes/day07.md) |
| 08 | 文件 IO：open、read、write、close | [day08_file_io](./linux_projects/day08_file_io) | [day08.md](./notes/day08.md) |
| 09 | 安全文件复制与短写入处理 | [day09_file_io_utils](./linux_projects/day09_file_io_utils) | [day09.md](./notes/day09.md) |
| 10 | 文件属性：stat、权限位和类型识别 | [day10_file_stat](./linux_projects/day10_file_stat) | [day10.md](./notes/day10.md) |
| 11 | 目录遍历：opendir、readdir、stat | [day11_dir_scan](./linux_projects/day11_dir_scan) | [day11.md](./notes/day11.md) |
| 12 | 进程管理：fork、execvp、waitpid | [day12_process_runner](./linux_projects/day12_process_runner) | [day12.md](./notes/day12.md) |
| 13 | 信号处理与优雅退出 | [day13_signal_guard](./linux_projects/day13_signal_guard) | [day13.md](./notes/day13.md) |
| 14 | 时间戳、分级日志和 logger 模块 | [day14_logger_module](./linux_projects/day14_logger_module) | [day14.md](./notes/day14.md) |
| 15 | fcntl、O_NONBLOCK 和 EAGAIN | [day15_nonblock_io](./linux_projects/day15_nonblock_io) | [day15.md](./notes/day15.md) |
| 16 | /proc、/sys、/dev 与虚拟文件系统 | [day16_proc_sys_dev](./linux_projects/day16_proc_sys_dev) | [day16.md](./notes/day16.md) |
| 17 | ioctl 终端查询与 mmap 文件映射 | [day17_ioctl_mmap_intro](./linux_projects/day17_ioctl_mmap_intro) | [day17.md](./notes/day17.md) |
| 18 | 文件监控综合项目 | [day18_file_monitor_tool](./linux_projects/day18_file_monitor_tool) | [day18.md](./notes/day18.md) |
| 19 | pthread 线程创建、join 和互斥锁 | [day19_pthread_basic](./linux_projects/day19_pthread_basic) | [day19.md](./notes/day19.md) |
| 20 | 条件变量与生产者消费者队列 | [day20_thread_queue](./linux_projects/day20_thread_queue) | [day20.md](./notes/day20.md) |
| 21 | pipe 父子进程通信与 FIFO | [day21_ipc_basic](./linux_projects/day21_ipc_basic) | [day21.md](./notes/day21.md) |
| 22 | FIFO 本地命令服务器、信号和日志 | [day22_local_command_server](./linux_projects/day22_local_command_server) | [day22.md](./notes/day22.md) |
| 23 | TCP echo server/client 基础 | [day23_tcp_echo](./linux_projects/day23_tcp_echo) | [day23.md](./notes/day23.md) |
| 24 | pthread 多客户端 TCP server | [day24_multi_client_server](./linux_projects/day24_multi_client_server) | [day24.md](./notes/day24.md) |
| 25 | select IO 多路复用 | [day25_select_server](./linux_projects/day25_select_server) | [day25.md](./notes/day25.md) |
| 26 | poll IO 多路复用 | [day26_poll_server](./linux_projects/day26_poll_server) | [day26.md](./notes/day26.md) |
| 27 | epoll IO 多路复用 | [day27_epoll_server](./linux_projects/day27_epoll_server) | [day27.md](./notes/day27.md) |
| 28 | TCP 粘包/拆包与 length + payload 协议 | [day28_tcp_protocol](./linux_projects/day28_tcp_protocol) | [day28.md](./notes/day28.md) |
| 29 | TCP 请求-响应协议与完整收发 | [day29_request_response](./linux_projects/day29_request_response) | [day29.md](./notes/day29.md) |
| 30 | epoll 多客户端请求-响应服务器 | [day30_epoll_protocol_server](./linux_projects/day30_epoll_protocol_server) | [day30.md](./notes/day30.md) |
| 31 | 设备网关结构重构与模块化拆分 | [day31_device_gateway_refactor](./linux_projects/day31_device_gateway_refactor) | [day31.md](./notes/day31.md) |
| 32 | 网关 logger 模块集成 | —（以笔记为主） | [day32.md](./notes/day32.md) |
| 33 | 网关配置文件与 config 模块 | —（以笔记为主） | [day33.md](./notes/day33.md) |
| 34 | 信号、EINTR 与资源清理 | —（以笔记为主） | [day34.md](./notes/day34.md) |
| 35 | DeviceState 设备状态模块 | [day35_device_gateway_state](./linux_projects/day35_device_gateway_state) | [day35.md](./notes/day35.md) |
| 36 | LED 软件状态管理 | [day36_device_gateway_led_state](./linux_projects/day36_device_gateway_led_state) | [day36.md](./notes/day36.md) |
| 37 | 温度、电压动态采样 | [day37_device_gateway_dynamic_state](./linux_projects/day37_device_gateway_dynamic_state) | [day37.md](./notes/day37.md) |
| 38 | 统一响应格式与错误码 | [day38_device_gateway_response_code](./linux_projects/day38_device_gateway_response_code) | [day38.md](./notes/day38.md) |
| 39 | 可展示的 PC 版设备网关收尾 | [day39_device_gateway_final](./linux_projects/day39_device_gateway_final) | [day39.md](./notes/day39.md) |
| 40 | i.MX6ULL 串口、网络和文件传输准备 | [day40_board_bringup](./linux_projects/day40_board_bringup) | [day40.md](./notes/day40.md) |
| 41 | ARM 交叉编译与开发板运行 | [day41_cross_compile](./linux_projects/day41_cross_compile) | [day41.md](./notes/day41.md) |
| 42 | 设备网关项目上板运行 | [day42_gateway_on_board](./linux_projects/day42_gateway_on_board) | [day42.md](./notes/day42.md) |
| 43 | sysfs LED 控制与交叉编译工具链适配 | [day43_led_control](./linux_projects/day43_led_control) | [day43.md](./notes/day43.md) |
| 44 | 设备网关 LED 硬件扩展准备 | [day44_gateway_led_hardware](./linux_projects/day44_gateway_led_hardware) | [day44 README](./linux_projects/day44_gateway_led_hardware/README.md) |

## Day 43：通过 sysfs 控制开发板 LED

Day 43 不编写 LED 驱动，而是在应用层使用 Linux LED 子系统已经暴露的 sysfs 接口：

~~~text
C 程序
  -> open / write / close
  -> /sys/class/leds/<led_name>/brightness
  -> Linux LED 子系统
  -> 内核驱动
  -> GPIO
  -> 开发板 LED
~~~

当前程序支持 red、green、blue 三个普通 LED：

~~~bash
cd linux_projects/day43_led_control
make
~~~

在开发板上运行：

~~~bash
./ledctl red on
./ledctl red off
./ledctl green on
./ledctl green off
./ledctl blue on
./ledctl blue off
~~~

也可以在开发板上直接验证 sysfs 节点：

~~~bash
echo 1 > /sys/class/leds/red/brightness
echo 0 > /sys/class/leds/red/brightness
cat /sys/class/leds/red/trigger
~~~

开发板上的 LED 名称和 trigger 由设备树及系统配置决定，运行前应先检查：

~~~bash
ls -l /sys/class/leds
cat /sys/class/leds/red/trigger
~~~

Day 43 笔记还记录了一个重要的工具链兼容性问题：使用 Ubuntu 22.04 自带的较新 ARM 编译器生成的程序，在 Debian 10 开发板上可能依赖 GLIBC_2.34；当前验证可用的是 Arm GNU Toolchain 8.3，对应工具链路径已写入 Day 43 的 Makefile。实际部署时应以目标板的 glibc 版本为准。

## Day 44：设备网关与 LED 硬件扩展

Day 44 项目保留了完整的设备网关架构：

- TCP server / client
- epoll 多客户端事件处理
- 4 字节长度字段加 payload 的应用层协议
- status、led on、led off、reboot 命令
- 配置文件加载
- 服务端日志
- SIGINT / SIGTERM 信号处理
- 设备状态和动态温度、电压采样
- 统一的 OK / ERR 响应格式

项目结构：

~~~text
day44_gateway_led_hardware/
├── include/
├── src/
├── config/gateway.conf
├── Makefile
└── README.md
~~~

当前 Day 44 的 device_state.c 仍然只修改内存中的 led_on 状态，不会直接写入 /sys/class/leds/.../brightness。目录中的 led_control.h 和 led_control.c 目前只是未完成的接口草稿，尚未被 server 构建目标调用。因此真实硬件接入的推荐演进路径是：

~~~text
客户端 led on/off 命令
        |
        v
command.c 解析命令
        |
        v
device_state.c 更新状态
        |
        v
sysfs / GPIO / 驱动节点
        |
        v
开发板真实 LED
~~~

Day 43 的 ledctl 已经验证了 sysfs 控制链路，下一步可以把这部分逻辑封装到网关的设备状态模块中，并继续保留错误处理、日志记录和统一响应。

## 开发环境

### 主机端

- Linux 或 WSL2（Ubuntu 20.04+）
- gcc / g++
- GNU Make
- CMake
- GDB
- Git

### 开发板端

- i.MX6ULL 开发板
- Debian Linux
- 串口工具，例如 PuTTY 或 minicom
- SSH / scp 文件传输
- 与开发板系统兼容的 ARM 交叉编译器

涉及 fork、sigaction、pthread、epoll 和 sysfs 的项目需要真实 Linux 环境；Windows 原生环境不适合作为这些项目的运行环境。

## 项目规范

- 多文件项目优先采用 include/、src/、build/ 分层。
- Makefile 通常提供 all、clean，综合项目再提供 run、run1、run2 或 deploy。
- 编译阶段保持 -Wall -g -O0，便于发现问题和使用 GDB 调试。
- 使用系统调用时检查返回值，并在适当位置使用 perror()。
- 头文件使用 include guard。
- TCP 项目不能假设一次 send() 对应一次 recv()，应通过完整读写函数处理短读、短写和半包。
- build/、日志和本地生成的可执行文件不应作为源代码提交；提交前请检查 git status。

## 后续计划

- 将 Day 44 的软件 LED 状态接入 Day 43 已验证的 sysfs 控制逻辑。
- 在设备网关中加入真实传感器或 /sys、I2C 数据读取。
- 完善 ARM 交叉编译和开发板部署脚本。
- 使用 systemd 或启动脚本管理网关服务。
- 继续完善 Qt 上位机，通过 TCP 连接设备网关并解析响应。

---

本仓库用于记录学习过程和实验代码。每个 Day 的详细目标、命令、踩坑与总结以 notes/ 中对应笔记为准；项目的具体编译参数和运行方式以项目目录内的 Makefile、CMakeLists.txt 和 README.md 为准。
