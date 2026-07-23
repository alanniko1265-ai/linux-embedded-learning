# Day23 TCP echo server 基础

## 今日目标

进入 Linux 网络编程，完成一个 TCP echo demo：

- server 监听 TCP 端口
- client 连接 server
- client 发送消息
- server 接收后原样返回
- client 打印 echo 结果
- server 可以连续处理多个 client

## 工程路径

代码路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day23_tcp_echo
```

WSL 路径：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day23_tcp_echo
```

VS Code 打开命令：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day23_tcp_echo
code .
```

## 今日文件

```text
src/tcp_server.c
src/tcp_client.c
Makefile
```

## 编译运行

编译：

```bash
make clean
make
```

终端 1 启动 server：

```bash
make run1
```

终端 2 发送消息：

```bash
make run2
make run3
make run4
```

也可以直接运行：

```bash
./build/tcp_client hello tcp
./build/tcp_client status
./build/tcp_client set led on
```

## server 流程

server 的整体流程：

```text
socket
setsockopt
bind
listen
accept
recv
send
close
```

创建 TCP socket：

```c
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
```

含义：

```text
AF_INET：IPv4
SOCK_STREAM：TCP
0：默认协议
```

设置端口复用：

```c
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
```

作用是避免 server 刚退出后立刻重启时容易遇到：

```text
Address already in use
```

绑定地址：

```c
bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
```

当前绑定的是：

```text
0.0.0.0:9000
```

监听连接：

```c
listen(server_fd, 5);
```

等待 client：

```c
accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
```

`accept` 会阻塞，直到有 client 连接。

## server_fd 和 client_fd

`server_fd` 是监听 socket：

```text
负责监听端口，等待新连接
```

`client_fd` 是连接 socket：

```text
负责和某一个 client 收发数据
```

所以处理完一个 client 后：

```c
close(client_fd);
```

但 server 还要继续监听，就不能关闭 `server_fd`。

## 连续处理多个 client

最初版本只 `accept` 一次，所以处理一个 client 后 server 就退出。

后来改成外层循环：

```c
while (1) {
    int client_fd = accept(...);
    ...
    close(client_fd);
}
```

这样 server 可以：

```text
接收 client1
处理 client1
关闭 client1
继续等待 client2
```

## client 流程

client 的整体流程：

```text
检查参数
socket
准备 server 地址
connect
send
recv
close
```

连接本机 server：

```c
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000
```

`127.0.0.1` 表示本机回环地址。

把字符串 IP 转换成网络地址：

```c
inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
```

连接 server：

```c
connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
```

如果 server 没有先启动，`connect` 会失败。

## send 和 write 的区别

FIFO 中使用：

```c
write(fd, buffer, len);
```

socket 中可以使用：

```c
send(client_fd, buffer, len, 0);
```

`send` 第四个参数是 flags：

```text
0 表示默认发送方式
```

所以：

```c
send(client_fd, "\n", 1, 0);
```

比 `write` 多一个 socket 发送选项参数。

## TCP 是字节流

今天观察到 server 可能输出：

```text
[server] recv: hello
[server] recv:  tcp
```

原因：

```text
TCP 是字节流，不保留消息边界
```

client 多次 `send`：

```text
send "hello"
send " "
send "tcp"
send "\n"
```

server 的一次 `recv` 不保证刚好收到一整条消息。

TCP 只保证：

```text
字节顺序正确
数据可靠传输
```

不保证：

```text
一次 send 对应一次 recv
```

这就是后面要学习协议解析的原因。

## 一次组包发送

为了让今天的输出更清楚，client 改成先拼接消息：

```c
char send_buffer[BUFFER_SIZE];
send_buffer[0] = '\0';
```

把命令行参数拼成：

```text
set led on\n
```

再一次性发送：

```c
send(client_fd, send_buffer, strlen(send_buffer), 0);
```

注意：即使一次 `send`，工程上也不能假设 server 一定一次 `recv` 完整收到。真正可靠的做法还是设计协议格式。

## 今日遇到的问题

1. client 最初从 `argv[0]` 开始发送，导致程序名也被发给 server。
2. TCP 多次 `send` 后，server 可能多次 `recv` 才读完整条消息。
3. server 最初只 `accept` 一次，处理一个 client 后就退出。
4. `send` 比 `write` 多一个 flags 参数。
5. 打印时如果 buffer 自带 `\n`，`printf` 再加 `\n` 会多空行。

## 今日完成状态

- [x] 实现 TCP server
- [x] 实现 TCP client
- [x] client 连接 `127.0.0.1:9000`
- [x] server 接收消息并 echo 返回
- [x] server 支持连续处理多个 client
- [x] client 改成先组包再发送
- [x] 理解 TCP 是字节流，不保证消息边界
