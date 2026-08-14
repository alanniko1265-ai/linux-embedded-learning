# Day49：i.MX6ULL RS485 C 应用

## 今日目标

把 Day47 使用 Shell 完成的 RS485 收发过程封装成可复用的 Linux C 模块，并在 i.MX6ULL 与 Windows USB-RS485 模块之间完成双向通信。

项目目录：

```text
C:\Users\20858\Documents\实习\linux_projects\day49_rs485_app
```

## 硬件与系统对应关系

| 项目 | 当前使用值 |
| --- | --- |
| 板端接口 | RS485-1 |
| 串口设备 | `/dev/ttymxc1` |
| 收发方向 GPIO | GPIO22 |
| GPIO 值文件 | `/sys/class/gpio/gpio22/value` |
| GPIO22=1 | 板端发送 |
| GPIO22=0 | 板端接收 |
| 波特率 | 9600，8N1，无流控 |
| PC 适配器 | USB-RS485，测试时为 COM11 |
| 接线 | A 接 T/R+，B 接 T/R- |

J7 必须给 RS485 电路供电。Day47 通信失败的根因是 J7 跳帽未接，而不是串口程序、A/B 接线或开发板损坏。

## 项目结构

```text
day49_rs485_app/
├── include/
│   └── rs485.h
├── src/
│   ├── main.c
│   └── rs485.c
├── build/
│   └── rs485_tool
└── Makefile
```

模块调用关系：

```text
main.c
  ├── rs485_open()
  ├── rs485_send() 或 rs485_receive()
  └── rs485_close()
```

`main.c` 负责解析 `send`、`receive` 命令和显示结果；`rs485.c` 负责设备打开、串口配置、方向切换和实际收发。

## 设备结构体

```c
struct Rs485Device {
    int serial_fd;
    int direction_fd;
};
```

- `serial_fd` 指向 `/dev/ttymxc1`，负责发送和接收数据。
- `direction_fd` 指向 GPIO22 的 `value` 文件，负责切换 RS485 收发方向。
- 将两个文件描述符放进同一结构体，可以把一个 RS485 通道作为完整设备传递给各个函数。

## 打开与配置

`rs485_open()` 的主要过程：

1. 打开串口设备和 GPIO 方向文件。
2. 使用 `tcgetattr()` 读取原串口配置。
3. 使用 `cfmakeraw()` 设置原始模式，防止终端处理字符。
4. 配置 8 位数据位、无校验、1 位停止位、无硬件流控。
5. 使用 `cfsetispeed()`、`cfsetospeed()` 设置波特率。
6. 使用 `tcsetattr()` 立即应用配置。
7. 默认切换到接收状态。

`baud_to_speed()` 将普通整数 `9600` 转换为 termios 使用的 `B9600`。

## 发送流程

```text
rs485_send()
  -> GPIO22 写 1，进入发送状态
  -> write() 循环处理短写和 EINTR
  -> tcdrain() 等待串口硬件真正发送完毕
  -> GPIO22 写 0，恢复接收状态
```

不能在 `write()` 返回后立刻切换为接收，因为 `write()` 只保证数据已经交给内核，不保证最后一个字节已经从 UART 引脚发完。`tcdrain()` 用来等待发送队列清空。

## 接收流程

```text
rs485_receive()
  -> GPIO22 写 0，进入接收状态
  -> poll() 等待串口出现 POLLIN 事件
  -> 超时返回 0
  -> 出错返回 -1
  -> 有数据时 read() 并返回字节数
```

`poll()` 让程序在最多 15 秒内等待数据，不需要持续循环读取占用 CPU。

## 构建

手动编译验证通过后，使用 Makefile 保存构建规则：

```bash
make clean
make
file build/rs485_tool
```

验证结果：

```text
ELF 32-bit LSB executable, ARM, EABI5
```

这说明产物面向 ARM，可部署到 i.MX6ULL；`main.c` 和 `rs485.c` 也已经成功链接成一个程序。

## 板端准备

板子重启后，旧式 sysfs GPIO 导出状态不会保留，需要重新执行：

```bash
echo 22 > /sys/class/gpio/export 2>/dev/null
echo out > /sys/class/gpio/gpio22/direction
echo 0 > /sys/class/gpio/gpio22/value
```

程序部署目录：

```text
/home/debian/apps/day49_rs485_app/rs485_tool
```

## 双向测试结果

### 开发板发送到电脑

板端：

```bash
./rs485_tool send "hello_from_rs485_c"
```

结果：

```text
sent 19 bytes
```

PowerShell 接收到：

```text
hello_from_rs485_c
```

### 电脑发送到开发板

板端先执行：

```bash
./rs485_tool receive
```

然后由 PowerShell 通过 COM11 发送。开发板成功收到数据，反向链路验证通过。

## 今日结论

- 已掌握 RS485 半双工通信为什么需要方向控制。
- 已用 termios 配置真实板端串口，而不是依赖 Shell 的默认配置。
- 已用 `poll()` 实现带超时的阻塞等待。
- 已处理 `write()` 短写、`EINTR`、超时和设备关闭。
- 已完成 PC 与 i.MX6ULL 之间的双向 RS485 C 程序验证。

## 待整理的小问题

当前 `main.c` 有两处仅影响显示的拼写：

```text
unknown cpmmand  -> unknown command
RS485 date       -> RS485 data
```

它们不影响通信逻辑，可以在下一次整理代码时修正。
