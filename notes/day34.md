# Day34 设备网关优雅退出

## 今日目标

在 Day33 的设备网关基础上加入信号处理，让 server 收到 `Ctrl+C` 或终止信号时，不是直接被打断，而是主动跳出主循环并执行清理流程。

核心流程：

```text
Ctrl+C / SIGTERM
        |
        v
handle_signal()
        |
        v
g_running = 0
        |
        v
epoll_wait 被信号打断
        |
        v
while(g_running) 结束
        |
        v
logger_info("server stopped")
close(epoll_fd)
close(server_fd)
logger_close()
```

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day34_device_gateway_graceful_shutdown
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day34_device_gateway_graceful_shutdown
```

VS Code 打开：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day34_device_gateway_graceful_shutdown
code .
```

## 新增头文件

```c
#include <signal.h>
#include <errno.h>
```

`signal.h` 用于 `signal()`、`SIGINT`、`SIGTERM`、`sig_atomic_t`。

`errno.h` 用于判断 `epoll_wait()` 是否被信号打断。

## 运行标志

```c
static volatile sig_atomic_t g_running = 1;
```

含义：

```text
g_running = 1  server 继续运行
g_running = 0  server 准备退出
```

`sig_atomic_t` 适合在信号处理函数中读写。`volatile` 表示这个变量可能被正常执行流之外的信号处理函数修改。

## 信号处理函数

```c
static void handle_signal(int signo)
{
    (void)signo;
    g_running = 0;
}
```

`(void)signo;` 表示参数暂时不用，避免 `unused parameter` 警告。

信号处理函数里只修改标志位，不直接写日志、不关闭 socket。真正的清理动作放在主循环结束后的正常代码里。

## 注册信号

在 `main()` 开始处：

```c
signal(SIGINT, handle_signal);
signal(SIGTERM, handle_signal);
```

含义：

```text
SIGINT   通常来自 Ctrl+C
SIGTERM  通常来自 kill、脚本停止、服务管理器停止
```

## 主循环修改

原来：

```c
while (1) {
```

改成：

```c
while (g_running) {
```

这样 server 不再是无条件死循环，而是受退出标志控制。

## EINTR 处理

```c
if (ready < 0) {
    if (errno == EINTR) {
        continue;
    }

    perror("epoll_wait");
    break;
}
```

当 `Ctrl+C` 到来时，`epoll_wait()` 可能被信号打断并返回 `-1`，这时 `errno == EINTR`。

这个情况不是服务端故障，而是正常退出流程的一部分，所以这里 `continue` 回到 `while(g_running)` 判断。因为 `g_running` 已经被改成 `0`，循环自然结束。

## 清理流程

循环结束后执行：

```c
logger_info("server stopped");
close(epoll_fd);
close(server_fd);
logger_close();
return 0;
```

这说明程序确实走到了正常退出路径。

## 运行验证

编译：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day34_device_gateway_graceful_shutdown
make clean
make
```

终端 1：

```bash
make run1
```

终端 2：

```bash
make run2
```

客户端输入：

```text
status
quit
```

回到 server 终端，按：

```text
Ctrl+C
```

查看日志：

```bash
cat logs/server.log
```

实际验证结果：

```text
[2026-08-03 14:02:16] [INFO] server starting
[2026-08-03 14:02:16] [INFO] server config loaded
[2026-08-03 14:02:25] [INFO] client connected
[2026-08-03 14:02:28] [INFO] request received: status
[2026-08-03 14:02:28] [INFO] response built: OK device=online temp=32 voltage=24
[2026-08-03 14:02:29] [INFO] client disconnected
[2026-08-03 14:02:34] [INFO] server stopped
```

最关键的是：

```text
server stopped
```

它证明 `Ctrl+C` 后 server 走到了正常清理代码。

## 今日易错点

1. 只定义 `handle_signal()` 不够，必须用 `signal()` 注册。
2. `while(1)` 必须改成 `while(g_running)`。
3. `epoll_wait()` 被信号打断时会返回 `-1`。
4. `errno == EINTR` 表示系统调用被信号中断。
5. 信号处理函数里不要写复杂逻辑。
6. 清理动作应该放在主循环退出后的正常路径中。

## 今日完成状态

- [x] 引入 `signal.h`
- [x] 引入 `errno.h`
- [x] 新增 `g_running`
- [x] 新增 `handle_signal()`
- [x] 注册 `SIGINT`
- [x] 注册 `SIGTERM`
- [x] 将 `while(1)` 改成 `while(g_running)`
- [x] 处理 `epoll_wait()` 的 `EINTR`
- [x] 验证 `Ctrl+C` 后写入 `server stopped`

## 今日总结

Day34 让设备网关具备了“可停止”的服务形态。

之前 server 能运行、能通信、能写日志、能读配置，但没有干净的退出路径。今天加入信号处理后，程序可以响应外部停止请求，然后自己关闭资源、记录日志、退出进程。

这一步对嵌入式 Linux 应用很重要。设备上的服务通常会被脚本、看门狗、systemd 或维护人员停止和重启。能不能优雅退出，决定了日志是否完整、端口是否释放、文件是否正确关闭。
