# Day 15 - fcntl 和非阻塞 IO

日期：2026-07-18

## 今日目标

- [x] 补回原计划中的 `fcntl` 和非阻塞 IO。
- [x] 理解阻塞 IO 会让程序卡在 `read`。
- [x] 使用 `fcntl` 获取文件状态标志。
- [x] 使用 `O_NONBLOCK` 把 stdin 设置成非阻塞。
- [x] 正确处理 `EAGAIN` / `EWOULDBLOCK`。
- [x] 输入 `quit` 后正常退出程序。
- [x] 编写 Makefile 构建项目。

## 项目目录

```text
linux_projects/day15_nonblock_io
```

## VS Code 打开命令

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day15_nonblock_io
code .
```

## 今日程序

程序名：

```text
nonblock_demo
```

运行方式：

```bash
./build/nonblock_demo
```

程序行为：

```text
没有输入时，read 不会卡住，程序继续循环。
有输入时，读取并打印输入内容。
输入 quit 时，程序退出。
```

## 阻塞 IO 现象

普通 `read`：

```c
n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
```

如果终端没有输入，程序会停在这一行。

现象：

```text
waiting input...
```

然后程序不继续打印 `loop`，直到用户输入内容并按回车。

这就是阻塞 IO：

```text
没有数据时，read 会等待。
等待期间，后面的代码不会执行。
```

## fcntl

头文件：

```c
#include <fcntl.h>
```

获取当前文件状态标志：

```c
flags = fcntl(STDIN_FILENO, F_GETFL);
```

含义：

```text
读取标准输入当前的 flags。
```

设置非阻塞：

```c
fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
```

含义：

```text
在原来的 flags 基础上，加上 O_NONBLOCK。
```

为什么是：

```c
flags | O_NONBLOCK
```

而不是直接设置成：

```c
O_NONBLOCK
```

原因：

```text
要保留原来的文件状态标志，只额外添加非阻塞属性。
```

## 非阻塞 read

设置 `O_NONBLOCK` 后：

```c
n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
```

没有输入时不会卡住，而是返回：

```text
n < 0
```

并且：

```c
errno == EAGAIN || errno == EWOULDBLOCK
```

处理方式：

```c
if (errno == EAGAIN || errno == EWOULDBLOCK) {
    printf("no input\n");
} else {
    perror("read");
    return 1;
}
```

含义：

```text
EAGAIN/EWOULDBLOCK 不是严重错误。
它表示当前没有数据可读，稍后再试。
```

## quit 退出

用户输入：

```text
quit
```

终端实际读到的内容可能是：

```text
quit\n
```

所以使用：

```c
strncmp(buffer, "quit", 4)
```

只比较前 4 个字符。

退出逻辑：

```c
if (strncmp(buffer, "quit", 4) == 0) {
    printf("quit command received\n");
    break;
}
```

## 今日遇到的问题

1. `sizeof(buffer-1)` 写错了。
2. `buffer[n] = '\n'` 写错了，应该是字符串结束符 `'\0'`。
3. Makefile 中把目标依赖写成了目标自己。
4. Makefile 中定义了 `src`，但一度使用了未定义的 `sr`。
5. 程序能跑，不代表 Makefile 依赖关系一定正确。

## 问题解释

### sizeof(buffer) - 1

正确：

```c
sizeof(buffer) - 1
```

含义：

```text
最多读取 buffer 大小减 1 个字节，留一个位置放字符串结束符。
```

错误：

```c
sizeof(buffer-1)
```

这里的 `buffer - 1` 是指针运算，不是数组大小减 1。

### 字符串结束符

读取后：

```c
buffer[n] = '\0';
```

作用：

```text
把 read 读到的字节数组变成 C 字符串，方便 printf("%s") 输出。
```

不能写成：

```c
buffer[n] = '\n';
```

否则字符串没有正确结束，输出可能混乱。

### 为什么 Makefile 错了程序也能跑

因为之前可能已经生成过：

```text
build/nonblock_demo
```

`make run` 只是运行已有可执行文件：

```bash
./build/nonblock_demo
```

所以程序能跑，不代表 Makefile 依赖写对了。

Makefile 的核心是：

```text
谁依赖谁
什么时候重新编译
怎么重新编译
```

正确依赖：

```makefile
$(ta): $(src) | $(bu)
```

表示：

```text
build/nonblock_demo 依赖 src/main.c。
src/main.c 变化后，make 应该重新编译。
```

## Makefile

常用命令：

```bash
make clean
make
make run
```

## 今日总结

今天我学会了：

1. 阻塞 `read` 会让程序停在读取位置。
2. 使用 `fcntl(F_GETFL)` 获取当前 flags。
3. 使用 `fcntl(F_SETFL, flags | O_NONBLOCK)` 设置非阻塞。
4. 非阻塞没有输入时，`read` 返回 `-1`，`errno` 是 `EAGAIN` 或 `EWOULDBLOCK`。
5. 使用 `strncmp(buffer, "quit", 4)` 处理带回车的输入。
6. Makefile 中依赖关系比“能运行”更重要。

今天还需要加强：

1. `fcntl` 还能修改哪些文件状态标志。
2. 非阻塞 IO 和后续 `select/poll/epoll` 的关系。
