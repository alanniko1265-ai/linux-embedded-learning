# Day44 设备网关接入真实 LED

## 今日目标

Day44 的目标是把 Day42 的设备网关项目和 Day43 的 LED 控制逻辑合并。

最终效果：

```text
client 输入 led on
  -> server 收到 TCP 请求
  -> command 模块解析命令
  -> led_control_set("red", 1)
  -> 写 /sys/class/leds/red/brightness
  -> 开发板 red LED 真实点亮
```

这一步让设备网关从“模拟设备状态”变成了“可以控制真实硬件”。

## 项目路径

本地路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day44_gateway_led_hardware
```

开发板部署路径：

```text
/home/debian/apps/day44_gateway_led_hardware
```

## 项目来源

Day44 基于 Day42 项目复制而来：

```text
day42_gateway_on_board
  -> day44_gateway_led_hardware
```

然后新增 LED 控制模块：

```text
include/led_control.h
src/led_control.c
```

## LED 控制模块

头文件：

```c
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

int led_control_set(const char *led_name, int on);
int led_control_get(const char *led_name);

#endif
```

核心接口：

```text
led_control_set("red", 1)
  点亮 red LED

led_control_set("red", 0)
  熄灭 red LED

led_control_get("red")
  读取 red LED 当前 brightness
```

当前支持：

```text
red
green
blue
```

Day44 网关先固定控制：

```text
red
```

## led_control.c 核心逻辑

路径拼接：

```text
/sys/class/leds/red/brightness
```

写 LED：

```text
open(path, O_WRONLY)
write(fd, "1" 或 "0", 1)
close(fd)
```

读 LED：

```text
open(path, O_RDONLY)
read(fd, &value, 1)
close(fd)
```

返回值约定：

```text
led_control_get() 返回 0：LED 关闭
led_control_get() 返回 1：LED 点亮
led_control_get() 返回 -1：读取失败
```

## Makefile 修改

server 需要链接 `src/led_control.c`：

```makefile
src1 := src/server.c src/protocol.c src/command.c src/logger.c src/config.c src/device_state.c src/led_control.c
```

client 不需要链接 LED 模块，因为 client 不直接访问硬件。

工具链固定为 Arm GNU Toolchain 8.3：

```makefile
cc := /home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
```

这样可以避免使用 Ubuntu 22.04 默认交叉编译器导致：

```text
GLIBC_2.34 not found
```

## command.c 接入方式

`led on`：

```text
先调用 led_control_set("red", 1)
如果硬件操作成功，再更新内存状态
如果硬件操作失败，返回 ERR
```

`led off`：

```text
先调用 led_control_set("red", 0)
如果硬件操作成功，再更新内存状态
如果硬件操作失败，返回 ERR
```

`status`：

```text
先调用 led_control_get("red")
如果读取成功，把真实 LED 状态同步到 DeviceState
再生成 status 响应
```

这样 `status` 返回的不再只是内存变量，而是会反映真实 red LED 状态。

## 编译和部署

本地编译：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day44_gateway_led_hardware

make clean
make
file build/server build/client
```

部署到开发板：

```bash
ssh debian@192.168.7.2 "mkdir -p /home/debian/apps/day44_gateway_led_hardware/config /home/debian/apps/day44_gateway_led_hardware/logs"

scp build/server build/client debian@192.168.7.2:/home/debian/apps/day44_gateway_led_hardware/

scp config/gateway.conf debian@192.168.7.2:/home/debian/apps/day44_gateway_led_hardware/config/
```

## 开发板运行

server：

```bash
cd /home/debian/apps/day44_gateway_led_hardware
./server
```

client：

```bash
cd /home/debian/apps/day44_gateway_led_hardware
./client
```

输入：

```text
status
led on
status
led off
status
quit
```

client 输出：

```text
[response] OK code=0 msg=status device=sensor_node_01 online=1 temp=32 voltage=24 led=off
[response] OK code=0 msg=led_on
[response] OK code=0 msg=status device=sensor_node_01 online=1 temp=33 voltage=25 led=on
[response] OK code=0 msg=led_off
[response] OK code=0 msg=status device=sensor_node_01 online=1 temp=34 voltage=24 led=off
```

实际观察：

```text
led on 后 red LED 真实点亮
led off 后 red LED 真实熄灭
status 能反映真实 LED 状态
```

## 日志验证

开发板日志：

```text
/home/debian/apps/day44_gateway_led_hardware/logs/server.log
```

关键内容：

```text
server starting
server config loaded
client connected
request received: status
response built: OK ... led=off
request received: led on
response built: OK code=0 msg=led_on
request received: status
response built: OK ... led=on
request received: led off
response built: OK code=0 msg=led_off
request received: status
response built: OK ... led=off
client disconnected
server stopped
```

说明：

```text
server 启动正常
client 连接正常
命令解析正常
LED 硬件控制正常
status 状态同步正常
日志模块正常
Ctrl+C 停止正常
```

## 今日踩坑

### 复制终端提示符

曾经把终端输出也复制进终端执行，例如：

```text
debian@npi:~/apps/day44_gateway_led_hardware$ ./client
[response] OK ...
```

真正应该输入的只有命令本身：

```bash
./client
```

提示符和响应内容不是命令，不要复制执行。

### server 需要 root 权限

写：

```text
/sys/class/leds/red/brightness
```

可能需要 root 权限。

如果普通用户运行 server 后 `led on` 返回失败，可以切换到 root 运行 server。

## 今日结论

Day44 完成了设备网关项目的重要升级：

```text
TCP client
  -> TCP server
  -> 应用层协议
  -> command 模块
  -> LED 控制模块
  -> Linux LED 子系统
  -> 真实硬件 LED
```

这是一个真正有嵌入式应用开发味道的项目节点。

从今天开始，设备网关不只是“会通信”，而是已经能通过 Linux 设备接口控制硬件。

