# Day27 epoll 多客户端 TCP server

## 今日目标

在 Day25 `select`、Day26 `poll` 的基础上，使用 Linux 专属的 `epoll` 实现单线程多客户端 TCP echo server。

今天重点：

- 从零搭建 `epoll_server`
- 理解 `epoll_create1`、`epoll_ctl`、`epoll_wait`
- 把 `server_fd` 和 `client_fd` 注册进 epoll
- 区分监听 socket 事件和客户端 socket 事件
- 用一个线程处理多个 TCP client
- 补齐交互式 `tcp_client`
- 对比 `select`、`poll`、`epoll`

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day27_epoll_server
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day27_epoll_server
```

VS Code：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day27_epoll_server
code .
```

## 工程文件

```text
day27_epoll_server/
├── src/
│   ├── epoll_server.c
│   └── tcp_client.c
├── build/
│   ├── epoll_server
│   └── tcp_client
└── Makefile
```

## 编译运行

```bash
make clean
make
```

终端一：

```bash
./build/epoll_server
```

终端二和终端三：

```bash
./build/tcp_client
```

客户端启动后可以连续输入消息，输入 `quit` 退出当前客户端。

## epoll 服务端整体流程

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
  ↓
遍历 ready 个事件
  ↓
server_fd 事件：accept 新客户端，并 epoll_ctl ADD client_fd
client_fd 事件：recv / send，断开时 epoll_ctl DEL + close
```

## 创建监听 socket

监听 socket 仍然沿用 TCP server 的基本流程：

```text
socket
  ↓
setsockopt(SO_REUSEADDR)
  ↓
bind
  ↓
listen
  ↓
返回 server_fd
```

`server_fd` 的职责不是收发普通数据，而是接收新连接。

当 `server_fd` 在 epoll 中出现 `EPOLLIN` 事件时，含义是：

```text
有新的客户端连接到来，可以 accept
```

## 创建 epoll 实例

```c
int epoll_fd = epoll_create1(0);
```

它会在内核中创建一个 epoll 对象。可以理解为一个由内核维护的“监听表”。

返回的 `epoll_fd` 不是客户端连接，也不是服务器连接，而是用来操作这个 epoll 对象的文件描述符。

## 注册 server_fd

```c
struct epoll_event event;
memset(&event, 0, sizeof(event));

event.events = EPOLLIN;
event.data.fd = server_fd;

epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
```

含义：

```text
我要让 epoll 监听 server_fd
我关心的是可读事件 EPOLLIN
当事件发生时，请把 server_fd 放进返回结果里
```

`event.data.fd` 是自己保存的标记。后面 `epoll_wait()` 返回事件时，程序靠它判断发生事件的是哪个 fd。

## epoll_wait 等待事件

```c
struct epoll_event events[MAX_EVENTS];

int ready = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
```

参数含义：

```text
epoll_fd    epoll 对象
events      接收已发生事件的数组
MAX_EVENTS  一次最多取回多少个事件
-1          一直阻塞等待，不超时
```

返回值：

```text
ready > 0  有 ready 个事件发生
ready == 0 超时，本项目使用 -1 不会正常超时
ready < 0  调用出错
```

重点：

```c
for (int i = 0; i < ready; i++)
```

这里遍历的不是全部客户端，而是这次真正发生事件的 fd。

## 处理 server_fd 事件

```c
if (fd == server_fd) {
    int client_fd = accept(server_fd, ...);
}
```

`server_fd` 出现在事件列表中，说明有新连接到来。

服务端调用 `accept()` 后得到新的 `client_fd`。

随后必须把 `client_fd` 注册进 epoll：

```c
struct epoll_event client_event;
memset(&client_event, 0, sizeof(client_event));

client_event.events = EPOLLIN;
client_event.data.fd = client_fd;

epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
```

这样下一轮 `epoll_wait()` 才能监听这个客户端是否发来数据。

## 处理 client_fd 事件

```c
else {
    handle_client_event(epoll_fd, fd);
}
```

只要事件不是 `server_fd`，就说明它来自某个已连接客户端。

处理流程：

```text
recv
  ↓
