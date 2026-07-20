# Day 14 - 日志模块：logger

日期：2026-07-18

## 今日目标

- [x] 创建 `logger` 模块项目。
- [x] 自己写 `include/logger.h` 对外接口。
- [x] 自己写 `src/logger.c` 实现日志模块。
- [x] 使用 `FILE *` 保存日志文件资源。
- [x] 实现 `logger_init` 和 `logger_close`。
- [x] 实现 `logger_info`、`logger_warn`、`logger_error`。
- [x] 使用内部函数 `write_log` 减少重复代码。
- [x] 给日志增加时间戳。
- [x] 编写 Makefile 构建多文件项目。

## 项目目录

```text
linux_projects/day14_logger_module
```

## VS Code 打开命令

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day14_logger_module
code .
```

## 项目结构

```text
day14_logger_module
├── Makefile
├── include
│   └── logger.h
├── logs
│   └── app.log
└── src
    ├── logger.c
    └── main.c
```

## 模块边界

今天开始做的是完整项目后面会复用的日志模块。

文件职责：

```text
logger.h   对外接口，告诉 main.c 可以调用哪些日志函数。
logger.c   日志模块内部实现，负责打开文件、写日志、关闭文件。
main.c     业务流程，只调用 logger_xxx，不直接操作日志文件。
```

## logger.h

对外接口：

```c
int logger_init(const char *path);
void logger_info(const char *message);
void logger_warn(const char *message);
void logger_error(const char *message);
void logger_close(void);
```

含义：

```text
logger_init   初始化日志模块，打开日志文件
logger_info   写 INFO 日志
logger_warn   写 WARN 日志
logger_error  写 ERROR 日志
logger_close  关闭日志文件
```

## FILE 指针

logger.c 内部保存：

```c
static FILE *g_log_file = NULL;
```

含义：

```text
g_log_file 是日志文件的操作入口。
```

它不是文件内容本身，而是指向 C 标准库内部文件对象的指针。

`static` 的作用：

```text
让 g_log_file 只在 logger.c 内部可见。
外部 main.c 不能直接访问它，只能通过 logger_init/logger_close 等接口操作。
```

## logger_init

```c
int logger_init(const char *path)
{
    g_log_file = fopen(path, "a");
    if (g_log_file == NULL) {
        perror("fopen");
        return -1;
    }

    return 0;
}
```

返回值约定：

```text
0   成功
-1  失败
```

`"a"` 表示追加写入：

```text
文件不存在则创建。
文件存在则在末尾继续追加。
```

## logger_close

```c
void logger_close(void)
{
    if (g_log_file != NULL) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
}
```

关闭后把指针设为 `NULL`，可以避免后续误用旧指针。

## 日志级别

今天实现了三个级别：

```text
INFO   普通运行信息
WARN   警告，程序还能继续
ERROR  错误，某个操作失败
```

对外函数：

```c
void logger_info(const char *message)
{
    write_log("INFO", message);
}

void logger_warn(const char *message)
{
    write_log("WARN", message);
}

void logger_error(const char *message)
{
    write_log("ERROR", message);
}
```

## 内部函数 write_log

`write_log` 是 logger.c 内部函数：

```c
static void write_log(const char *level, const char *message)
```

它负责统一完成：

```text
检查日志文件是否打开
获取当前时间
格式化时间字符串
写入 [时间] [级别] 消息
fflush 刷新日志文件
```

这样 `logger_info/warn/error` 不需要重复写 `fprintf/fflush`。

## 时间戳

使用头文件：

```c
#include <time.h>
```

核心代码：

```c
time_t now;
struct tm *tm_info;
char time_text[32];

now = time(NULL);
tm_info = localtime(&now);
strftime(time_text, sizeof(time_text), "%Y-%m-%d %H:%M:%S", tm_info);
```

最终日志格式：

```text
[2026-07-18 10:20:11] [INFO] logger module started
[2026-07-18 10:20:11] [WARN] device temperature is close to limit
[2026-07-18 10:20:11] [ERROR] sensor read failed simulated
```

## 编译和运行

```bash
make clean
make
make run
make show-log
```

`make run`：

```bash
./build/logger_demo
```

`make show-log`：

```bash
cat logs/app.log
```

## 今日遇到的问题

1. `logger_info` 一开始把判断写反了。
2. `logger_init` 失败返回值一开始写成 `1`，后来改成 `-1`。
3. `logger_warn` 中日志级别一开始写成 `WRAN`。
4. Makefile 中 `logger.c` 路径一开始写错，实际应该是 `src/logger.c`。
5. 编译时找不到 `logger.h`，原因是缺少 `-Iinclude`。
6. Makefile 中定义了 `sr`，但规则里误写成 `$(src)`。

## 问题解释

### logger_info 判断为什么是 g_log_file == NULL

正确逻辑：

```c
if (g_log_file == NULL) {
    return;
}
```

含义：

```text
如果日志文件没打开，就不写日志。
如果日志文件已经打开，才继续 fprintf。
```

不能写成：

```c
if (g_log_file != NULL) {
    return;
}
```

否则文件打开后反而直接返回，日志不会写入。

### 为什么要用 -Iinclude

`main.c` 中写：

```c
#include "logger.h"
```

而头文件实际在：

```text
include/logger.h
```

所以编译时需要：

```bash
-Iinclude
```

告诉 gcc 去 `include` 目录查找头文件。

### 为什么源文件要写 src/logger.c

实际文件位置：

```text
src/logger.c
```

所以 Makefile 中应该写：

```makefile
sr := src/main.c src/logger.c
```

不能只写：

```makefile
logger.c
```

## 今日总结

今天我学会了：

1. 把日志功能封装成独立模块。
2. 用 `.h` 暴露接口，用 `.c` 隐藏实现细节。
3. 使用 `FILE *` 保存文件资源入口。
4. 使用 `static` 限制模块内部变量和内部函数的作用范围。
5. 用 `time/localtime/strftime` 生成时间戳。
6. 用 Makefile 构建 `main.c + logger.c` 多文件项目。

今天还需要加强：

1. Makefile 变量名必须完全一致。
2. 模块接口和模块内部状态的边界。
3. 后续完整项目中复用 logger 模块。
