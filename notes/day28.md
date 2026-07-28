# Day28 TCP 粘包/拆包与应用层协议设计

## 今日目标

在前几天 TCP echo server 的基础上，理解 TCP 是字节流协议，不天然保留消息边界，并实现一个最小应用层协议：

```text
+----------------+----------------+
| 4 字节长度字段  | payload 正文    |
+----------------+----------------+
```

今天重点：

- 理解 TCP 字节流和消息边界问题
- 使用 `uint32_t` 表示固定 4 字节长度字段
- 使用 `htonl` / `ntohl` 处理网络字节序
- 服务端按协议读取完整消息
- 客户端按协议发送 `length + payload`
- 手动编译后再写 Makefile

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day28_tcp_protocol
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day28_tcp_protocol
```

VS Code：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day28_tcp_protocol
code .
```

## 工程文件

```text
day28_tcp_protocol/
├── src/
│   ├── protocol_server.c
│   └── protocol_client.c
├── build/
│   ├── protocol_server
│   └── protocol_client
└── Makefile
```

## 编译运行

手动编译：

```bash
mkdir -p build
gcc -Wall -g -O0 src/protocol_server.c -o build/protocol_server
gcc -Wall -g -O0 src/protocol_client.c -o build/protocol_client
```

Makefile 编译：

```bash
make clean
make
```

终端一：

```bash
make runse
```

终端二：

```bash
make runc
```

客户端输入：

```text
hello protocol
status
set led on
quit
```

服务端典型输出：

```text
client connected
[message] len=15, data=hello protocol
[message] len=7, data=status
[message] len=11, data=set led on
client disconnected
```

`fgets()` 会保留回车，所以 `status\n` 的长度是 7。

## 为什么 TCP 需要应用层协议

TCP 是字节流协议。

这意味着：

```text
客户端 send 两次
服务端不一定 recv 两次

客户端 send 一次
服务端也不一定一次 recv 完
```

因此真实项目不能只靠：

```c
recv(fd, buffer, sizeof(buffer), 0);
```

就认为读到了一条完整消息。

应用层必须自己规定消息格式。本项目使用最简单的长度头协议：

```text
先发 4 字节长度
再发 payload 正文
```

## uint32_t 的作用

```c
uint32_t length;
```

`uint32_t` 表示无符号 32 位整数，固定占 4 字节。

协议字段需要大小明确，所以长度字段不用普通 `int`。

```text
int       大小理论上依赖平台
uint32_t  明确就是 4 字节
```

## 网络字节序

协议传输多字节整数时，必须统一字节顺序。

发送端：

```c
uint32_t net_length = htonl(length);
```

含义：

```text
host to network long
本机字节序 -> 网络字节序
```

接收端：

```c
uint32_t length = ntohl(net_length);
```

含义：

```text
network to host long
网络字节序 -> 本机字节序
```

一句话：

```text
协议传输用网络字节序，程序计算用本机字节序。
```

字符串 payload 不需要 `ntohl`，因为字节序问题只影响多个字节组成的整数。

## read_exact

```c
static ssize_t read_exact(int fd, void *buffer, size_t size)
```

作用：

```text
一直 recv，直到读满 size 个字节
```

核心变量：

```c
size_t total = 0;
char *ptr = buffer;
```

`total` 表示已经读到多少字节。

`ptr + total` 表示从缓冲区后面继续写，避免覆盖前面已经读到的数据。

为什么需要它：

```text
TCP 一次 recv 不保证读满目标长度。
```

## read_message

服务端读取一条完整消息：

```text
read_exact 读取 4 字节 net_length
  ↓
ntohl 转成本机 length
  ↓
检查 length 是否合法
  ↓
read_exact 按 length 读取 payload
  ↓
补 '\0'
  ↓
返回 payload 长度
```

长度检查：

```c
if (length == 0 || length > MAX_PAYLOAD_SIZE || length >= buffer_size)
```

避免非法长度导致缓冲区越界或服务端等待一个不合理的大包。

## send_message

客户端发送一条消息：

```text
strlen 计算 payload 长度
  ↓
htonl 转成网络字节序
  ↓
send 4 字节长度
  ↓
send payload
```

当前版本保持直观写法：

```c
send(fd, &net_length, sizeof(net_length), 0);
send(fd, message, length, 0);
```

今天没有继续扩展 `send_exact()`。

原因：

```text
Day28 的主目标是理解应用层协议和消息边界。
send_exact 属于健壮性增强，和服务端 read_exact 思路类似，可以在后续项目合并 epoll 时再补。
```

## server 流程

```text
create_server_socket
  ↓
accept 一个 client
  ↓
while (1)
  ↓
read_message
  ↓
n > 0：打印完整消息
n == 0：客户端断开
n < 0：读取失败或协议非法
```

今天 server 只处理一个 client。

这是为了把协议解析讲清楚，不把并发模型和协议解析混在一起。

## client 流程

```text
connect_to_server
  ↓
循环 fgets 读取终端输入
  ↓
quit 则退出
  ↓
send_message 发送 length + payload
```

数组传参：

```c
send_message(client_fd, message);
```

`message` 是字符数组，传参时会退化为 `&message[0]`，类型符合 `const char *`。

不能写：

```c
send_message(client_fd, &message);
```

因为 `&message` 是整个数组的地址，类型是 `char (*)[1024]`。

## 今日遇到的问题

1. `size_t total` 必须初始化为 `0`，否则循环条件使用随机值。
2. 条件判断要使用逻辑或 `||`，不是按位或 `|`。
3. `uint32_t` 是标准固定宽度类型，优先于 `u_int32_t`。
4. `accept` / `connect` 的地址参数应转成 `struct sockaddr *`。
5. `connect_to_server()` 成功后不能提前 `close(client_fd)`，否则返回的是已经关闭的 fd。
6. `inet_pton()` 出错时，`perror` 文案应写 `inet_pton`。
7. Makefile 中源文件路径必须写 `src/protocol_client.c`，不能只写 `protocol_client.c`。

## 今日完成状态

- [x] 理解 TCP 字节流不保留消息边界
- [x] 设计 `4 字节长度 + payload` 协议
- [x] 使用 `uint32_t` 固定长度字段
- [x] 使用 `htonl` / `ntohl` 处理网络字节序
- [x] 实现服务端 `read_exact`
- [x] 实现服务端 `read_message`
- [x] 实现客户端 `send_message`
- [x] 手动 gcc 编译 server/client
- [x] 编写 Makefile 自动构建
- [x] 明确 `send_exact` 属于后续健壮性增强，不在今天展开
