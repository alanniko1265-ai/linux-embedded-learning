# Day54 网关运行诊断接口

## 今日目标

Day54 的目标是在现有网关中增加一个运行诊断命令：

```text
diag
```

client 输入：

```text
diag
```

server 返回：

```text
OK code=0 msg=diag version=1.1.0 pid=800 uptime_sec=10
```

这个功能用于回答现场设备维护时常见的问题：

```text
当前运行的是哪个版本？
server 进程 PID 是多少？
服务运行了多久？
```

## 项目路径

本地路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day54_gateway_diagnostics
```

开发板路径：

```text
/home/debian/apps/day54_gateway_diagnostics
```

systemd 服务：

```text
demo-gateway.service
```

## 新增模块

新增头文件：

```text
include/diagnostics.h
```

内容：

```c
#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <stddef.h>

void diagnostics_init(void);
void diagnostics_build_response(char *buffer, size_t buffer_size);

#endif
```

新增源文件：

```text
src/diagnostics.c
```

核心内容：

```c
#include "diagnostics.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define GATEWAY_VERSION "1.1.0"

static time_t g_start_time;

void diagnostics_init(void)
{
    g_start_time = time(NULL);
}

void diagnostics_build_response(char *buffer, size_t buffer_size)
{
    time_t now = time(NULL);
    long uptime_sec = 0;

    if (g_start_time > 0 && now >= g_start_time) {
        uptime_sec = (long)(now - g_start_time);
    }

    snprintf(buffer,
             buffer_size,
             "OK code=0 msg=diag version=%s pid=%d uptime_sec=%ld",
             GATEWAY_VERSION,
             getpid(),
             uptime_sec);
}
```

## 关键理解

### 1. 为什么要有 diagnostics_init

server 启动时调用：

```c
diagnostics_init();
```

它记录启动时间：

```c
g_start_time = time(NULL);
```

后面收到 `diag` 命令时：

```text
当前时间 - 启动时间 = 运行秒数 uptime_sec
```

### 2. 为什么 g_start_time 用 static

```c
static time_t g_start_time;
```

这里的 `static` 表示这个变量只在 `diagnostics.c` 内部可见。

它是诊断模块自己的内部状态，不应该让其他 `.c` 文件随便修改。

### 3. 为什么用 snprintf

```c
snprintf(buffer, buffer_size, ...);
```

它把响应内容写入 server 的 response 缓冲区。

相比 `sprintf`，`snprintf` 多了 `buffer_size`，可以限制最大写入长度，降低数组越界风险。

### 4. 为什么用 getpid

```c
getpid()
```

它返回当前 server 进程 PID。

实际验证时：

```text
systemctl status demo-gateway -> Main PID: 800
client diag -> pid=800
```

两个 PID 一致，说明 client 查询到的就是 systemd 正在管理的 server。

## 接入 command.c

在 `src/command.c` 中包含：

```c
#include "diagnostics.h"
```

新增分支：

```c
else if(strcmp(request,"diag\n")==0){
    diagnostics_build_response(response, response_size);
}
```

注意这里匹配的是：

```text
diag\n
```

因为 client 发送命令时会带换行符。

## 接入 server.c

在 `src/server.c` 中包含：

```c
#include "diagnostics.h"
```

在日志初始化成功后调用：

```c
if(logger_init(config.log_path)<0){
    return 1;
}

diagnostics_init();

logger_info("server starting");
```

## 修改 Makefile

server 源文件列表增加：

```makefile
src/diagnostics.c
```

否则会出现链接错误：

```text
undefined reference to `diagnostics_init'
undefined reference to `diagnostics_build_response'
```

原因是：

```text
头文件只提供声明
.c 文件才提供函数实现
Makefile 必须把实现文件编译进去
```

## systemd 切换到 Day54

Day54 的服务文件必须指向新目录：

```ini
WorkingDirectory=/home/debian/apps/day54_gateway_diagnostics
ExecStart=/home/debian/apps/day54_gateway_diagnostics/server
```

安装服务文件：

```bash
install -m 644 /home/debian/apps/day54_gateway_diagnostics/demo-gateway.service /etc/systemd/system/demo-gateway.service
systemctl daemon-reload
systemctl restart demo-gateway
systemctl status demo-gateway
```

成功状态：

```text
CGroup: /system.slice/demo-gateway.service
        └─700 /home/debian/apps/day54_gateway_diagnostics/server
```

## 今日遇到的问题

### 1. 根分区空间满

上传 Day54 时出现：

```text
No space left on device
```

检查：

```bash
df -h
```

发现：

```text
/dev/mmcblk1p2  7.1G  7.0G  0  100% /
```

继续定位：

```bash
du -xhd1 / 2>/dev/null | sort -h
du -xhd1 /var 2>/dev/null | sort -h
ls -lhS /var/log | head -20
```

发现是 `/var/log` 占用 6.5G：

```text
daemon.log  3.6G
syslog.1    2.1G
syslog      875M
```

清空大日志：

```bash
truncate -s 0 /var/log/daemon.log
truncate -s 0 /var/log/syslog
truncate -s 0 /var/log/syslog.1
```

空间恢复：

```text
/dev/mmcblk1p2  7.1G  543M  6.2G  8% /
```

### 2. 正在运行的 server 不能直接覆盖

修正 `msg==diag` 后重新上传 server，出现：

```text
scp: /home/debian/apps/day54_gateway_diagnostics//server: Text file busy
```

原因是 systemd 正在运行这个可执行文件，不能直接覆盖。

正确更新流程：

```bash
systemctl stop demo-gateway
scp build/server debian@192.168.7.2:/home/debian/apps/day54_gateway_diagnostics/
systemctl start demo-gateway
systemctl status demo-gateway
```

今日理解：

```text
更新板端服务时，不是简单 scp 覆盖文件。
如果目标程序正在运行，要先 stop，再上传，再 start。
```

## 最终验证

systemd 状态：

```text
Main PID: 800
CGroup:
  └─800 /home/debian/apps/day54_gateway_diagnostics/server
```

client 测试：

```text
> diag
[response] OK code=0 msg=diag version=1.1.0 pid=800 uptime_sec=10
```

验证通过：

```text
diag 命令生效
版本号正确
PID 与 systemd 一致
uptime_sec 正常增长
```

## 今日总结

Day54 给网关增加了运行诊断能力。

它不是外设功能，而是设备维护能力：

```text
远程查询服务版本
远程确认进程 PID
远程判断服务是否刚重启
```

这类接口在真实嵌入式 Linux 设备中很常见，因为设备部署到现场后，不能总靠登录系统手动 `ps`、`systemctl`、`tail` 来判断状态。
