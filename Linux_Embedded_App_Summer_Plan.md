# Linux 嵌入式应用开发暑假学习计划

起始日期：2026-07-08

目标路线：

`Linux 系统编程 -> 嵌入式 Linux 应用 -> 交叉编译/部署 -> Buildroot/rootfs -> BSP/驱动预备`

最终目标：

1. 完成一个 `EmbeddedDeviceGateway` 项目。
2. 支持日志、配置、协议解析、TCP、串口、线程/epoll、优雅退出。
3. 能在 Linux PC 上运行。
4. 能交叉编译并部署到开发板或 QEMU/Buildroot 系统。
5. 为开学后的 BSP/驱动学习打好基础。

开发板使用节奏：

- Day 1 - Day 28：不需要开发板，使用 WSL/Ubuntu/虚拟机即可。
- Day 29 - Day 35：开始选择并购买开发板、USB-TTL、杜邦线、电源等。
- Day 36 - Day 42：最好有开发板，练交叉编译、部署、串口、systemd/init。
- Day 43 - Day 49：有开发板更好，也可以用 QEMU + Buildroot。
- Day 50 - Day 56：如果进入 GPIO/I2C/SPI/设备树/BSP，强烈建议有开发板。

推荐开发板：

- 首选：i.MX6ULL Linux 开发板，资料成熟，适合 BSP/驱动入门。
- 次选：STM32MP157，适合从 MCU 过渡到 Linux。
- 后续再考虑：RK3568/RK3588，实际项目常见但复杂度更高。

每天固定节奏：

- 30 分钟：学习概念和 API。
- 60 - 90 分钟：写代码。
- 20 分钟：写笔记、整理问题、提交 Git。

每周固定产出：

- 一份 `notes/weekN.md`
- 一个可运行 demo 或项目版本
- 一次 README 更新
- 一次 Git 提交

---

## Week 1：Linux C/C++ 开发环境和工具链

目标：能在 Linux 下独立编译、运行、调试 C/C++ 程序。

本周不需要开发板。

### Day 1：环境确认和第一个 Linux C 程序

日期：2026-07-08

学习内容：

- WSL/Ubuntu 环境确认
- `gcc`
- `g++`
- `make`
- `gdb`
- Linux 项目目录组织

编码任务：

- 创建目录 `linux_projects/day01_hello_linux`
- 编写 `hello_linux.c`
- 编译并运行
- 故意制造一个简单错误，观察编译报错

今日产出：

- `hello_linux.c`
- `README.md`
- `notes/day01.md`

验收标准：

- 能用命令行编译 C 程序。
- 能说清楚源文件、可执行文件、编译命令分别是什么。
- 能记录今天遇到的 3 个 Linux 命令。

### Day 2：gcc/g++ 编译流程

学习内容：

- 预处理
- 编译
- 汇编
- 链接
- `-Wall`
- `-g`
- `-O0`
- `-I`
- `-L`
- `-l`

编码任务：

- 写一个 `add.c/add.h/main.c`
- 分步骤生成 `.i`、`.s`、`.o`、可执行文件
- 用 `gcc -Wall -g` 编译

今日产出：

- `day02_compile_flow`
- 编译流程笔记

验收标准：

- 能说出 `.c -> .o -> 可执行文件` 的过程。
- 能解释头文件和源文件的关系。

### Day 3：Makefile 入门

学习内容：

- Makefile 目标
- 依赖
- 命令
- 变量
- `clean`
- 增量编译

编码任务：

- 给 Day 2 项目写 Makefile
- 支持 `make`
- 支持 `make clean`
- 故意修改一个源文件，观察增量编译

今日产出：

- `day03_makefile_basic`

验收标准：

- 能独立写出最小 Makefile。
- 能解释为什么 Makefile 适合多文件项目。

### Day 4：CMake 入门

学习内容：

- `CMakeLists.txt`
- `add_executable`
- `target_include_directories`
- out-of-source build

