# Day40 Board Bring-up

## 开发板信息

- 板卡型号：野火 EBF6ULL S1 Pro
- CPU / SoC：NXP i.MX6ULL
- CPU 架构：ARMv7 / Cortex-A7
- Linux 内核：4.19.35-imx6
- 系统版本：Debian GNU/Linux 10 (buster)
- 主机名：npi
- 串口登录用户：root
- 串口登录密码：root
- SSH 登录用户：debian
- SSH 登录密码：temppwd

## 连接信息

- 供电：DC 12V
- 调试串口：USB 转串口 CH340
- 串口参数：115200 8N1，无流控
- USB 网络接口：Micro USB / USB OTG
- Windows USB 网卡 IP：192.168.7.1
- 开发板 usb0 IP：192.168.7.2
- WSL 用户：anllenge

## 板端系统信息

```text
root@npi:~# whoami
root

root@npi:~# uname -a
Linux npi 4.19.35-imx6 #1.2508stable SMP PREEMPT Sat Aug 23 03:32:38 UTC 2025 armv7l GNU/Linux

root@npi:~# cat /etc/os-release
PRETTY_NAME="Debian GNU/Linux 10 (buster)"
VERSION_ID="10"
VERSION="10 (buster)"
```

## 网络信息

开发板：

```text
usb0: 192.168.7.2/30
```

Windows：

```text
以太网 2: 192.168.7.1/30
```

## 文件传输验证

WSL 侧创建测试文件：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects
mkdir -p day40_board_bringup
echo hello_imx6ull > day40_board_bringup/hello_board.txt
```

上传到开发板：

```bash
scp day40_board_bringup/hello_board.txt debian@192.168.7.2:/home/debian/hello_board.txt
```

开发板验证：

```bash
ls -l /home/debian/hello_board.txt
cat /home/debian/hello_board.txt
```

## 今日目标

- [x] 串口能看到启动日志
- [x] 能登录开发板 Linux
- [x] Windows 识别 USB/RNDIS 网卡
- [x] 开发板 usb0 获取固定 IP
- [x] SSH 登录开发板
- [x] scp 上传文件到开发板
- [ ] 交叉编译程序并上传运行
