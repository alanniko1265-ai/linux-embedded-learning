# Day22 local_command_server 小项目

## 今日目标

完成一个本地命令服务小项目，把 Week3 的内容合起来：

- FIFO IPC
- server 长期运行
- client 发送命令
- server 写日志
- `SIGINT` / `SIGTERM` 优雅退出

## 工程路径

代码路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day22_local_command_server
```

WSL 路径：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day22_local_command_server
```

VS Code 打开命令：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day22_local_command_server
code .
```

## 项目模型

```text
client
  |
  | 写入命令
  v
/tmp/day22_command_fifo
  |
  | server 读取命令
  v
server 打印命令并写入 logs/server.log
```

## 关键文件

```text
src/server.c
src/client.c
logs/server.log
Makefile
```

## 运行方式

编译：

```bash
make clean
make
```

终端 1 启动 server：

```bash
make run
```

终端 2 发送命令：

```bash
make send1
make send2
make send3
```

也可以直接运行：

```bash
./build/client status
./build/client set led on
./build/client reboot device
```

## FIFO 路径

FIFO 路径：

```c
#define FIFO_PATH "/tmp/day22_command_fifo"
```

原因：

```text
/mnt/c/... 是 Windows 文件系统挂载路径，不适合创建 FIFO 特殊文件
/tmp 是 WSL/Linux 原生路径，可以创建 FIFO
```

## server 的核心逻辑

server 做的事情：

```text
注册退出信号
创建 FIFO
打开日志文件
打开 FIFO 读端
保留一个 FIFO 写端 keep_fd
循环读取 client 命令
收到命令后打印并写日志
退出前关闭 fd 和日志文件
```

## signal 的作用

```c
signal(SIGINT, handle_signal);
signal(SIGTERM, handle_signal);
```

这不是强制中断程序，而是把退出请求转成状态变量：

```c
static volatile sig_atomic_t g_running = 1;
```

收到信号后：

```c
g_running = 0;
```

主循环：

```c
while (g_running) {
    ...
}
```

这样程序可以走到清理流程：

```text
写 server exit 日志
close fd
fclose log_file
```

## 为什么最开始收一条命令就退出

FIFO 有一个规则：

```text
当所有写端都关闭后，读端 read 会返回 0
```

最初流程：

```text
server 打开 FIFO 读端
client 打开 FIFO 写端
client 写入 status
client close 写端
server 下一次 read 发现所有写端关闭
read 返回 0
server break
server 退出
```

所以问题不是 server 主动想退出，而是它把 `read == 0` 理解为通信结束。

## keep_fd 的作用

为了解决 server 收一条命令就退出，server 自己保留一个 FIFO 写端：

```c
int read_fd = open(FIFO_PATH, O_RDONLY);
int keep_fd = open(FIFO_PATH, O_WRONLY);
```

其中：

```text
read_fd：真正读取 client 命令
keep_fd：server 自己保留的写端，不写数据，只防止所有写端关闭
```

这样 client 写完关闭后，FIFO 仍然有一个写端存在：

```text
client 写端关闭
server 的 keep_fd 还开着
read 不会因为“所有写端关闭”而返回 0
server 可以继续等待下一条命令
```

## client 的核心逻辑

client 支持命令行参数：

```bash
./build/client status
./build/client set led on
./build/client reboot device
```

程序通过：

```c
for (int i = 1; i < argc; i++) {
    write(fd, argv[i], strlen(argv[i]));
}
```

把参数写入 FIFO。

多个参数之间补空格：

```c
write(fd, " ", 1);
```

最后补换行：

```c
write(fd, "\n", 1);
```

## 今日遇到的问题

1. `fopen` 失败时不能 `fclose(NULL)`。
2. `write` 调用不能写成逗号表达式。
3. FIFO 中 client 写完关闭后，server 会因为 `read == 0` 退出。
4. 通过 `keep_fd` 保留写端，可以让 server 长期运行。
5. Makefile 里的目标名和实际可执行文件名要一致。

## 今日完成状态

- [x] 实现 server
- [x] 实现 client
- [x] client 支持命令行参数
- [x] server 通过 FIFO 接收命令
- [x] server 写日志到 `logs/server.log`
- [x] server 支持 Ctrl+C 优雅退出
- [x] 理解 `keep_fd` 为什么能防止 server 收一条命令就退出
