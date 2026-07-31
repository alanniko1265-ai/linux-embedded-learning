# Day31 设备网关项目结构重构

## 今日目标

把 Day30 的单文件 `epoll + length/payload + request/response` 项目，重构成更接近真实嵌入式 Linux 项目的多文件结构。

目标不是新增 API，而是拆清楚职责：

```text
protocol 层：负责 length + payload 协议收发
command 层：负责命令解析和响应生成
server 层：负责 socket + epoll 多客户端
client 层：负责输入命令和打印响应
```

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day31_device_gateway_refactor
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day31_device_gateway_refactor
```

VS Code：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day31_device_gateway_refactor
code .
```

## 工程文件

```text
day31_device_gateway_refactor/
├── include/
│   ├── protocol.h
│   └── command.h
├── src/
│   ├── protocol.c
│   ├── command.c
│   ├── server.c
│   └── client.c
├── build/
│   ├── server
│   └── client
└── Makefile
```

## 模块职责

`protocol.h / protocol.c`：

```text
read_exact
send_exact
read_message
send_message
```

负责 TCP 字节流上的完整消息收发。

`command.h / command.c`：

```text
build_response
```

负责把请求命令转换成响应字符串。

`server.c`：

```text
create_server_socket
epoll_create1
epoll_ctl
epoll_wait
accept
handle_client_event
```

负责网络监听、多客户端事件分发和调用业务接口。

`client.c`：

```text
connect_to_server
fgets
send_message
read_message
```

负责测试 server 的请求-响应协议。

## 头文件的作用

头文件只放声明：

```c
int read_message(int fd, char *buffer, size_t buffer_size);
int send_message(int fd, const char *message);
```

源文件放实现：

```c
int read_message(...) { ... }
```

这样 `server.c` 和 `client.c` 可以通过：

```c
#include "protocol.h"
```

使用协议函数，而不需要重复写实现。

头文件里的函数声明不能写 `static`。

原因：

```text
static 表示当前源文件内部私有
头文件是给多个 .c 共用的公共接口
```

## protocol 模块

`protocol.h` 暴露：

```c
ssize_t read_exact(int fd, void *buffer, size_t size);
ssize_t send_exact(int fd, const void *buffer, size_t size);
int read_message(int fd, char *buffer, size_t buffer_size);
int send_message(int fd, const char *message);
```

`read_message()` 关键流程：

```text
read_exact 读取 4 字节长度
ntohl 转成本机长度
检查长度合法性
read_exact 读取 payload
补 '\0'
返回 payload 长度
```

`send_message()` 关键流程：

```text
strlen 得到 payload 长度
htonl 转网络字节序
send_exact 发送长度字段
send_exact 发送 payload
```

## command 模块

`command.h` 暴露：

```c
void build_response(const char *request, char *response, size_t response_size);
```

支持命令：

```text
status   -> OK device=online temp=32 voltage=24
led on   -> OK led=on
led off  -> OK led=off
reboot   -> OK reboot scheduled
其他命令 -> ERR unknown command
```

`strcmp()` 判断相等必须写：

```c
strcmp(request, "status\n") == 0
```

因为 `strcmp` 相等时返回 `0`。

## server 模块

重构后的 `server.c` 不再实现协议细节，也不再实现命令细节。

它只调用：

```c
read_message(client_fd, request, sizeof(request));
build_response(request, response, sizeof(response));
send_message(client_fd, response);
```

这就是模块化的意义：

```text
server.c 关注“谁有事件”
protocol.c 关注“如何完整收发”
command.c 关注“命令如何响应”
```

## client 模块

重构后的 `client.c` 也不再重复写协议函数。

它只包含：

```c
#include "protocol.h"
```

然后直接调用：

```c
send_message(client_fd, request);
read_message(client_fd, response, sizeof(response));
```

## Makefile 链接关系

server 需要：

```text
src/server.c
src/protocol.c
src/command.c
```

client 需要：

```text
src/client.c
src/protocol.c
```

Makefile：

```makefile
src1 := src/server.c src/protocol.c src/command.c
src2 := src/client.c src/protocol.c
```

`-Iinclude` 的作用：

```text
告诉 gcc 去 include/ 目录查找 protocol.h 和 command.h
```

## 今日遇到的问题

1. 头文件中的函数声明不应该加 `static`。
2. `send_message` 声明和实现的参数必须一致。
3. `read_exact` 中 `n == 0` 应返回 `0`，表示对端断开。
4. `read_message` 读取长度字段也要用 `read_exact`。
5. `read_message` 中 `n == 0` 要直接返回，不能继续解析长度。
6. `strcmp` 相等时返回 `0`，条件必须写 `== 0`。
7. `server.c` 调用 `read_message/send_message/build_response` 时，链接阶段必须带上 `protocol.c/command.c`。
8. 只 `#include` 头文件不等于链接了函数实现。

## undefined reference

遇到：

```text
undefined reference to `read_message'
undefined reference to `build_response'
undefined reference to `send_message'
```

原因：

```text
server.c 只看到了函数声明
链接时没有把 protocol.c / command.c 一起编译进目标程序
```

解决：

```bash
gcc -Wall -g -O0 -Iinclude src/server.c src/protocol.c src/command.c -o build/server
```

一句话：

```text
#include 负责声明可见，.c 文件参与编译链接才有函数实现。
```

## 今日完成状态

- [x] 建立 include / src 目录结构
- [x] 完成 protocol.h / protocol.c
- [x] 完成 command.h / command.c
- [x] 重构 server.c
- [x] 重构 client.c
- [x] 理解头文件声明与源文件实现的区别
- [x] 解决多文件链接 undefined reference
- [x] 编写多文件 Makefile
- [x] 完成 server/client 编译
