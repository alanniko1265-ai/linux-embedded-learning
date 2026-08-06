# Day42 设备网关项目上板运行

## 今日目标

Day42 的目标不是重新写一个新 demo，而是把 Day39 的设备网关综合项目部署到 i.MX6ULL 开发板运行。

今天完成的链路：

```text
PC / WSL 编译
ARM 交叉编译器生成 server 和 client
scp 上传到开发板
开发板运行 server
开发板运行 client
完成请求-响应通信
日志写入开发板文件系统
```

这一步把前面 Linux 应用开发阶段的设备网关项目真正迁移到了嵌入式 Linux 开发板上。

## 项目路径

本地项目路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day42_gateway_on_board
```

开发板部署路径：

```text
/home/debian/apps/day42_gateway_on_board
```

开发板目录内容：

```text
client
server
config/
logs/
```

配置文件路径：

```text
/home/debian/apps/day42_gateway_on_board/config/gateway.conf
```

日志文件路径：

```text
/home/debian/apps/day42_gateway_on_board/logs/server.log
```

## 交叉编译工具链调整

最开始使用 Ubuntu 22.04 自带的交叉编译器：

```text
/usr/bin/arm-linux-gnueabihf-gcc
gcc version 11.4.0
```

Day41 中发现它编译出来的动态链接程序在开发板上可能依赖较新的 glibc：

```text
GLIBC_2.34 not found
```

开发板当前 glibc 版本：

```text
Debian GLIBC 2.28
```

因此 Day42 改用便携版 Arm GNU Toolchain 8.3：

```text
/home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
gcc version 8.3.0
```

重新编译后生成的是 ARM 32-bit 动态链接程序：

```text
build/server: ELF 32-bit ARM dynamically linked
build/client: ELF 32-bit ARM dynamically linked
```

这一次动态链接版本可以在 i.MX6ULL 上正常运行。

## Makefile 修改

Day42 的 Makefile 使用 ARM 交叉编译器：

```makefile
cc := arm-linux-gnueabihf-gcc
st := -Wall -g -O0 -Iinclude
```

核心区别：

```text
gcc
  编译 x86_64 主机程序

arm-linux-gnueabihf-gcc
  编译 ARM Linux 目标板程序
```

## 上传文件

创建开发板目录：

```bash
ssh debian@192.168.7.2 "mkdir -p /home/debian/apps/day42_gateway_on_board/config /home/debian/apps/day42_gateway_on_board/logs"
```

上传 server 和 client：

```bash
scp build/server build/client debian@192.168.7.2:/home/debian/apps/day42_gateway_on_board/
```

上传配置文件：

```bash
scp config/gateway.conf debian@192.168.7.2:/home/debian/apps/day42_gateway_on_board/config/
```

注意：

```text
路径最后加 / 表示目标是目录
路径以 /home/... 开头表示开发板上的绝对路径
```

## 开发板运行结果

server 启动：

```text
demo_gateway server listening on port 9200
```

client 输入：

```text
status
led on
led off
abc
quit
```

client 响应：

```text
[response] OK code=0 msg=status device=sensor_node_01 online=1 temp=32 voltage=24 led=off
[response] OK code=0 msg=led_on
[response] OK code=0 msg=led_off
[response] ERR code=1001 msg=unknown_command
```

server 端日志：

```text
client connected, fd=6
[fd=6] request: status
[fd=6] request: led on
[fd=6] request: led off
[fd=6] request: abc
client disconnect,fd=6
```

按 `Ctrl + C` 后 server 正常停止。

## 日志验证

开发板日志文件：

```text
logs/server.log
```

日志内容包含：

```text
server starting
server config loaded
client connected
request received: status
response built: OK code=0 msg=status ...
request received: led on
response built: OK code=0 msg=led_on
request received: led off
response built: OK code=0 msg=led_off
request received: abc
response built: ERR code=1001 msg=unknown_command
client disconnected
server stopped
```

说明：

```text
配置加载正常
客户端连接正常
请求解析正常
响应生成正常
日志模块在开发板文件系统中正常工作
信号停止流程正常
```

## 今日踩坑

### 命令拼写错误

错误命令：

```bash
mkdur -p ...
```

正确命令：

```bash
mkdir -p ...
```

`command not found` 通常表示命令名字写错，或者系统没有安装该命令。

### 引号没有闭合

如果终端出现：

```text
>
```

通常表示当前命令还没输入完整，例如双引号没有闭合。可以按 `Ctrl + C` 取消当前输入。

### scp 路径少了 /

错误路径：

```text
debian@192.168.7.2:home/debian/apps/day42_gateway_on_board/
```

正确路径：

```text
debian@192.168.7.2:/home/debian/apps/day42_gateway_on_board/
```

前者是相对路径，后者是开发板根目录下的绝对路径。

## 今日结论

Day42 完成了从 Linux 应用项目到嵌入式 Linux 开发板项目的迁移。

真正掌握的不是某一个命令，而是这条工程链路：

```text
选择合适交叉工具链
编译 ARM Linux 程序
检查 ELF 架构和链接方式
上传程序和配置文件
在目标板运行服务端和客户端
验证通信和日志
```

这就是后续 GPIO、I2C、UART、驱动接口项目都会复用的基础流程。