n > 0：收到数据，send 回去
n == 0：客户端正常断开
n < 0：接收失败
```

客户端断开或出错时：

```c
epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
close(client_fd);
```

这两步要一起记：

```text
EPOLL_CTL_DEL  从 epoll 监听表中删除 fd
close          关闭 fd 本身
```

## server 端没有 connect 的原因

TCP server 和 client 的职责不同：

```text
server:
socket -> bind -> listen -> accept

client:
socket -> connect -> send -> recv
```

`connect()` 是客户端主动连接服务器使用的函数。

服务器不会主动 `connect` 客户端，而是等待客户端连接，然后用 `accept()` 接住这个连接。

`epoll` 不替代 `connect()`，它只负责通知程序：

```text
server_fd 有事件：有新连接
client_fd 有事件：客户端发数据或断开
```

## tcp_client 流程

客户端流程：

```text
socket
  ↓
填写服务器 IP 和端口
  ↓
inet_pton
  ↓
connect
  ↓
循环 fgets 读取终端输入
  ↓
send
  ↓
recv
  ↓
打印 echo
  ↓
quit 退出
```

发送时使用：

```c
send(client_fd, send_buffer, strlen(send_buffer), 0);
```

因为发送的是字符串中的有效内容，所以使用 `strlen`。

接收时使用：

```c
recv(client_fd, recv_buffer, sizeof(recv_buffer) - 1, 0);
```

因为接收要告诉系统缓冲区最多还能放多少字节，并且要预留一个字节给 `'\0'`。

## select / poll / epoll 对比

`select`：

```text
使用 fd_set
每一轮都要重新 FD_ZERO / FD_SET
select 返回后 fd_set 会被内核修改
需要 max_fd + 1
```

`poll`：

```text
使用 struct pollfd 数组
events 是程序关注的事件
revents 是内核返回的实际事件
不需要 max_fd
但仍然要扫描整个 pollfd 数组
```

`epoll`：

```text
先通过 epoll_ctl 把 fd 注册进内核
epoll_wait 只返回真正发生事件的 fd
程序只遍历 ready 个事件
适合大量 fd 的 Linux 网络服务器
```

三者共同点：

```text
都是 IO 多路复用
都可以用一个线程管理多个 fd
都不是让多个客户端真正并行执行 CPU 任务
```

## 今日遇到的问题

1. `error.h` 不是本项目需要的头文件，系统错误码应使用 `errno.h`。
2. `accept()` 的第二个参数应强转为 `struct sockaddr *`，不是 `struct sockaddr_in *`。
3. `epoll_ctl` 代码中不能混入中文符号，例如 `【`。
4. `recv_buffer[n] = '\0'` 前，`recv()` 长度应使用 `sizeof(buffer) - 1`。
5. `inet_pton()` 应判断 `<= 0`，因为 `0` 表示 IP 字符串格式错误。
6. 客户端断开或出错后，应从 epoll 删除 fd，再关闭 fd。

## 运行结果

服务端典型输出：

```text
epoll server listening on port 9000
client connected: fd5
[server] recv from fd=5 client1 hello
client connected: fd6
[server] recv from fd=6 client2 status
[server] recv from fd=5 client1 again
client disconnected: fd5
client disconnected: fd6
```

客户端典型输出：

```text
connected to server
>client1 hello
[echo] client1 hello
>client1 again
[echo] client1 again
>quit
```

## 今日完成状态

- [x] 从零完成 epoll TCP server
- [x] 创建并注册 `server_fd`
- [x] 使用 `epoll_wait` 等待事件
- [x] 使用 `accept` 接收新客户端
- [x] 把 `client_fd` 加入 epoll
- [x] 处理客户端 `recv` / `send`
- [x] 客户端断开时执行 `EPOLL_CTL_DEL` 和 `close`
- [x] 补齐交互式 TCP client
- [x] 编写双目标 Makefile
- [x] 理解 `select`、`poll`、`epoll` 的差异