编码任务：

- 把 Day 2 项目改成 CMake 构建
- 创建 `build` 目录
- 使用 `cmake ..`
- 使用 `cmake --build .`

今日产出：

- `day04_cmake_basic`

验收标准：

- 能用 CMake 编译多文件 C/C++ 项目。
- 能说清楚 Makefile 和 CMake 的区别。

### Day 5：gdb 调试

学习内容：

- `gdb`
- breakpoint
- run
- next
- step
- print
- backtrace
- segmentation fault

编码任务：

- 写一个数组越界或空指针崩溃 demo
- 用 gdb 找到崩溃位置
- 修改 bug

今日产出：

- `day05_gdb_debug`
- 一份 gdb 常用命令表

验收标准：

- 能用 gdb 定位一个段错误。
- 能看懂 backtrace。

### Day 6：静态库和动态库

学习内容：

- `.a`
- `.so`
- `ar`
- `ldd`
- `LD_LIBRARY_PATH`
- 头文件和库文件分离

编码任务：

- 把日志函数封装成静态库
- 再封装成动态库
- 主程序分别链接两种库
- 用 `ldd` 查看动态依赖

今日产出：

- `day06_static_shared_lib`

验收标准：

- 能解释静态库和动态库的区别。
- 能解决一次动态库找不到的问题。

### Day 7：Week 1 复盘和小项目

任务：

- 做一个 `linux_toolbox_v1`
- 功能包括：
  - 打印程序版本
  - 打印当前时间
  - 写一条日志
  - 支持 Makefile 或 CMake 构建

今日产出：

- `linux_toolbox_v1`
- `notes/week01.md`

验收标准：

- 项目能一键编译。
- README 写清楚构建方式和运行方式。

---

## Week 2：Linux 文件 IO 和系统调用

目标：掌握 Linux 应用和内核交互的基本接口。

本周不需要开发板。

### Day 8：文件 IO 基础

学习内容：

- `open`
- `read`
- `write`
- `close`
- `lseek`
- 文件描述符

编码任务：

- 实现 `my_cat`
- 实现 `my_cp`
- 对比 C 标准库 `fopen/fread/fwrite` 和 Linux 系统调用

今日产出：

- `day08_file_io`

验收标准：

- 能用系统调用读写文件。
- 能解释文件描述符是什么。

### Day 9：错误处理和 errno

学习内容：

- `errno`
- `perror`
- `strerror`
- 返回值检查
- 错误码设计

编码任务：

- 给 Day 8 的工具补全错误处理
- 处理文件不存在、权限不足、参数错误

今日产出：

- `day09_errno_error_handling`

验收标准：

- 所有系统调用都检查返回值。
- 错误信息能帮助定位问题。

### Day 10：文件属性和目录操作

学习内容：

- `stat`
- `lstat`
- `opendir`
- `readdir`
- `closedir`
- 文件类型
- 文件权限

编码任务：

- 实现简化版 `ls -l`
- 输出文件名、大小、权限、修改时间

今日产出：

- `day10_my_ls`

验收标准：

- 能判断普通文件、目录、链接文件。
- 能解释 Linux 权限位。

### Day 11：fcntl 和非阻塞 IO

学习内容：

- `fcntl`
- `O_NONBLOCK`
- 阻塞和非阻塞
- 文件状态标志

编码任务：

- 写一个非阻塞读取 stdin 的 demo
- 观察没有输入时的返回值和 errno

今日产出：

- `day11_nonblock_io`

验收标准：

- 能解释阻塞 IO 和非阻塞 IO。
- 能正确处理 `EAGAIN`。

### Day 12：/proc、/sys、/dev

学习内容：

- `/proc`
- `/sys`
- `/dev`
- 设备文件
- procfs/sysfs/devtmpfs 基础概念

编码任务：

- 读取 `/proc/cpuinfo`
- 读取 `/proc/meminfo`
- 访问 `/dev/null`
- 访问 `/dev/zero`

