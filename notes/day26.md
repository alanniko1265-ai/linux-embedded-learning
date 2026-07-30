# Day26 poll 多客户端 TCP server

## 今日目标

在 Day25 `select` 服务器的基础上，使用 `poll` 实现单线程多客户端 TCP echo server。

今天重点：

- 从零完成 TCP 服务端监听 socket
- 使用 `struct pollfd` 管理监听 socket 和客户端 socket
- 理解 `events` 与 `revents`
- 使用 `poll()` 等待多个 fd
- 从零重写保持连接的交互式 TCP client
- 对比 `select` 与 `poll`

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day26_poll_server
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day26_poll_server
```

VS Code：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day26_poll_server
code .
```

## 工程文件

```text
day26_poll_server/
├── src/
│   ├── poll_server.c
│   └── tcp_client.c
├── build/
│   ├── poll_server
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
make runse
```

终端二和终端三：

```bash
make runc
```

客户端启动后可以连续输入消息，输入 `quit` 退出当前客户端。

## 创建监听 socket

服务端初始化流程：

```text
socket
  ↓
setsockopt(SO_REUSEADDR)
  ↓
填写 sockaddr_in
  ↓
bind
  ↓
listen
  ↓
返回 server_fd
```

`socket`：

```c
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
```

参数含义：

```text
AF_INET      IPv4
SOCK_STREAM  TCP 字节流
0            自动选择对应协议
```

`SO_REUSEADDR`：

```c
int reuse = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
           &reuse, sizeof(reuse));
```

它允许服务端重启后快速重新绑定刚使用过的端口。

绑定地址：

```c
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(SERVER_PORT);
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
```

`INADDR_ANY` 表示监听本机所有网络接口。

## pollfd 数组

```c
#define MAX_CLIENTS 16
#define POLL_FD_COUNT (MAX_CLIENTS + 1)

struct pollfd fds[POLL_FD_COUNT];
```

数组布局：

```text
fds[0]     server_fd
fds[1]     client1_fd
fds[2]     client2_fd
...
fds[16]    client16_fd
```

多出的一个位置专门保存 `server_fd`。

初始化：

```c
for (int i = 0; i < POLL_FD_COUNT; i++) {
    fds[i].fd = -1;
    fds[i].events = POLLIN;
    fds[i].revents = 0;
}

fds[0].fd = server_fd;
```

`poll()` 会忽略 `fd == -1` 的位置。

## events 与 revents

`struct pollfd` 的核心成员：

```c
struct pollfd {
    int fd;
    short events;
    short revents;
};
```

区别：

```text
fd       要观察的文件描述符
events   程序告诉内核希望观察什么事件
revents  poll 返回后，内核告诉程序实际发生了什么事件
```

设置：

```c
fds[i].events = POLLIN;
```

表示希望观察可读事件。

检查：

```c
if (fds[i].revents & POLLIN)
```

表示这个 fd 本轮实际发生了可读事件。

## poll 调用

```c
int ready = poll(fds, POLL_FD_COUNT, -1);
```

参数含义：

```text
fds            pollfd 数组
POLL_FD_COUNT  数组元素数量
-1             无限等待，直到至少一个 fd 发生事件
```

返回值：

```text
ready > 0  有 fd 就绪
ready == 0 超时，本项目使用 -1 不会正常超时
ready < 0  调用出错
```

## 接收新客户端

`fds[0]` 保存 `server_fd`：

```c
if (fds[0].revents & POLLIN) {
    int client_fd = accept(...);
}
```

监听 socket 出现 `POLLIN`，表示有新连接可以 `accept()`。

接收后寻找空位：

```c
for (int i = 1; i < POLL_FD_COUNT; i++) {
    if (fds[i].fd == -1) {
        fds[i].fd = client_fd;
        added = 1;
        break;
    }
}
```

如果没有空位：

```c
close(client_fd);
```

必须关闭刚接收的 socket，避免文件描述符泄漏。

## 处理客户端消息

从下标 `1` 开始遍历：

```c
for (int i = 1; i < POLL_FD_COUNT; i++)
```

