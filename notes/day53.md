# Day53 网关日志轮转与存储保护

## 今日目标

Day53 的目标是给已经由 systemd 托管的网关服务增加日志轮转规则，避免开发板长期运行后日志文件无限增长。

要解决的问题：

```text
server 长期运行
  -> logs/server.log 不断变大
  -> 占用 eMMC/SD/flash 空间
  -> 严重时影响系统和服务运行
```

今天使用 Linux 常见工具：

```text
logrotate
```

它的作用是按规则管理日志文件，例如改名、保留多份、压缩旧日志。

## 当前服务和日志路径

当前服务来自 Day48：

```text
demo-gateway.service
```

开发板程序目录：

```text
/home/debian/apps/day48_gateway_systemd_service
```

网关日志文件：

```text
/home/debian/apps/day48_gateway_systemd_service/logs/server.log
```

## 检查 logrotate 环境

检查命令位置：

```bash
which logrotate
```

实际结果：

```text
/usr/sbin/logrotate
```

检查规则目录：

```bash
ls -ld /etc/logrotate.d
```

实际结果：

```text
drwxr-xr-x 2 root root 4096 Aug 12  2025 /etc/logrotate.d
```

说明开发板已经有 logrotate，并且系统规则目录存在。

## 日志轮转规则

规则文件：

```text
/etc/logrotate.d/demo-gateway
```

内容：

```text
/home/debian/apps/day48_gateway_systemd_service/logs/server.log {
    size 64k
    rotate 4
    missingok
    notifempty
    copytruncate
    compress
    delaycompress
}
```

字段含义：

| 配置 | 作用 |
|---|---|
| `size 64k` | 日志超过 64KB 后触发轮转 |
| `rotate 4` | 最多保留 4 份旧日志 |
| `missingok` | 日志文件不存在时不报错 |
| `notifempty` | 日志为空时不轮转 |
| `copytruncate` | 复制旧日志后清空原日志文件 |
| `compress` | 压缩旧日志 |
| `delaycompress` | 最新一份旧日志暂时不压缩，下一轮再压缩 |

## 为什么使用 copytruncate

当前 logger 模块运行时会打开：

```text
logs/server.log
```

如果直接把 `server.log` 改名，正在运行的 server 可能还在往旧文件句柄里写。

`copytruncate` 的行为是：

```text
复制 server.log -> server.log.1
清空 server.log
server 继续写 server.log
```

这样不需要重启服务，也能完成日志轮转。

这适合当前网关程序。

## 安装规则

从 WSL 上传规则文件：

```bash
scp deploy/demo-gateway-logrotate debian@192.168.7.2:/home/debian/demo-gateway-logrotate
```

在开发板 root 终端安装：

```bash
cp /home/debian/demo-gateway-logrotate /etc/logrotate.d/demo-gateway
cat /etc/logrotate.d/demo-gateway
```

检查是否有拼错文件：

```bash
ls -l /etc/logrotate.d/demo-gateway /etc/logrotate.d/demo-gatewy
```

实际结果：

```text
ls: cannot access '/etc/logrotate.d/demo-gatewy': No such file or directory
-rwxr-xr-x 1 root root 171 Aug 11 09:00 /etc/logrotate.d/demo-gateway
```

后来修正权限为配置文件权限：

```bash
chmod 644 /etc/logrotate.d/demo-gateway
ls -l /etc/logrotate.d/demo-gateway
```

结果：

```text
-rw-r--r-- 1 root root 171 Aug 11 09:00 /etc/logrotate.d/demo-gateway
```

## 模拟测试

执行：

```bash
logrotate -d /etc/logrotate.d/demo-gateway
```

`-d` 是 debug 模式，只模拟，不真正修改日志。

第一次出现提醒：

```text
Potentially dangerous mode on /etc/logrotate.d/demo-gateway: 0755
```

原因是规则文件有执行权限。修正为 `644` 后，警告消失。

模拟结果：

```text
rotating pattern: /home/debian/apps/day48_gateway_systemd_service/logs/server.log  65536 bytes (4 rotations)
log does not need rotating (log size is below the 'size' threshold)
```

意思是规则语法正常，但当前日志还没达到 64KB。

## 强制轮转测试

轮转前：

```bash
ls -lh /home/debian/apps/day48_gateway_systemd_service/logs
```

结果：

```text
server.log 11K
```

强制轮转：

```bash
logrotate -f /etc/logrotate.d/demo-gateway
```

`-f` 表示 force，忽略大小条件，强制执行一次。

轮转后：

```text
server.log   0
server.log.1 11K
```

说明 `copytruncate` 生效：

```text
旧内容进入 server.log.1
当前 server.log 被清空
```

## 验证服务继续写日志

运行 client：

```text
status
led on
led off
quit
```

查看新日志：

```bash
tail -n 20 /home/debian/apps/day48_gateway_systemd_service/logs/server.log
```

结果中出现：

```text
[2026-08-11 09:05:52] [INFO] client connected
[2026-08-11 09:06:02] [INFO] request received: status
[2026-08-11 09:06:06] [INFO] request received: led on
[2026-08-11 09:06:10] [INFO] client disconnected
```

说明日志轮转后，server 仍然继续写入新的 `server.log`。

## 第二次轮转与压缩

再次执行：

```bash
logrotate -f /etc/logrotate.d/demo-gateway
ls -lh /home/debian/apps/day48_gateway_systemd_service/logs
```

结果：

```text
server.log       0
server.log.1     543
server.log.2.gz  935
```

含义：

| 文件 | 含义 |
|---|---|
| `server.log` | 当前正在写的新日志 |
| `server.log.1` | 最新一份旧日志，因为 `delaycompress` 暂时不压缩 |
| `server.log.2.gz` | 更早的旧日志，已经被压缩 |

## logrotate 状态文件

查看状态：

```bash
grep server.log -n /var/lib/logrotate/status
```

结果：

```text
14:"/home/debian/apps/day48_gateway_systemd_service/logs/server.log" 2026-8-11-9:6:56
```

`/var/lib/logrotate/status` 是 logrotate 的状态文件。

它会记录某个日志上一次轮转的时间，后续 logrotate 会根据这个状态和规则判断是否需要再次轮转。

## 今日理解

今天学到的是嵌入式 Linux 服务长期运行时必须考虑的一个问题：

```text
程序能运行
不等于适合长期运行
```

长期运行还要考虑：

```text
日志是否会无限增长
存储是否会被写满
服务是否需要重启
旧日志是否需要压缩
排查时如何找到当前日志和历史日志
```

今天形成的判断链：

```text
which logrotate
  -> 确认工具存在

/etc/logrotate.d/demo-gateway
  -> 确认规则安装

logrotate -d
  -> 模拟测试语法

logrotate -f
  -> 强制验证轮转效果

tail server.log
  -> 确认服务继续写日志

/var/lib/logrotate/status
  -> 确认 logrotate 已记录状态
```