今日产出：

- `day12_proc_sys_dev`

验收标准：

- 能说明 `/proc`、`/sys`、`/dev` 的作用。
- 能理解用户态程序如何通过设备文件接触内核。

### Day 13：ioctl 和 mmap 概念

学习内容：

- `ioctl`
- `mmap`
- 为什么驱动常用 ioctl
- 为什么视频、显示、DMA 场景常见 mmap

编码任务：

- 写一个查询终端窗口大小的 `ioctl` demo
- 写一个 `mmap` 读取文件 demo

今日产出：

- `day13_ioctl_mmap_intro`

验收标准：

- 能解释 `read/write/ioctl/mmap` 各自适合什么场景。

### Day 14：Week 2 小项目

任务：

- 完成 `file_monitor_tool`
- 功能包括：
  - 读取配置文件
  - 监控一个文件大小
  - 周期写日志
  - 支持错误处理

今日产出：

- `file_monitor_tool`
- `notes/week02.md`

验收标准：

- 程序能稳定运行 5 分钟。
- 日志内容清楚。
- README 写清楚用到了哪些系统调用。

---

## Week 3：进程、线程、IPC

目标：能写长期运行的 Linux 服务程序。

本周不需要开发板。

### Day 15：进程基础

学习内容：

- 进程 ID
- 父进程
- 子进程
- `fork`
- `getpid`
- `getppid`

编码任务：

- 写一个 fork demo
- 观察父子进程变量变化
- 用 `ps` 查看进程

今日产出：

- `day15_process_fork`

验收标准：

- 能解释 fork 后父子进程的关系。

### Day 16：exec 和 wait

学习内容：

- `exec`
- `wait`
- `waitpid`
- 退出状态
- 僵尸进程

编码任务：

- 父进程创建子进程
- 子进程执行 `ls`
- 父进程回收子进程

今日产出：

- `day16_exec_wait`

验收标准：

- 能避免僵尸进程。

### Day 17：signal 和优雅退出

学习内容：

- `SIGINT`
- `SIGTERM`
- `signal`
- `sigaction`
- 资源释放

编码任务：

- 写一个持续运行程序
- 捕获 Ctrl+C
- 退出前写日志并关闭文件

今日产出：

- `day17_signal_exit`

验收标准：

- 程序能响应退出信号。
- 退出时没有明显资源泄漏。

### Day 18：pthread 基础

学习内容：

- `pthread_create`
- `pthread_join`
- 线程参数
- 线程返回值

编码任务：

- 创建 2 个线程
- 一个线程写日志
- 一个线程更新状态

今日产出：

- `day18_pthread_basic`

验收标准：

- 能解释进程和线程的区别。

### Day 19：线程同步

学习内容：

- mutex
- condition variable
- 共享数据保护
- 生产者消费者模型

编码任务：

- 实现一个线程安全队列
- 一个线程生产数据
- 一个线程消费数据

今日产出：

- `day19_thread_queue`

验收标准：

- 没有数据竞争。
- 能解释为什么需要锁。

### Day 20：IPC 基础

学习内容：

- pipe
- FIFO
- shared memory
- Unix domain socket 概念

编码任务：

- pipe 父子进程通信
- FIFO 两个独立程序通信

今日产出：

- `day20_ipc_basic`

验收标准：

- 能说出几种 IPC 的适用场景。

### Day 21：Week 3 小项目

任务：

- 完成 `local_command_server`
- 功能包括：
  - 一个 server 进程长期运行
  - client 发送命令
  - server 记录日志
  - 支持优雅退出

今日产出：

- `local_command_server`
- `notes/week03.md`

验收标准：

- server/client 能稳定通信。
- 程序结构清楚。

---

## Week 4：网络编程和协议解析

目标：把 Qt 通信经验迁移到纯 Linux C/C++。

本周不需要开发板。

### Day 22：TCP client/server 基础

