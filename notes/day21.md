# Day21 IPC 基础

## 今日目标

学习 Linux 进程间通信 IPC 的基础方式：

- pipe
- FIFO
- 父子进程通信
- 两个独立程序通信

今天的项目模型：

```text
设备采集进程 -> IPC -> 主服务进程
```

这和后面的嵌入式设备网关有关：采集、协议处理、主控服务可能不在同一个进程里，需要 IPC 传递数据。

## 工程路径

代码路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day21_ipc_basic
```

WSL 路径：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day21_ipc_basic
```

VS Code 打开命令：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day21_ipc_basic
code .
```

## 今日文件

```text
src/pipe_demo.c
src/fifo_reader.c
src/fifo_writer.c
Makefile
```

## 编译运行

编译全部程序：

```bash
make clean
make
```

运行 pipe demo：

```bash
make runp
```

运行 FIFO 需要两个终端。

终端 1：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day21_ipc_basic
make runr
```

终端 2：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day21_ipc_basic
make runw
```

`make runr` 停住是正常现象，因为 FIFO reader 会等待 writer 打开写端。

## pipe

创建 pipe：

```c
int pipe_fd[2];
pipe(pipe_fd);
```

含义：

```text
pipe_fd[0]：读端
pipe_fd[1]：写端
```

父子进程通信流程：

```text
父进程创建 pipe
父进程 fork 子进程
子进程关闭读端，只写数据
父进程关闭写端，只读数据
父进程 waitpid 回收子进程
```

子进程只写，所以关闭：

```c
close(pipe_fd[0]);
```

父进程只读，所以关闭：

```c
close(pipe_fd[1]);
```

## 为什么要关闭不用的一端

如果不用的一端不关闭，程序可能无法正确判断通信结束。

例如父进程读取 pipe：

```c
ssize_t n = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
```

当所有写端都关闭后，`read` 才会返回：

```text
0
```

`read` 返回 0 表示：

```text
没有更多数据了，可以结束读取循环
```

如果某个写端还没关闭，父进程可能会继续阻塞等待。

## buffer[n] = '\0'

`read` 读取的是字节，不会自动添加字符串结束符。

所以每次读取后，如果要用 `%s` 打印，需要手动加：

```c
buffer[n] = '\0';
```

不能写成：

```c
buffer[128] = '\0';
```

因为数组：

```c
char buffer[128];
```

合法下标是：

```text
0 到 127
```

`buffer[128]` 是越界访问。

## FIFO

FIFO 也叫命名管道。

pipe 通常用于父子进程通信，FIFO 可以用于两个独立程序通信。

创建 FIFO：

```c
mkfifo(FIFO_PATH, 0666);
```

打开读端：

```c
open(FIFO_PATH, O_RDONLY);
```

打开写端：

```c
open(FIFO_PATH, O_WRONLY);
```

## mkfifo 错误处理

`mkfifo` 失败不一定要退出。

如果 FIFO 已经存在，`mkfifo` 会失败，并设置：

```text
errno = EEXIST
```

这时可以继续打开 FIFO。

严谨写法：

```c
if (mkfifo(FIFO_PATH, 0666) < 0) {
    if (errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }
}
```

含义：

```text
如果只是已经存在，可以继续
如果是其他错误，退出程序
```

## FIFO 路径问题

今天遇到：

```text
mkfifo: Operation not supported
```

原因：

```text
/mnt/c/... 是 WSL 挂载 Windows C 盘的路径
Windows 文件系统不支持 Linux FIFO 这种特殊文件
```

解决：

```c
#define FIFO_PATH "/tmp/day21_device_fifo"
```

也就是：

```text
代码和可执行文件可以放 /mnt/c
FIFO 特殊文件放 /tmp
```

## write 使用 strlen

当前 writer 中：

```c
const char *message[] = {
    "device data: temperature=33\n",
    "device data: humidity=58\n",
    "device data: voltage=24\n"
};
```

`message[i]` 是 `const char *` 指针。

所以不能用：

```c
sizeof(message[i])
```

因为这得到的是指针大小，通常是 8 字节。

应该用：

```c
strlen(message[i])
```

得到字符串实际长度。

## 今日总结

pipe 和 FIFO 都是字节流通信。

区别：

```text
pipe：适合父子进程等有亲缘关系的进程
FIFO：适合两个独立程序通过一个命名管道通信
```

今天需要记住：

```text
pipe_fd[0] 是读端
pipe_fd[1] 是写端
read 返回 0 表示对端写端关闭
FIFO reader/writer 单独打开时可能阻塞
WSL 的 /mnt/c 不适合创建 FIFO，要放 /tmp
```

## 今日完成状态

- [x] 实现 pipe 父子进程通信
- [x] 子进程连续写入多条设备数据
- [x] 父进程循环读取 pipe
- [x] 理解 `read` 返回 0
- [x] 实现 FIFO reader
- [x] 实现 FIFO writer
- [x] 解决 `/mnt/c` 不支持 FIFO 的问题
- [x] 编写 Makefile 编译三个程序
