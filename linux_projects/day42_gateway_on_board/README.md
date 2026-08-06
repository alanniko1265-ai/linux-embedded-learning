# Embedded Linux Device Gateway

## 项目简介

这是一个基于 Linux C 的嵌入式设备网关项目，使用 TCP 作为通信方式，实现客户端与服务端之间的命令请求和状态响应。

项目模拟一个嵌入式网关服务：客户端可以查询设备状态、控制 LED 状态、发送重启命令；服务端负责维护设备状态、记录运行日志、读取配置文件，并支持基于 SIGINT / SIGTERM 的服务退出处理与资源释放。

该项目当前运行在 WSL / Ubuntu 环境，后续可迁移到嵌入式 Linux 开发板，通过交叉编译部署，并将软件 LED 状态替换为真实 GPIO 控制。

## 核心功能

- TCP server / client 通信
- epoll 多客户端事件处理
- length + payload 应用层协议
- 设备状态查询
- LED 状态控制
- 模拟温度 / 电压动态采样
- 配置文件读取
- 日志文件记录
- SIGINT / SIGTERM 信号处理与资源清理
- 统一响应格式与错误码

## 项目结构

```text
day39_device_gateway_final/
├── include/
│   ├── command.h
│   ├── config.h
│   ├── device_state.h
│   ├── logger.h
│   └── protocol.h
├── src/
│   ├── client.c
│   ├── command.c
│   ├── config.c
│   ├── device_state.c
│   ├── logger.c
│   ├── protocol.c
│   └── server.c
├── config/
│   └── gateway.conf
├── logs/
│   └── server.log
├── Makefile
└── README.md
```

## 模块职责

| 模块 | 职责 |
| --- | --- |
| `server.c` | 创建 TCP 服务端，使用 epoll 处理客户端事件，管理服务主循环和资源释放 |
| `client.c` | 读取配置文件，连接服务端，发送命令并接收响应 |
| `protocol.c` | 实现 length + payload 应用层协议，解决 TCP 字节流粘包/半包问题 |
| `command.c` | 解析客户端命令，生成统一响应 |
| `device_state.c` | 维护设备状态，处理 LED 状态、温度和电压采样 |
| `config.c` | 读取 `config/gateway.conf`，加载端口、设备名和日志路径 |
| `logger.c` | 将服务运行事件写入日志文件 |

## 构建与运行

