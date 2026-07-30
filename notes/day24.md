# Day24 多客户端 TCP server

## 今日目标

在 Day23 TCP echo server 的基础上，升级为多客户端 TCP server。

今天重点：

- main 线程负责 `accept`
- worker 线程负责每个 client 的 `recv/send`
- 每个 client 使用独立线程处理
- 使用 `pthread_detach` 自动回收线程资源
- 理解 `malloc/free` 在多线程参数传递中的作用

## 工程路径

代码路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day24_multi_client_server
```

WSL 路径：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day24_multi_client_server
```

VS Code 打开命令：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day24_multi_client_server
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
make run-se
```

终端 2 发送消息：

```bash
make run-m1
make run-m2
make run-m3
```

也可以手动运行：

```bash
./build/tcp_client hello tcp
./build/tcp_client status
./build/tcp_client set led on
```

server 是长期运行程序，测试完用 `Ctrl+C` 停止。

## 项目模型

Day23 的 server 是：

```text
accept client1
处理 client1
client1 断开
再 accept client2
```

Day24 的 server 是：

```text
main 线程：一直 accept 新 client
worker 线程1：处理 client1
worker 线程2：处理 client2
worker 线程3：处理 client3
```

核心模型：

```text
main thread
  |
  | accept
  v
client_fd
  |
  | pthread_create
  v
worker thread
  |
  | recv/send
  v
处理某个 client
```

## ClientInfo

每个 client 的信息用结构体保存：

```c
struct ClientInfo {
    int client_fd;
    struct sockaddr_in client_addr;
};
```

字段含义：

- `client_fd`：和这个 client 通信的 socket
- `client_addr`：这个 client 的 IP 和端口

## client_thread

线程函数：

```c
static void *client_thread(void *arg)
```

`arg` 是 `pthread_create` 传进来的参数，需要转回：

```c
struct ClientInfo *client = (struct ClientInfo *)arg;
```

worker 线程流程：

```text
取出 client_fd
解析 client IP 和端口
循环 recv
收到数据后 send 原样返回
client 断开后 close client_fd
free client
线程退出
```

`recv` 返回值：

```text
n > 0：收到 n 个字节
n == 0：client 断开
n < 0：出错
```

## main 线程

main 线程完成监听准备：

```text
socket
setsockopt
bind
listen
```

然后进入循环：

```c
while (1) {
    struct ClientInfo *client = malloc(sizeof(struct ClientInfo));
    client->client_fd = accept(...);
    pthread_create(&tid, NULL, client_thread, client);
    pthread_detach(tid);
}
```

main 线程不负责和 client 通信，只负责不断接收新连接。

## 为什么使用 malloc

如果写成局部变量：

```c
struct ClientInfo client;
pthread_create(&tid, NULL, client_thread, &client);
```

会有风险：

```text
main 线程很快进入下一轮 while
client 变量可能被下一次连接覆盖
worker 线程拿到的数据可能已经变了
```

所以每个 client 单独申请一份堆内存：

```c
struct ClientInfo *client = malloc(sizeof(struct ClientInfo));
```

这样每个线程都有自己独立的 `ClientInfo`。

## malloc 和 free 的生命周期

这一句里有两个东西：

```c
struct ClientInfo *client = malloc(sizeof(struct ClientInfo));
```

区别：

```text
client 指针变量：局部变量，存在栈上
malloc 出来的 ClientInfo：存在堆上，需要手动 free
```

释放规则：

```text
accept 失败：main free
pthread_create 失败：main close client_fd，然后 free
pthread_create 成功：worker 线程负责 free
```

worker 线程结束时：

```c
close(client_fd);
free(client);
```

## pthread_detach

```c
pthread_detach(tid);
```

含义：

```text
线程结束后，系统自动回收线程资源
main 线程不需要 pthread_join
```

多客户端 server 中，client 数量不固定，不适合主线程一个个 `pthread_join`。

所以这里使用 `pthread_detach`。

## 内存泄漏监视

server 是长期运行程序，内存泄漏不会马上暴露，但会慢慢积累。

常见观察方式：

```bash
ps aux | grep tcp_server
```

查看某个进程内存：

```bash
ps -o pid,rss,vsz,cmd -p <pid>
```

查看 `/proc`：

```bash
cat /proc/<pid>/status | grep Vm
```

持续观察：

```bash
watch -n 1 "cat /proc/<pid>/status | grep Vm"
```

使用 Valgrind：

```bash
valgrind --leak-check=full ./build/tcp_server
```

重点看：

```text
definitely lost
```

这通常表示确定的内存泄漏。

## 今日遇到的问题

1. `recv` 需要 4 个参数，最后一个 flags 通常填 `0`。
2. `pthread_create` 成功返回 `0`，失败返回错误码，不能用 `< 0` 判断。
3. `accept` 失败后要 `free(client)`，否则泄漏。
4. `malloc` 失败不应该让长期 server 直接退出，可以 `continue`。
5. `malloc` 出来的堆内存不会自动释放，必须设计清楚谁负责 `free`。

## 今日完成状态

- [x] 定义 `ClientInfo`
- [x] 实现 `client_thread`
- [x] main 线程循环 `accept`
- [x] 每个 client 创建独立线程
- [x] 使用 `pthread_detach`
- [x] 正确处理 `malloc/free`
- [x] Makefile 加入 `-pthread`
- [x] 理解多客户端 TCP server 的基本模型