学习内容：

- socket
- bind
- listen
- accept
- connect
- send
- recv

编码任务：

- 写 TCP echo server
- 写 TCP client

今日产出：

- `day22_tcp_echo`

验收标准：

- client 发什么，server 回什么。

### Day 23：多客户端 TCP server

学习内容：

- 多进程模型
- 多线程模型
- client 连接管理

编码任务：

- 支持多个 client 同时连接
- 每个连接输出日志

今日产出：

- `day23_multi_client_server`

验收标准：

- 两个以上客户端能同时通信。

### Day 24：select/poll/epoll 入门

学习内容：

- IO 多路复用
- `select`
- `poll`
- `epoll`
- 事件驱动模型

编码任务：

- 用 `select` 改写 echo server
- 了解 `epoll` 版本结构

今日产出：

- `day24_io_multiplexing`

验收标准：

- 能解释为什么不能无限开线程。

### Day 25：自定义通信协议

学习内容：

- 帧头
- 长度
- 命令字
- payload
- checksum
- 粘包和拆包

编码任务：

- 设计协议：
  - magic: 2 bytes
  - length: 2 bytes
  - cmd: 1 byte
  - payload: N bytes
  - checksum: 1 byte
- 实现编码和解码函数

今日产出：

- `day25_protocol_codec`

验收标准：

- 能处理半包、粘包、错误校验。

### Day 26：设备网关 v1

学习内容：

- 模块拆分
- socket 模块
- protocol 模块
- log 模块
- config 模块

编码任务：

- 创建 `EmbeddedDeviceGateway`
- 实现 TCP server
- 接收协议命令
- 返回响应帧

今日产出：

- `EmbeddedDeviceGateway/v1`

验收标准：

- 能通过 client 发送命令并收到响应。

### Day 27：配置和日志完善

学习内容：

- 配置文件格式
- 日志等级
- 日志轮转概念

编码任务：

- 支持 `gateway.conf`
- 支持 `INFO/WARN/ERROR`
- 日志写入文件

今日产出：

- `EmbeddedDeviceGateway/v1.1`

验收标准：

- 修改配置后程序行为变化。
- 日志能定位问题。

### Day 28：Week 4 复盘

任务：

- 整理 `EmbeddedDeviceGateway` 第一阶段 README
- 写清楚：
  - 项目功能
  - 协议格式
  - 编译方式
  - 运行方式
  - 模块结构

今日产出：

- `notes/week04.md`
- `EmbeddedDeviceGateway/README.md`

验收标准：

- 这是你第一个 Linux 应用开发阶段作品。

---

## Week 5：串口、设备通信、嵌入式接口

目标：开始进入嵌入式 Linux 应用场景。

本周开始可以购买开发板，但前半周仍可用 PC 模拟。

### Day 29：开发板选型和串口基础

学习内容：

- Linux 开发板选型
- USB-TTL
- UART
- 串口设备名
- `/dev/ttyUSB0`
- `/dev/ttyS*`

任务：

- 确定是否购买开发板
- 如果购买，优先选 i.MX6ULL 或 STM32MP157
- 同时准备 USB-TTL、杜邦线、电源、网线/Type-C 数据线

编码任务：

- 写串口学习笔记
- 整理串口参数含义

今日产出：

- `notes/day29_board_selection.md`

验收标准：

- 明确自己买哪块板，为什么买。

### Day 30：termios 串口编程

学习内容：

- `termios`
- 波特率
- 数据位
- 停止位
- 校验位
- 原始模式

编码任务：

- 写 `serial_open`
- 写 `serial_config`
- 写 `serial_read/write`

今日产出：

- `day30_serial_termios`

验收标准：

- 串口模块接口清楚，即使暂时没有真设备也能编译。

### Day 31：虚拟串口模拟

学习内容：

- 伪终端
- 串口模拟
- 测试思路

编码任务：

