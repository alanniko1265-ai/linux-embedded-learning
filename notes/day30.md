# Day30 epoll 多客户端请求-响应协议服务器

## 今日目标

把 Day27 的 `epoll` 多客户端服务器和 Day29 的 `length + payload` 请求-响应协议合并，完成一个单线程多客户端命令服务器雏形。

今天完成的模型：

```text
多个 client 同时连接 server
  ↓
server 使用 epoll_wait 等待事件
  ↓
client 发送 length + payload 请求
  ↓
server 读取完整请求并解析命令
  ↓
server 返回 length + payload 响应
```

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day30_epoll_protocol_server
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day30_epoll_protocol_server
```

VS Code：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day30_epoll_protocol_server
code .
```

## 工程文件

```text
day30_epoll_protocol_server/
├── src/
│   ├── server.c
│   └── client.c
├── build/
│   ├── server
│   └── client
└── Makefile
```

## 编译运行

手动编译：

```bash
mkdir -p build
gcc -Wall -g -O0 src/server.c -o build/server
gcc -Wall -g -O0 src/client.c -o build/client
```

Makefile 编译：

```bash
make clean
make
```

当前 Makefile 中：

```bash
make run2   # server
make run1   # client
```

## 今日架构

server 分为三层：

```text
socket 层：
    create_server_socket

协议层：
    read_exact
    send_exact
    read_message
    send_message

业务层：
    build_response
    handle_client_event
```

`epoll` 不负责解析协议，它只负责告诉程序哪个 fd 有事件。

真正的请求-响应由：

```text
handle_client_event
```

完成。

## 协议层

今天继续使用：

```text
+----------------+----------------+
| 4 字节长度字段  | payload 正文    |
+----------------+----------------+
```

`send_message`：

```text
strlen 得到 payload 长度
htonl 转网络字节序
send_exact 发送 4 字节长度
send_exact 发送 payload
```

`read_message`：

```text
read_exact 读取 4 字节长度
ntohl 转本机字节序
检查长度是否合法
read_exact 读取 payload
补 '\0'
返回 payload 长度
```

`read_exact` 和 `send_exact` 是为了保证：

```text
TCP 的一次 read/send 不完整时，继续读/继续发。
```

## epoll 主循环

server 启动后：

```text
create_server_socket
  ↓
epoll_create1
  ↓
epoll_ctl ADD server_fd
  ↓
while (1)
  ↓
epoll_wait
```

事件分发：

```text
fd == server_fd：
    accept 新客户端
    epoll_ctl ADD client_fd

fd != server_fd：
    handle_client_event(epoll_fd, fd)
```

## handle_client_event

对单个 client 执行一次请求-响应：

```text
read_message 读取请求
  ↓
build_response 生成响应
  ↓
send_message 发送响应
```

断开或错误时：

```c
epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
close(client_fd);
```

这表示：

```text
先从 epoll 监听表移除 fd
再关闭 fd
```

## 命令响应

server 支持：

```text
status   -> OK device=online temp=32 voltage=24
led on   -> OK led=on
led off  -> OK led=off
reboot   -> OK reboot scheduled
其他命令 -> ERR unknown command
```

命令比较中带 `\n`，因为 client 使用 `fgets()` 读取输入。

## 多客户端验证

server 输出示例：

```text
epoll protocol server listening on port 9000
client connected, fd=5
[fd=5] request: status
client connected, fd=6
[fd=6] request: led on
[fd=6] request: abc
[fd=5] request: reboot
client disconnected,fd=5
```

这说明：

```text
fd=5 和 fd=6 是两个不同 client
server 没有创建线程
epoll 能轮流处理两个 client 的请求
请求仍然按 length + payload 协议完整解析
```

## 今日遇到的问题

1. `epoll_fd` 是 `int` 文件描述符，不能 `memset(&epoll_fd, ...)`。
2. `struct epoll_event event` 才需要 `memset(&event, 0, sizeof(event))`。
3. `epoll_ctl ADD` 不能传 `NULL`，必须传带 `EPOLLIN` 和 `data.fd` 的事件结构。
4. `accept` 第三个参数必须是 `socklen_t *`，即 `&client_len`。
5. 注册 client fd 时必须设置 `client_event.events = EPOLLIN` 和 `client_event.data.fd = client_fd`。
6. `request` 本身带换行，server 打印时不要额外再加一个 `\n`。
7. Makefile 当前 `run2` 是 server，`run1` 是 client，运行时要注意顺序。

## 今日完成状态

- [x] 完成 epoll server 初始化
- [x] 注册 `server_fd` 到 epoll
- [x] 接收多个 client 并注册 `client_fd`
- [x] 将请求-响应协议接入 `handle_client_event`
- [x] 支持多个 client 轮流发送命令
- [x] 完成协议 client
- [x] 手动编译 server/client
- [x] 编写 Makefile
- [x] 完成多客户端联调