因为 `fds[0]` 是监听 socket，不是普通客户端。

只处理出现 `POLLIN` 的客户端：

```c
if (!(fds[i].revents & POLLIN)) {
    continue;
}
```

接收结果：

```text
n > 0  收到数据，打印并发送回去
n == 0 客户端断开
n < 0  接收错误
```

客户端断开或通信失败后：

```c
close(client_fd);
fds[i].fd = -1;
```

这样该位置以后可以被新连接复用。

## 客户端连接流程

客户端从零完成：

```text
socket
  ↓
填写服务器 sockaddr_in
  ↓
inet_pton 转换 127.0.0.1
  ↓
connect
  ↓
返回 client_fd
```

服务端使用：

```c
htonl(INADDR_ANY)
```

客户端使用：

```c
inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr)
```

因为服务端监听本机接口，而客户端需要指定要连接的服务器地址。

## 客户端收发循环

```text
fgets 读取终端一行
  ↓
send 发送有效数据
  ↓
recv 等待 echo
  ↓
补字符串结束符
  ↓
打印结果
  ↓
不是 quit 就继续
```

退出判断：

```c
strcmp(send_buffer, "quit\n") == 0
```

`fgets()` 会保留用户按回车产生的 `'\n'`，因此比较内容中也要包含换行符。

## strlen 与 sizeof

发送时：

```c
send(client_fd, send_buffer, strlen(send_buffer), 0);
```

需要传入当前有效数据长度，所以使用 `strlen`。

接收时：

```c
recv(client_fd, recv_buffer, sizeof(recv_buffer) - 1, 0);
```

需要告诉系统目标数组最多能容纳多少字节，所以使用数组容量。

预留一个字节后补：

```c
recv_buffer[n] = '\0';
```

记忆：

```text
send：已有数据的实际长度
recv：目标缓冲区的最大容量
```

## select 与 poll 对比

`select`：

```text
使用 fd_set
每轮重新 FD_ZERO 和 FD_SET
select 返回后集合被修改
需要计算 max_fd
通过 FD_ISSET 判断事件
```

`poll`：

```text
使用 pollfd 数组
fd 和关注事件保存在数组中
不需要 max_fd
通过 revents 判断实际事件
fd 为 -1 时会被忽略
```

共同点：

```text
都是 IO 多路复用
都是一个线程等待多个 fd
返回后仍然要遍历所有候选 fd
都不是并行执行客户端业务
```

`poll` 取消了 `select` 使用位集合造成的固定编号范围限制，但每次仍需要把数组交给内核，并线性遍历事件。

## 今日遇到的问题

1. `fds[i].fd = -1` 是赋值，判断空位必须写 `fds[i].fd == -1`。
2. 客户端事件循环必须放在 `while` 内，否则正常情况下永远不会运行。
3. `events` 是程序设置的关注事件，`revents` 是内核返回的实际事件。
4. `if (send < 0)` 检查了函数名，应该检查返回值 `sent`。
5. `strcmp(send_buffer, "quit\n") == 0` 的比较必须写在函数调用外。
6. Makefile 的依赖变量必须使用已定义的 `$(sr1)`、`$(sr2)`，否则修改源码后可能不会重新编译。

## 运行结果

服务端典型输出：

```text
poll tcp server listen on port 9000
new client connected, fd=4
new client connected, fd=5
[fd=4] recv: hello from client 1
[fd=5] recv: hello from client 2
client disconnected, fd=4
client disconnected, fd=5
```

客户端典型输出：

```text
connected to 127.0.0.1:9000
enter message, or enter quit to exit
hello from client 1
[client] echo: hello from client 1
quit
[client] echo: quit
```

## 今日完成状态

- [x] 从零完成 TCP 监听 socket
- [x] 初始化 pollfd 数组
- [x] 使用 poll 等待多个 fd
- [x] 接收并保存多个客户端
- [x] 处理客户端消息和断开
- [x] 实现 TCP echo
- [x] 从零重写交互式 TCP client
- [x] 编写双目标 Makefile
- [x] 理解 select 与 poll 的差异
