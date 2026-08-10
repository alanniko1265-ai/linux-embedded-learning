# Day48 将设备网关部署为 systemd 服务

## 今日目标

Day48 的目标是把 Day46 的设备网关从“手动运行程序”升级为“开发板系统服务”。

以前运行方式：

```bash
./server
```

这种方式依赖当前终端。终端断开、程序崩溃、系统重启后，服务就可能消失。

今天运行方式：

```bash
systemctl start demo-gateway
systemctl stop demo-gateway
systemctl restart demo-gateway
systemctl status demo-gateway
```

这表示程序由 systemd 管理，更接近真实嵌入式 Linux 产品中的后台服务。

## 项目路径

本地路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day48_gateway_systemd_service
```

开发板路径：

```text
/home/debian/apps/day48_gateway_systemd_service
```

服务文件：

```text
/etc/systemd/system/demo-gateway.service
```

## systemd 服务文件

文件内容：

```ini
[Unit]
Description=Demo Gateway Service
After=network.target

[Service]
Type=simple
WorkingDirectory=/home/debian/apps/day48_gateway_systemd_service
ExecStart=/home/debian/apps/day48_gateway_systemd_service/server
Restart=on-failure
RestartSec=2
User=root

[Install]
WantedBy=multi-user.target
```

关键字段：

| 字段 | 作用 |
|---|---|
| `Description` | 服务说明，`systemctl status` 中会显示 |
| `After=network.target` | 尽量在网络初始化后启动 |
| `Type=simple` | 前台程序，由 systemd 直接管理进程 |
| `WorkingDirectory` | 程序运行时的当前目录 |
| `ExecStart` | 真正要启动的程序路径 |
| `Restart=on-failure` | 程序异常退出时自动重启 |
| `RestartSec=2` | 失败后等待 2 秒再重启 |
| `User=root` | 用 root 权限运行，便于访问 `/dev/input/event1` 和 LED sysfs |
| `WantedBy=multi-user.target` | 设置开机自启动时挂到多用户运行级别 |

## 部署步骤

在 WSL 编译：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day48_gateway_systemd_service
make clean
make
file build/server build/client
```

确认结果：

```text
ELF 32-bit LSB executable, ARM, EABI5
```

上传到开发板：

```bash
ssh debian@192.168.7.2 "mkdir -p /home/debian/apps/day48_gateway_systemd_service/config /home/debian/apps/day48_gateway_systemd_service/logs"
scp build/server build/client debian@192.168.7.2:/home/debian/apps/day48_gateway_systemd_service/
scp config/gateway.conf debian@192.168.7.2:/home/debian/apps/day48_gateway_systemd_service/config/
scp deploy/demo-gateway.service debian@192.168.7.2:/home/debian/apps/day48_gateway_systemd_service/
```

在开发板 root 终端安装服务：

```bash
cp /home/debian/apps/day48_gateway_systemd_service/demo-gateway.service /etc/systemd/system/demo-gateway.service
systemctl daemon-reload
systemctl start demo-gateway
systemctl status demo-gateway
```

成功状态：

```text
Active: active (running)
```

设置开机自启动：

```bash
systemctl enable demo-gateway
systemctl is-enabled demo-gateway
```

成功输出：

```text
enabled
```

## 今日遇到的问题

### 1. 普通用户无法读取按键设备

用 `debian` 用户手动运行 server 时出现：

```text
open key event: Permission denied
```

原因是程序要读取：

```text
/dev/input/event1
```

这类设备节点通常需要 root 权限。

解决方式：systemd 服务中使用：

```ini
User=root
```

### 2. 端口被旧进程占用

systemd 初次启动失败，状态中出现：

```text
bind: Address already in use
```

意思是 `9200` 端口已经被旧的 `server` 占用。

定位命令：

```bash
netstat -tnlp | grep 9200
```

实际看到：

```text
tcp 0 0 0.0.0.0:9200 0.0.0.0:* LISTEN 6736/./server
```

说明 PID `6736` 的旧 server 占用了端口。

停止旧进程：

```bash
kill 6736
```

再次确认端口释放：

```bash
netstat -tnlp | grep 9200
```

没有输出表示端口已经空闲。

## 服务管理命令

| 命令 | 作用 |
|---|---|
| `systemctl start demo-gateway` | 启动服务 |
| `systemctl stop demo-gateway` | 停止服务 |
| `systemctl restart demo-gateway` | 重启服务 |
| `systemctl status demo-gateway` | 查看服务状态 |
| `systemctl enable demo-gateway` | 设置开机自启动 |
| `systemctl is-enabled demo-gateway` | 查看是否开机自启动 |

## 验证结果

服务状态：

```text
Active: active (running)
Main PID: 7498 (server)
```

进程验证：

```bash
ps -ef | grep day48_gateway_systemd_service/server | grep -v grep
```

结果：

```text
root 7498 1 0 15:56 ? 00:00:00 /home/debian/apps/day48_gateway_systemd_service/server
```

端口验证：

```bash
netstat -tnlp | grep 9200
```

结果：

```text
tcp 0 0 0.0.0.0:9200 0.0.0.0:* LISTEN 7498/server
```

业务验证：

```text
status -> 返回设备状态
led on -> 控制开发板 LED 打开
led off -> 控制开发板 LED 关闭
```

日志验证：

```bash
tail -n 20 /home/debian/apps/day48_gateway_systemd_service/logs/server.log
```

关键日志：

```text
[2026-08-10 15:56:13] [INFO] server starting
[2026-08-10 15:56:13] [INFO] server config loaded
[2026-08-10 15:56:26] [INFO] server stopped
[2026-08-10 15:56:26] [INFO] server starting
[2026-08-10 15:56:26] [INFO] server config loaded
```

## 今日理解

今天的重点不是多学几个命令，而是理解嵌入式 Linux 程序的部署方式：

```text
C 程序
  -> 编译成 ARM 可执行文件
  -> 放到开发板固定目录
  -> 用 systemd 注册成服务
  -> 用 systemctl 管理生命周期
  -> 用 ps/netstat/log 排查运行状态
```

以后一个板上应用是否“真的跑起来”，不能只看一个现象，而要建立判断链：

```text
systemctl status: systemd 认为服务是否运行
ps -ef: Linux 进程是否存在
netstat: 端口是否正在监听
client: 业务功能是否可用
server.log: 程序内部逻辑是否正常
```