### 编译项目

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day39_device_gateway_final
make clean
make
```

编译完成后会生成：

```text
build/server
build/client
```

### 启动服务端

终端 1：

```bash
make run1
```

服务端会读取：

```text
config/gateway.conf
```

并根据配置启动 TCP 服务。

### 启动客户端

终端 2：

```bash
make run2
```

客户端启动后可以输入命令：

```text
status
led on
led off
reboot
abc
quit
```

### 查看日志

```bash
cat logs/server.log
```

## 配置文件

配置文件路径：

```text
config/gateway.conf
```

示例内容：

```text
server_port=9200
device_name=demo_gateway
log_path=logs/server.log
```

字段说明：

| 字段 | 说明 |
| --- | --- |
| `server_port` | 服务端监听端口 |
| `device_name` | 网关服务名称 |
| `log_path` | 日志文件路径 |

服务端和客户端都会读取同一个配置文件中的 `server_port`。修改端口时只需要更新 `config/gateway.conf`，不需要分别修改 `server.c` 和 `client.c`。

## 命令与响应格式

### 支持命令

| 命令 | 功能 |
| --- | --- |
| `status` | 查询设备状态，并触发一次模拟采样 |
| `led on` | 打开 LED 状态 |
| `led off` | 关闭 LED 状态 |
| `reboot` | 模拟设备重启命令 |
| 其他命令 | 返回未知命令错误 |

### 响应格式

成功响应：

```text
OK code=0 msg=...
```

失败响应：

```text
ERR code=1001 msg=...
```

响应示例：

```text
OK code=0 msg=status device=sensor_node_01 online=1 temp=32 voltage=24 led=off
OK code=0 msg=led_on
OK code=0 msg=led_off
OK code=0 msg=reboot_scheduled
ERR code=1001 msg=unknown_command
```

## 应用层协议

TCP 是字节流协议，不能保证一次 `send()` 对应一次 `recv()`。项目在 `protocol.c` 中实现了 length + payload 应用层协议：

```text
4 字节消息长度
实际消息内容 payload
```

发送流程：

```text
计算 payload 长度
长度字段转换为网络字节序
发送长度字段
发送 payload
```

接收流程：

```text
读取 4 字节长度字段
转换为本机字节序
检查长度合法性
按长度读取完整 payload
补 '\0' 形成 C 字符串
```

该协议用于解决 TCP 通信中的粘包、半包问题。

## 运行示例

客户端输入：

```text
status
led on
status
led off
reboot
abc
quit
```

客户端输出示例：

```text
[response] OK code=0 msg=status device=sensor_node_01 online=1 temp=32 voltage=24 led=off
[response] OK code=0 msg=led_on
[response] OK code=0 msg=status device=sensor_node_01 online=1 temp=33 voltage=25 led=on
[response] OK code=0 msg=led_off
[response] OK code=0 msg=reboot_scheduled
[response] ERR code=1001 msg=unknown_command
```

服务端日志示例：

```text
[INFO] server starting
[INFO] server config loaded
[INFO] client connected
[INFO] request received: status
[INFO] response built: OK code=0 msg=status device=sensor_node_01 online=1 temp=32 voltage=24 led=off
[INFO] request received: led on
[INFO] response built: OK code=0 msg=led_on
[INFO] client disconnected
[INFO] server stopped
```

## 技术要点

- 使用 `socket` / `bind` / `listen` / `accept` 创建 TCP 服务端
- 使用 `connect` 实现 TCP 客户端连接
- 使用 `epoll` 管理多个客户端连接事件
- 使用 length + payload 协议封装应用层消息
- 使用多文件结构拆分协议、命令、设备状态、日志和配置模块
- 使用 `fopen` / `fgets` / `sscanf` 读取配置文件
- 使用 `fprintf` / `fflush` 记录服务端运行日志
- 使用 `SIGINT` / `SIGTERM` 处理服务退出请求
- 使用 `errno == EINTR` 处理 `epoll_wait()` 被信号中断的情况
- 使用结构体维护设备状态，并通过命令修改状态

## 后续迁移方向

当前项目运行在 WSL / Ubuntu 环境，后续可以迁移到嵌入式 Linux 开发板：

```text
PC/WSL 编译运行
        |
        v
交叉编译为 ARM 可执行文件
        |
        v
通过 scp 上传到开发板
        |
        v
在开发板运行 server
        |
        v
将 led_on 软件状态替换为真实 GPIO 控制
```

迁移后可继续扩展：

- 使用真实 GPIO 控制 LED
- 从 sysfs、I2C 传感器或驱动节点读取设备状态
- 使用 systemd 或启动脚本管理服务进程
- 将日志和配置文件放到板端固定目录
- 使用 Qt 上位机连接网关并解析响应

## 简历描述参考

项目名称：

```text
基于 Linux C 的嵌入式设备网关服务
```

简历描述：

```text
基于 Linux C 实现嵌入式设备网关服务，采用 TCP 通信和 epoll 事件模型支持多客户端连接，设计 length + payload 应用层协议解决 TCP 粘包/半包问题。项目模块化拆分为协议、命令解析、设备状态、配置和日志模块，支持设备状态查询、LED 状态控制、动态采样模拟、统一响应码、配置文件加载，以及基于 SIGINT/SIGTERM 的资源清理流程。后续可通过交叉编译部署到嵌入式 Linux 开发板，并接入真实 GPIO 控制。
```

可以提炼到简历项目要点：

- 使用 Linux socket 和 epoll 实现多客户端 TCP 网关服务
- 设计 length + payload 应用层协议处理粘包和半包
- 实现配置文件、日志记录、信号处理和资源释放机制
- 使用模块化结构维护设备状态，并支持命令控制 LED 状态
- 预留开发板迁移接口，可替换为真实 GPIO 和传感器数据读取

## 当前状态

- [x] TCP server / client
- [x] epoll 多客户端事件处理
- [x] length + payload 应用层协议
- [x] 配置文件读取
- [x] 日志记录
- [x] SIGINT / SIGTERM 信号处理
- [x] 设备状态管理
- [x] LED 状态控制
- [x] 动态采样模拟
- [x] 统一响应格式与错误码
- [ ] 交叉编译到开发板
- [ ] 接入真实 GPIO
- [ ] 板端部署与开机启动
