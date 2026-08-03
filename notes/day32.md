# Day32 设备网关加入日志模块

## 今日目标

在 Day31 的模块化设备网关基础上，新增一个独立的日志模块，让服务端运行时可以把关键事件写入文件：

```text
server starting
client connected
request received
response built
client disconnected
error event
```

这一天的重点不是重新写 TCP 协议，也不是重新写 epoll，而是练习真实项目里常见的“给已有服务增加辅助模块”。

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day32_device_gateway_logger
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day32_device_gateway_logger
```

VS Code 打开：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day32_device_gateway_logger
code .
```

## 工程结构

```text
day32_device_gateway_logger/
├── include/
│   ├── protocol.h
│   ├── command.h
│   └── logger.h
├── src/
│   ├── protocol.c
│   ├── command.c
│   ├── logger.c
│   ├── server.c
│   └── client.c
├── logs/
│   └── server.log
└── Makefile
```

## logger 模块接口

`include/logger.h`：

```c
int logger_init(const char *path);
void logger_info(const char *message);
void logger_error(const char *message);
void logger_close(void);
```

含义：

```text
logger_init   打开日志文件
logger_info   写 INFO 日志
logger_error  写 ERROR 日志
logger_close  关闭日志文件
```

`logger_init()` 返回 `int`，因为打开文件可能失败。成功返回 `0`，失败返回 `-1`。

## logger.c 核心设计

日志模块内部使用一个文件指针：

```c
static FILE *g_log_file = NULL;
```

`static` 的作用是限制作用域，让 `g_log_file` 只在 `logger.c` 内部可见。外部模块不能直接操作日志文件，只能通过 `logger_info()`、`logger_error()` 等接口使用日志模块。

日志写入流程：

```text
logger_info("message")
        |
        v
write_log("INFO", "message")
        |
        v
生成时间字符串
        |
        v
fprintf 写入 logs/server.log
        |
        v
fflush 立即刷新到文件
```

`fflush()` 很重要，因为服务端程序通常长期运行。如果不刷新，日志可能暂时停留在缓冲区，打开文件时看不到最新内容。

## server.c 接入点

服务端启动时：

```c
if (logger_init("logs/server.log") < 0) {
    return 1;
}

logger_info("server starting");
```

客户端连接时：

```c
printf("client connected, fd=%d\n", client_fd);
logger_info("client connected");
```

收到请求时：

```c
char log_message[MAX_PAYLOAD_SIZE + 64];
snprintf(log_message, sizeof(log_message), "request received: %s", request);
logger_info(log_message);
```

生成响应时：

```c
snprintf(log_message, sizeof(log_message), "response built: %s", response);
logger_info(log_message);
```

客户端断开时：

```c
logger_info("client disconnected");
```

出错路径上：

```c
logger_error("read request failed");
logger_error("send response failed");
```

## Makefile 修改

server 需要链接日志模块：

```makefile
src1 := src/server.c src/protocol.c src/command.c src/logger.c
```

client 暂时不需要日志模块：

```makefile
src2 := src/client.c src/protocol.c
```

运行 server 前确保日志目录存在：

```makefile
run1:$(ta1)
	mkdir -p logs
	./$(ta1)
```

## 运行方式

编译：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day32_device_gateway_logger
make clean
make
```

终端 1 启动服务端：

```bash
make run1
```

终端 2 启动客户端：

```bash
make run2
```

客户端输入：

```text
status
led on
abc
quit
```

查看日志：

```bash
cat logs/server.log
```

期望看到类似内容：

```text
[2026-07-31 14:38:08] [INFO] server starting
[2026-07-31 14:38:36] [INFO] client connected
[2026-07-31 14:38:41] [INFO] request received: status
[2026-07-31 14:38:41] [INFO] response built: OK device=online temp=32 voltage=24
[2026-07-31 14:38:41] [INFO] request received: led on
[2026-07-31 14:38:41] [INFO] response built: OK led=on
[2026-07-31 14:38:41] [INFO] request received: abc
[2026-07-31 14:38:41] [INFO] response built: ERR unknown command
[2026-07-31 14:38:46] [INFO] client disconnected
```

## 今日易错点

1. `logger_info` 拼写要和头文件声明一致。
2. `write_log()` 是 `void` 函数，不能 `return -1`。
3. `localtime()` 需要传入 `time_t` 变量的地址：`localtime(&now)`。
4. `FILE *` 要用 `fclose()` 关闭，不能用 `close()`。
5. `server.c` 调用了 logger 函数后，Makefile 必须把 `src/logger.c` 编译进 server。
6. `logs/server.log` 的父目录 `logs` 必须存在，否则 `fopen("logs/server.log", "a")` 会失败。
7. `response built` 比 `response sent` 更准确，因为日志写入发生在 `send_message()` 前。

## 今日完成状态

- [x] 复用 Day31 的 protocol / command / server / client 基础代码
- [x] 新增 logger.h
- [x] 新增 logger.c
- [x] 在 server.c 中初始化日志模块
- [x] 记录 server 启动事件
- [x] 记录 client connected / disconnected
- [x] 记录 request received
- [x] 记录 response built
- [x] 在错误路径记录 ERROR 日志
- [x] 修改 Makefile 链接 logger.c
- [x] 通过 logs/server.log 验证日志输出

## 今日总结

Day32 开始把设备网关从“能通信”推进到“可观察”。日志不是业务本身，但它是实际项目排查问题、定位异常、复盘运行过程的基础能力。

从今天开始，服务端不再只是终端里看到几行输出，而是能把运行过程沉淀到文件中。这也是后续嵌入式 Linux 应用开发里非常常见的能力：设备长期运行时，必须有日志帮助开发者判断它到底发生过什么。