- 用虚拟串口或两个进程模拟串口设备
- 一个进程发送模拟传感器数据
- 一个进程接收并解析

今日产出：

- `day31_virtual_serial_test`

验收标准：

- 没有开发板也能测试串口逻辑。

### Day 32：串口协议解析

学习内容：

- 串口流式数据
- 缓冲区
- 半包
- 错包
- 重同步

编码任务：

- 把 Week 4 协议解析器接到串口接收模块
- 串口收到数据后生成设备状态

今日产出：

- `EmbeddedDeviceGateway/v2_serial`

验收标准：

- 能从串口输入中解析出完整帧。

### Day 33：TCP 转串口网关

学习内容：

- 网关模型
- 上行数据
- 下行命令

编码任务：

- TCP client 下发命令
- gateway 转发到串口
- 串口收到模拟响应
- gateway 返回 TCP client

今日产出：

- `EmbeddedDeviceGateway/v2_gateway`

验收标准：

- 打通 TCP <-> gateway <-> serial 的链路。

### Day 34：GPIO/I2C/SPI 用户态认知

学习内容：

- GPIO chardev
- libgpiod 概念
- `/dev/i2c-*`
- `/dev/spidev*`
- 为什么这些接口和驱动有关

编码任务：

- 写用户态硬件接口笔记
- 暂不强求操作真实 GPIO/I2C/SPI

今日产出：

- `notes/day34_userspace_hardware_api.md`

验收标准：

- 能说清楚串口、GPIO、I2C、SPI 在用户态怎么接触。

### Day 35：Week 5 复盘

任务：

- 完成 `EmbeddedDeviceGateway v2`
- 整理串口模块 README
- 整理开发板到货后的测试清单

今日产出：

- `notes/week05.md`
- `EmbeddedDeviceGateway/v2 README`

验收标准：

- 即使没有开发板，也已经完成嵌入式通信程序骨架。

---

## Week 6：交叉编译、部署、服务化

目标：让程序进入目标设备环境。

本周最好有开发板；如果没有，用 QEMU 或暂时只做交叉编译练习。

### Day 36：交叉编译基础

学习内容：

- 交叉编译器
- target
- host
- sysroot
- ARM Linux ABI

编码任务：

- 安装或准备交叉编译工具链
- 编译一个 hello 程序为 ARM 版本
- 用 `file` 查看可执行文件架构

今日产出：

- `day36_cross_compile_hello`

验收标准：

- 能说清楚为什么 x86 程序不能直接跑在 ARM 板上。

### Day 37：CMake toolchain

学习内容：

- CMake toolchain file
- `CMAKE_SYSTEM_NAME`
- `CMAKE_C_COMPILER`
- `CMAKE_CXX_COMPILER`

编码任务：

- 给 `EmbeddedDeviceGateway` 写 toolchain 文件
- 完成交叉编译

今日产出：

- `toolchains/arm-linux-toolchain.cmake`

验收标准：

- PC 版本和 ARM 版本都能构建。

### Day 38：开发板首次连接

学习内容：

- 串口终端
- ssh
- scp
- IP 配置
- 登录开发板

任务：

- 连接串口终端
- 登录开发板 shell
- 确认网络连通
- 通过 scp 传输文件

今日产出：

- `notes/day38_board_first_login.md`

验收标准：

- 能进入开发板 shell。
- 能把一个文件传到开发板。

### Day 39：部署和动态库问题

学习内容：

- `ldd`
- 动态库路径
- `/lib`
- `/usr/lib`
- `LD_LIBRARY_PATH`
- 静态链接和动态链接取舍

编码任务：

- 把 `EmbeddedDeviceGateway` 部署到开发板
- 解决运行时报错

今日产出：

- `notes/day39_deploy_debug.md`

验收标准：

- 程序能在开发板上启动。

### Day 40：配置文件和日志路径

学习内容：

- `/etc`
- `/var/log`
- `/opt`
- 嵌入式文件系统可写性

编码任务：

