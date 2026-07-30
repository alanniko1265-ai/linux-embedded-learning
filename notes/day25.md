# Day25 select 多客户端 TCP server

## 今日目标

在 Day24 多线程 TCP server 的基础上，使用 `select` 实现单线程管理多个客户端。

今天重点：

- 使用 `fd_set` 保存需要监听的文件描述符
- 同时监听 `server_fd` 和多个 `client_fd`
- 使用 `FD_ISSET` 判断哪个 fd 发生了事件
- 理解 `select` 会修改传入的 fd 集合
- 对比阻塞、一个客户端一个线程和 IO 多路复用三种模型

## 工程路径

代码路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day25_select_server
```

WSL 路径：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day25_select_server
```

VS Code 打开命令：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day25_select_server
code .
```

## 今日文件

```text
src/select_server.c
src/tcp_client.c
Makefile
```

## 编译运行

编译：

```bash
make clean
make
```

终端一启动服务端：

```bash
make runs
```

终端二和终端三分别启动客户端：

```bash
make runm1
make runm2
```

客户端启动后手动输入消息，输入 `quit` 退出当前客户端。

## 原始阻塞模型

原始 TCP server 的运行方式：

```text
accept client1
recv 等待 client1
send 回复 client1
client1 断开
再次 accept client2
```

如果 client1 一直不发送数据，server 会阻塞在 `recv`，不能处理 client2。

## Day24 多线程模型

Day24 为每个客户端创建一个线程：

```text
main 线程：accept 新连接
worker 线程1：处理 client1
worker 线程2：处理 client2
worker 线程3：处理 client3
```

某个线程阻塞在 `recv` 时，不会阻止其他线程工作，但客户端越多，线程也越多。

## Day25 select 模型

Day25 只有一个主线程：

```text
构造 fd 集合
    |
    v
select 等待任意 fd 就绪
    |
    +-- server_fd 就绪：accept 新客户端
    |
    +-- client_fd 就绪：recv 客户端数据
    |
    v
处理事件后进入下一轮
```

`select` 没有改变 TCP 的通信方式，仍然使用：

```text
socket -> bind -> listen -> accept -> recv/send
```

它改变的是服务器等待多个 fd 的方式。

## clients 数组

```c
int clients[MAX_CLIENTS];
```

数组保存当前已经连接的 `client_fd`。

初始化时：

```c
clients[i] = -1;
```

`-1` 表示当前位置没有客户端。

新客户端连接后，`add_client` 找到一个值为 `-1` 的位置保存 `client_fd`。

客户端断开后，`remove_client` 完成：

```c
close(clients[index]);
clients[index] = -1;
```

## 每轮重新构造 fd_set

```c
fd_set read_fps;
FD_ZERO(&read_fps);
FD_SET(server_fd, &read_fps);
```

然后加入所有有效客户端：

```c
for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i] != -1) {
        FD_SET(clients[i], &read_fps);
    }
}
```

必须在每轮循环重新构造，因为 `select` 返回时会直接修改 `read_fps`：

```text
调用前：集合中是所有需要观察的 fd
返回后：集合中只保留本轮已经就绪的 fd
```

如果下一轮不重新加入，上一轮没有事件的 fd 就不会继续被监听。

## max_fd

```c
int max_fd = server_fd;
```

遍历客户端时找出最大的 fd：

```c
if (clients[i] > max_fd) {
    max_fd = clients[i];
}
```

调用：

```c
select(max_fd + 1, &read_fps, NULL, NULL, NULL);
```

第一个参数表示要检查的 fd 范围是：

```text
0 到 max_fd
```

因此需要传入 `max_fd + 1`。

后面三个 `NULL` 分别表示暂时不监听：

```text
可写事件
异常事件
超时时间
```

最后一个参数为 `NULL`，所以 `select` 会一直等待，直到某个 fd 就绪。

## 判断 server_fd

```c
if (FD_ISSET(server_fd, &read_fps)) {
    int client_fd = accept(...);
}
```

监听 socket 的可读含义是：

```text
有新的客户端连接可以 accept
```

它不是普通的客户端消息。

## 判断 client_fd

```c
if (!FD_ISSET(client_fd, &read_fps)) {
    continue;
}
```

如果某个 `client_fd` 本轮没有就绪，就跳过它。

如果就绪，则调用：

```c
ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
```

返回值含义：

```text
n > 0：收到数据
n == 0：客户端正常断开
n < 0：接收出错
```

收到数据后：

```c
buffer[n] = '\0';
printf("[fd=%d] recv: %s", client_fd, buffer);
send(client_fd, buffer, n, 0);
```

server 把消息原样发送回客户端。

## 完整服务端流程

```text
初始化 clients[]
创建 server_fd
bind 9000
listen
进入 while
重新构造 read_fps
加入 server_fd
加入所有 client_fd
计算 max_fd
调用 select
检查 server_fd 是否就绪
有新连接则 accept 并保存
遍历所有 client_fd
只处理 FD_ISSET 为真的客户端
recv 数据或处理断开
send 回显
回到下一轮
```

## 客户端流程

今天的客户端是保持连接的交互版本：

```text
socket
填写 server_addr
connect
fgets 等待终端输入
send 发送消息
recv 等待回显
打印回显
继续等待下一条消息
输入 quit 后 close
```

它与之前一次发送一条消息就退出的客户端不同，可以更清楚地观察多个客户端同时保持连接。

## 运行结果

服务端典型输出：

```text
select tcp server listen on port 9000
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
enter a message, or enter quit to exit
hello from client 1
[client] echo: hello from client 1
quit
[client] echo: quit
```

## select 的限制

`select` 可以完成多客户端并发管理，但仍有局限：

- 单线程处理事件，不是真正的并行执行
- 每轮都要重新构造 `fd_set`
- 每轮都要遍历客户端数组
- 可监听的 fd 数量受到 `FD_SETSIZE` 限制
- 某个客户端处理时间过长，仍会耽误其他客户端

后续学习 `poll` 和 `epoll`，就是继续改进这种 IO 多路复用模型。

## 今日遇到的问题

1. `for` 三个部分之间必须使用分号，不能把第二个分号写成逗号。
2. `client_fd == -1` 是比较，`client_fd = -1` 是赋值。
3. `recv` 有四个参数，最后一个 flags 使用默认值 `0`。
4. 接收后要写 `buffer[n] = '\0'`，才能作为 C 字符串打印。
5. 应检查 `sent < 0`，不能把函数名 `send` 当成返回值。
6. Makefile 给交互式客户端附加的 `hello`、`linux` 参数不会被 `main(void)` 接收，消息需要在启动后输入。

## 今日完成状态

- [x] 创建并监听 TCP server socket
- [x] 使用数组保存多个 client_fd
- [x] 每轮重新构造 fd_set
- [x] 使用 select 等待多个 fd
- [x] 接收新的客户端连接
- [x] 处理客户端消息与断开
- [x] 向客户端发送 echo
- [x] 使用两个客户端验证多连接
- [x] 理解 select 和多线程模型的区别
