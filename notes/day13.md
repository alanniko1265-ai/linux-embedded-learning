# Day 13 - 信号基础：signal_guard

日期：2026-07-17

## 今日目标

- [x] 创建 `signal_guard` 项目。
- [x] 编写一个持续运行的主循环。
- [x] 理解 Ctrl+C 对应 `SIGINT`。
- [x] 使用 `sigaction` 注册信号处理函数。
- [x] 使用全局退出标志控制主循环结束。
- [x] 理解信号处理函数里不要做复杂操作。
- [x] 模拟服务程序运行时写日志。
- [x] 收到 Ctrl+C 后回到主流程清理资源。
- [x] 使用 Makefile 构建和运行项目。

## 项目目录

```text
linux_projects/day13_signal_guard
```

## VS Code 打开命令

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day13_signal_guard
code .
```

## 今日程序

程序名：

```text
signal_guard
```

运行方式：

```bash
./build/signal_guard
```

程序行为：

```text
每秒输出 running...
同时写入 signal_guard.log
按 Ctrl+C 后不直接乱退出
信号处理函数设置退出标志
主循环结束后打印退出信息并关闭日志文件
```

## Ctrl+C 和 SIGINT

在终端按：

```text
Ctrl+C
```

终端会给前台进程发送：

```text
SIGINT
```

默认情况下，程序收到 `SIGINT` 会直接终止。

Day13 的目标是让程序捕获 `SIGINT`，然后优雅退出。

## 退出标志

全局变量：

```c
static volatile sig_atomic_t g_stop = 0;
```

含义：

```text
g_stop = 0  继续运行
g_stop = 1  准备退出
```

为什么用 `sig_atomic_t`：

```text
它适合在信号处理函数和主流程之间传递简单标志。
```

为什么加 `volatile`：

```text
告诉编译器这个变量可能被正常流程之外的信号处理函数修改，
不要过度优化读取它的行为。
```

## 信号处理函数

```c
static void handle_sigint(int signo)
{
    (void)signo;
    g_stop = 1;
}
```

这里不直接 `printf`，也不直接 `fclose`。

原因：

```text
信号处理函数里应该尽量少做事。
只设置标志，真正的清理工作交给主流程。
```

`(void)signo;` 的作用：

```text
表示我知道这个参数存在，但当前不使用它。
避免编译器产生 unused parameter 警告。
```

## sigaction

头文件：

```c
#include <signal.h>
```

注册方式：

```c
struct sigaction sa;

sa.sa_handler = handle_sigint;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;

if (sigaction(SIGINT, &sa, NULL) < 0) {
    perror("sigaction");
    return 1;
}
```

含义：

```text
sa.sa_handler  指定收到 SIGINT 时调用哪个函数
sa.sa_mask     指定处理信号期间额外屏蔽哪些信号
sa.sa_flags    指定额外行为
```

相比 `signal()`：

```text
sigaction 行为更明确，更适合工程项目。
```

## 主循环

```c
while (!g_stop) {
    printf("running...%d\n", count);
    fprintf(log_file, "running...%d\n", count);
    fflush(log_file);
    count++;
    sleep(1);
}
```

逻辑：

```text
只要 g_stop 还是 0，就继续运行。
收到 Ctrl+C 后，handle_sigint 把 g_stop 改成 1。
下一轮循环判断时退出 while。
```

## 日志文件

打开日志：

```c
log_file = fopen("signal_guard.log", "a");
```

写日志：

```c
fprintf(log_file, "running...%d\n", count);
fflush(log_file);
```

关闭日志：

```c
fclose(log_file);
```

`fflush` 的作用：

```text
把缓冲区中的内容尽快写入文件。
```

这能减少程序异常退出时日志还停留在缓冲区里的概率。

## 优雅退出

退出时：

```c
if (g_stop) {
    printf("\nSIGINT received, stopping...\n");
}

printf("clean exit\n");
fclose(log_file);
```

意义：

```text
收到退出信号后，不在信号处理函数里直接清理资源。
而是让主循环自然结束，再统一关闭文件、打印日志。
```

这就是嵌入式 Linux 服务程序常见的优雅退出思路。

## Makefile

常用命令：

```bash
make clean
make
make run
make show-log
```

运行程序：

```bash
make run
```

按 `Ctrl+C` 结束后查看日志：

```bash
make show-log
```

清理：

```bash
make clean
```

## 今日遇到的问题

1. `count` 一开始没有初始化，导致输出值不确定。
2. `handle_sigint` 函数名一开始拼写错误。
3. `stopping` 写成了 `stipping`。
4. Makefile 中 `show-log` 一开始写成了 `slow-log`。
5. `.PHONY` 一开始写成了 `.PUHONY`。

## 今日总结

今天我学会了：

1. Ctrl+C 本质上是给程序发送 `SIGINT`。
2. 使用 `sigaction` 注册信号处理函数。
3. 信号处理函数里不要做复杂逻辑，只设置退出标志。
4. 主循环通过 `g_stop` 控制服务程序退出。
5. 退出前可以在主流程里关闭日志文件，完成资源清理。
6. 这类结构适合长期运行的嵌入式 Linux 后台服务。

今天还需要加强：

1. 哪些函数可以安全地在信号处理函数中调用。
2. `volatile sig_atomic_t` 的更深层含义。
3. 后续结合 `SIGTERM`，支持 systemd 或脚本停止服务。