- 配置文件放到 `/etc/device_gateway.conf`
- 日志写到 `/var/log/device_gateway.log`
- 支持命令行指定配置路径

今日产出：

- `EmbeddedDeviceGateway/v3_deploy`

验收标准：

- 程序路径不再写死。

### Day 41：systemd 或 init 脚本

学习内容：

- systemd service
- init.d 脚本
- 开机启动
- 自动重启

编码任务：

- 写 `device_gateway.service`
- 支持开机启动
- 支持异常退出后重启

今日产出：

- `deploy/device_gateway.service`

验收标准：

- 开发板重启后服务能自动启动。

### Day 42：Week 6 复盘

任务：

- 完成 `EmbeddedDeviceGateway v3`
- 写部署文档
- 录制或整理一次部署流程

今日产出：

- `notes/week06.md`
- `docs/deploy_on_board.md`

验收标准：

- 你的项目已经从 PC 程序变成嵌入式 Linux 应用。

---

## Week 7：Buildroot、rootfs、嵌入式系统组成

目标：理解嵌入式 Linux 系统由 bootloader、kernel、device tree、rootfs、应用组成。

本周有开发板更好，没有开发板也可以用 QEMU。

### Day 43：嵌入式 Linux 系统组成

学习内容：

- bootloader
- kernel image
- device tree
- rootfs
- BusyBox
- init

任务：

- 画一张启动流程图
- 写一页系统组成笔记

今日产出：

- `notes/day43_embedded_linux_boot_flow.md`

验收标准：

- 能讲清楚上电后到应用启动的大致流程。

### Day 44：Buildroot 入门

学习内容：

- Buildroot 是什么
- defconfig
- menuconfig
- toolchain
- rootfs

任务：

- 拉通 Buildroot 基本概念
- 准备一个 QEMU 或开发板配置

今日产出：

- `notes/day44_buildroot_intro.md`

验收标准：

- 能解释 Buildroot 产物包括什么。

### Day 45：构建最小 rootfs

学习内容：

- BusyBox
- rootfs 格式
- kernel image

任务：

- 使用 Buildroot 构建最小系统
- 启动 QEMU 或准备烧录开发板

今日产出：

- `notes/day45_minimal_rootfs.md`

验收标准：

- 能看到最小 Linux shell。

### Day 46：把应用加入 rootfs

学习内容：

- Buildroot package 概念
- post-build script 概念
- rootfs overlay

任务：

- 把 `EmbeddedDeviceGateway` 放进 rootfs
- 启动后能找到应用

今日产出：

- `buildroot_overlay/device_gateway`

验收标准：

- 应用成为系统镜像的一部分。

### Day 47：开机启动应用

学习内容：

- init 脚本
- systemd 或 BusyBox init
- rcS

任务：

- 在最小系统中开机启动 `device_gateway`
- 观察日志

今日产出：

- `rootfs_overlay/etc/init.d/S99device_gateway`

验收标准：

- 系统启动后应用自动运行。

### Day 48：网络和远程调试

学习内容：

- ifconfig/ip
- ping
- netstat/ss
- telnet/nc
- 远程日志

任务：

- 从 PC 连接嵌入式系统中的 TCP 服务
- 验证协议通信

今日产出：

- `notes/day48_network_test.md`

验收标准：

- PC 能访问目标系统中的 gateway 服务。

### Day 49：Week 7 复盘

任务：

- 整理 Buildroot/rootfs 阶段文档
- 更新项目 README
- 形成演示流程

今日产出：

- `notes/week07.md`
- `docs/buildroot_rootfs.md`

验收标准：

- 能讲清楚应用如何进入嵌入式系统镜像。

---

## Week 8：BSP/驱动预备

目标：为开学后的 BSP/驱动学习建立入口。

本周强烈建议有开发板。

### Day 50：内核模块基础

学习内容：

- kernel module
- `insmod`
- `rmmod`
- `lsmod`
- `dmesg`

