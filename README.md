# Linux 嵌入式学习笔记与项目

这是我的 Linux 嵌入式学习仓库，记录从 Linux 应用开发向驱动与 BSP 开发转型的过程。

当前采用“以练带学”的方式推进：每个阶段都尽量形成可运行的小项目、调试记录和复盘笔记，为后续的 RK3568 + STM32 车载联合项目，以及 Linux 车载嵌入式、音视频嵌入式方向的实习做准备。

## 当前环境

- 开发板：野火 EBF6ULL S1 Pro
- SoC：NXP i.MX6ULL（ARM Cortex-A7）
- 板端系统：Debian GNU/Linux 10
- 板端内核：Linux 4.19.35-imx6
- 开发方式：Windows + WSL/Linux 主机交叉编译，通过 SSH/SCP 与开发板联调
- 当前重点：Linux 内核构建、字符设备驱动、设备树和用户态应用联调

## 仓库结构

```text
.
├── README.md
├── linux_projects/    # 每日练习、阶段项目和当前主项目
└── notes/             # 学习笔记、命令解释、问题记录和复盘
```

仓库只保留以上学习内容。编译产物、临时目录、个人 IDE 配置和招聘截图分析不纳入版本管理。

## 学习路线

| 阶段 | 内容 | 对应成果 |
| --- | --- | --- |
| Day 01–07 | C 工程基础、GCC、Make/CMake、GDB、静态库与动态库 | 可维护的多文件 C 项目 |
| Day 08–18 | 文件 I/O、目录、进程、信号、非阻塞 I/O、`/proc`、`ioctl`、`mmap` | Linux 系统编程基础 |
| Day 19–22 | pthread、线程队列、IPC、本地命令服务 | 并发与进程间通信 |
| Day 23–31 | TCP、select/poll/epoll、自定义协议、请求响应 | 网络服务与协议设计 |
| Day 35–39 | 配置、日志、设备状态、响应码 | 设备网关阶段项目 |
| Day 40–49 | 开发板 bring-up、交叉编译、LED/按键、systemd、RS-485 | 应用落板与硬件交互 |
| Day 53–54 | logrotate、运行诊断 | 部署和可维护性 |
| Day 55 起 | 内核构建、字符设备驱动、设备树、应用联调 | Vehicle Sensor Lab |

## 重点项目

### 1. 设备网关

从 TCP 服务逐步演进出的设备网关，包含：

- epoll 多客户端处理
- 自定义请求/响应协议
- 配置文件与分级日志
- 设备状态管理
- 开发板 LED 和按键交互
- systemd 服务、日志轮转和运行诊断

主要版本：

- [Day 39：设备网关阶段完成版](linux_projects/day39_device_gateway_final/)
- [Day 46：按键状态接入](linux_projects/day46_gateway_key_status/)
- [Day 48：systemd 服务化](linux_projects/day48_gateway_systemd_service/)
- [Day 54：运行诊断](linux_projects/day54_gateway_diagnostics/)

### 2. Vehicle Sensor Lab（当前主项目）

[Vehicle Sensor Lab](linux_projects/vehicle_sensor_lab/) 是面向车载 Linux 场景的虚拟传感器驱动实验。第一阶段不依赖额外传感器，通过 i.MX6ULL 完成从内核驱动到用户态应用的完整链路：

```text
设备树/平台设备
        ↓
Linux 字符设备驱动
        ↓
/dev/vehicle_sensor
        ↓
用户态采集与展示程序
```

计划能力：

- 内核模块加载、卸载和日志观察
- 字符设备注册与 `file_operations`
- `read`、`poll`、`ioctl` 等接口
- 设备树匹配与 platform driver
- 用户态采集、状态展示和异常处理
- 后续接入车载网关或 Qt 可视化界面

当前进度：

- 已确认板端内核为 `4.19.35-imx6`
- 已获取运行内核配置并匹配野火 `ebf_4.19.35_imx6ul` 源码
- 已建立独立输出目录并完成 `olddefconfig`
- 已应用旧内核兼容新版 GCC 的 DTC `yylloc` 修复补丁
- 下一步：完成内核构建，生成 `Module.symvers`，再编译并加载第一个驱动模块

相关资料：

- [项目总览](linux_projects/vehicle_sensor_lab/README.md)
- [前置知识与命令解释](linux_projects/vehicle_sensor_lab/docs/prerequisite_guide.md)
- [架构说明](linux_projects/vehicle_sensor_lab/docs/architecture.md)
- [Day 55 启动记录](linux_projects/vehicle_sensor_lab/docs/day55_start.md)

## 使用方式

学习笔记在 [notes](notes/) 中按 Day 编号排列；对应代码位于 [linux_projects](linux_projects/)。

多数应用项目可以进入对应目录后执行：

```bash
make
./生成的程序
```

交叉编译和板端运行项目请先阅读项目目录中的 README 或当天笔记，不直接照搬主机端命令。

## 开发习惯

当前阶段保持简单但可靠的工程习惯：

- 每次只提交一个明确主题
- 提交前检查 `git diff` 和编译结果
- 源码、头文件、配置、部署文件分目录组织
- 命令不仅记录“怎么做”，也记录“为什么”
- 错误和修复过程保留在笔记中，形成可复现的排查链路
- 不提交编译产物、密钥、临时文件和个人 IDE 配置

目标不是一开始套用复杂流程，而是在持续做项目的同时逐步接近规范、可复现、可维护的嵌入式开发方式。
