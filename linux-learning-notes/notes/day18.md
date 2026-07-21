# Day 18 - Week2 综合小项目：file_monitor_tool

日期：2026-07-21

## 今日目标

- [x] 完成 Week2 综合小项目 `file_monitor_tool`。
- [x] 读取配置文件 `config/monitor.conf`。
- [x] 解析 `monitor_path`、`log_path`、`interval_sec`。
- [x] 使用 `stat` 获取被监控文件大小。
- [x] 周期性检查文件大小变化。
- [x] 文件大小变化时输出提示。
- [x] 将监控结果写入日志文件。
- [x] 使用 `sigaction` 支持 Ctrl+C 优雅退出。
- [x] 退出前关闭日志文件。
- [x] 编写 Makefile 构建、运行、追加测试数据、查看日志。

## 项目目录

```text
linux_projects/day18_file_monitor_tool
```

## VS Code 打开命令

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day18_file_monitor_tool
code .
```

## 项目结构

```text
day18_file_monitor_tool
├── Makefile
├── config
│   └── monitor.conf
├── data
│   └── watch.txt
├── logs
│   └── monitor.log
└── src
    └── main.c
```

## 配置文件

路径：

```text
config/monitor.conf
```

内容：

```text
monitor_path=data/watch.txt
log_path=logs/monitor.log
interval_sec=1
```

含义：

```text
monitor_path   要监控的文件
log_path       日志文件路径
interval_sec   检查间隔，单位秒
```

## 配置结构体

```c
typedef struct {
    char monitor_path[128];
    char log_path[128];
    int interval_sec;
} MonitorConfig;
```

程序解析配置后，把结果保存到这个结构体里。

注意区别：

```text
config_path           配置文件路径，例如 config/monitor.conf
config.monitor_path   被监控文件路径，例如 data/watch.txt
```

今天多次出错都和这两个变量混用有关。

## load_config

作用：

```text
读取配置文件，解析 key=value。
```

核心逻辑：

```c
if (strncmp(line, "monitor_path=", 13) == 0) {
    sscanf(line, "monitor_path=%127s", config->monitor_path);
} else if (strncmp(line, "log_path=", 9) == 0) {
    sscanf(line, "log_path=%127s", config->log_path);
} else if (strncmp(line, "interval_sec=", 13) == 0) {
    config->interval_sec = atoi(line + 13);
}
```

`strncmp` 返回 `0` 表示相等。

不能写成：

```c
if (strncmp(...)) {
```

那样表示“不相等时进入分支”。

## get_file_size

使用 `stat` 获取文件大小：

```c
static int get_file_size(const char *path, long *size)
{
    struct stat st;

    if (stat(path, &st) < 0) {
        perror("stat monitor file");
        return -1;
    }

    *size = st.st_size;
    return 0;
}
```

`long *size` 是输出参数。

调用方传：

```c
&file_size
```

函数内部通过：

```c
*size = st.st_size;
```

把结果写回调用方变量。

## 监控循环

初始大小：

```c
last_size = file_size;
```

循环检查：

```c
while (!g_stop) {
    sleep(config.interval_sec);

    if (get_file_size(config.monitor_path, &file_size) < 0) {
        fclose(log_file);
        return 1;
    }

    current_size = file_size;

    if (current_size != last_size) {
        printf("file size changed: %ld -> %ld bytes\n", last_size, current_size);
        last_size = current_size;
    } else {
        printf("file size unchanged: %ld bytes\n", current_size);
    }
}
```

注意：

```text
循环里必须监控 config.monitor_path。
不能写成 config_path。
```

否则检查的是配置文件大小，而不是被监控文件大小。

## 两个终端测试

终端 1：运行监控程序

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day18_file_monitor_tool
make run
```

终端 2：修改被监控文件

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day18_file_monitor_tool
make append
```

原因：

```text
终端 1 正在被 file_monitor_tool 占用。
此时在同一个终端输入 echo，不会被 shell 执行。
所以需要第二个终端执行 make append。
```

## 信号优雅退出

全局退出标志：

```c
static volatile sig_atomic_t g_stop = 0;
```

处理函数：

```c
static void handle_sigint(int signo)
{
    (void)signo;
    g_stop = 1;
}
```

注册：

```c
sa.sa_handler = handle_sigint;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;

if (sigaction(SIGINT, &sa, NULL) < 0) {
    perror("sigaction");
    fclose(log_file);
    return 1;
}
```

注意：

```text
sigaction 必须在 while 循环前注册。
如果写在 while 后面，程序会先进入循环，永远执行不到注册代码。
```

## 日志文件

打开日志：

```c
log_file = fopen(config.log_path, "a");
```

写日志：

```c
fprintf(log_file, "file size changed: %ld -> %ld bytes\n", last_size, current_size);
fflush(log_file);
```

退出前关闭：

```c
fprintf(log_file, "file monitor exit\n");
fflush(log_file);
fclose(log_file);
```

原则：

```text
打开了资源，就要保证正常退出和错误路径都能释放资源。
```

## Makefile

常用命令：

```bash
make clean
make
make run
make append
make show_log
```

说明：

```text
make run       运行监控程序
make append    向 data/watch.txt 追加一行
make show_log  查看 logs/monitor.log
make clean     删除 build 和日志文件
```

## 今日遇到的问题

1. usage 中误用 `argv[1]`，应使用 `argv[0]`。
2. 配置文件一开始没有创建。
3. `strncmp` 少写 `== 0`，导致解析逻辑反了。
4. `config_path` 和 `config.monitor_path` 混用。
5. `file_size` 重复定义。
6. 在同一个终端运行监控程序时，输入 `echo` 不会被 shell 执行。
7. `sigaction` 一开始放在 while 后面，导致注册代码执行不到。
8. 正常退出时一开始忘记 `fclose(log_file)`。

## 今日总结

今天我学会了：

1. 用配置文件控制程序行为。
2. 用结构体保存配置项。
3. 用 `stat` 周期性获取文件大小。
4. 判断文件大小是否变化。
5. 用日志文件记录监控结果。
6. 用 `sigaction` 支持 Ctrl+C 优雅退出。
7. 理解长期运行程序必须处理资源清理。

今天还需要加强：

1. 配置解析的鲁棒性，例如空行、注释、非法值。
2. 日志模块化，把日志逻辑拆成独立文件。
3. 错误路径统一清理，避免多处重复 `fclose`。