编码任务：

- 写 hello kernel module
- 加载和卸载模块
- 查看内核日志

今日产出：

- `driver_practice/day50_hello_module`

验收标准：

- 能成功看到内核模块日志。

### Day 51：字符设备驱动概念

学习内容：

- major/minor
- `file_operations`
- `open`
- `read`
- `write`
- `release`

编码任务：

- 写最小字符设备驱动
- 创建设备节点
- 用户态程序打开 `/dev/mydev`

今日产出：

- `driver_practice/day51_char_device`

验收标准：

- 用户态能通过 `/dev/mydev` 访问驱动。

### Day 52：ioctl 驱动接口

学习内容：

- ioctl 命令设计
- 用户态和内核态传参
- `copy_to_user`
- `copy_from_user`

编码任务：

- 给字符设备增加 ioctl
- 用户态程序设置和读取一个状态值

今日产出：

- `driver_practice/day52_ioctl`

验收标准：

- 能解释为什么有些控制不适合只用 read/write。

### Day 53：platform driver 和设备树概念

学习内容：

- platform bus
- compatible
- probe
- remove
- device tree node

任务：

- 阅读一份简单设备树节点
- 写 platform driver 学习笔记

今日产出：

- `notes/day53_platform_device_tree.md`

验收标准：

- 能说出设备树如何把硬件描述交给驱动。

### Day 54：GPIO 驱动/用户态对照

学习内容：

- GPIO controller
- gpiod
- 用户态 GPIO
- 内核态 GPIO API 概念

任务：

- 如果有开发板：控制一个 LED 或读取一个按键
- 如果没有开发板：整理 GPIO 用户态和驱动态对照笔记

今日产出：

- `notes/day54_gpio_compare.md`

验收标准：

- 能说明用户态 GPIO 和内核驱动 GPIO 的区别。

### Day 55：BSP 学习路线整理

学习内容：

- U-Boot
- kernel
- device tree
- rootfs
- 驱动
- 板级 bring-up

任务：

- 整理开学后的 BSP 学习路线
- 明确先学字符设备还是设备树

今日产出：

- `BSP_Driver_Next_Semester_Plan.md`

验收标准：

- 开学后的方向不再模糊。

### Day 56：最终复盘和作品整理

任务：

- 整理暑假项目
- 更新 README
- 写项目亮点
- 写简历描述
- 写面试讲解话术

最终交付物：

- `EmbeddedDeviceGateway`
- `docs/deploy_on_board.md`
- `docs/buildroot_rootfs.md`
- `notes/week01.md` 到 `notes/week08.md`
- `BSP_Driver_Next_Semester_Plan.md`

验收标准：

- 能用 3 分钟讲清楚：
  - 为什么先学 Linux 应用
  - 项目解决了什么问题
  - 怎么部署到嵌入式环境
  - 后续如何进入 BSP/驱动

---

## 今天 Day 1 立刻开始

今天只做 4 件事：

1. 确认 Linux 环境能用。
2. 创建 `linux_projects/day01_hello_linux`。
3. 编译运行第一个 C 程序。
4. 写 `notes/day01.md`，记录今天学到的命令和问题。

Day 1 最小代码：

```c
#include <stdio.h>

int main(void)
{
    printf("Hello, Linux embedded application!\n");
    return 0;
}
```

Day 1 推荐命令：

```bash
mkdir -p linux_projects/day01_hello_linux
cd linux_projects/day01_hello_linux
vim hello_linux.c
gcc -Wall -g hello_linux.c -o hello_linux
./hello_linux
file hello_linux
```

今日结束前，你需要能回答：

1. `gcc -Wall -g hello_linux.c -o hello_linux` 每个参数是什么意思？
2. 源文件和可执行文件有什么区别？
3. `file hello_linux` 显示了哪些信息？
4. 如果以后交叉编译到 ARM，为什么不能直接运行 x86 可执行文件？
