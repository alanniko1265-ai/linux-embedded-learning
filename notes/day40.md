# Day40 开发板基础环境准备

## 今日目标

Day40 是从 PC/WSL Linux 应用开发转向嵌入式 Linux 开发板阶段的第一天。

今天不写新功能，也不烧录系统，而是打通最小上板链路：

```text
串口登录
USB 网络
SSH 登录
scp 文件传输
```

这些是后续交叉编译、GPIO 控制、板端部署的基础。

## 开发板信息

```text
板卡：野火 EBF6ULL S1 Pro
SoC：NXP i.MX6ULL
架构：ARMv7 / Cortex-A7
系统：Debian GNU/Linux 10 (buster)
内核：Linux 4.19.35-imx6
主机名：npi
```

串口登录：

```text
root / root
```

SSH 登录：

```text
debian / temppwd
```

## 安全边界

今天没有做：

```text
烧录系统
修改 BOOT 拨码
擦除 eMMC / NAND
更新 U-Boot
更新 Kernel
接未知电压外设
```

当前阶段只做连接和文件传输，不破坏板端系统。

## 串口连接

使用开发板的 USB 转串口 CH340 接口连接电脑。

串口参数：

```text
波特率：115200
数据位：8
停止位：1
校验：None
流控：None
```

登录后确认：

```bash
whoami
uname -a
cat /etc/os-release
pwd
ip addr
```

确认结果：

```text
当前用户：root
当前路径：/root
系统：Debian 10 buster
架构：armv7l
usb0：192.168.7.2/30
```

## USB 网络

板子通过 Micro USB / USB OTG 接入 Windows。

开发板：

```text
usb0 = 192.168.7.2
```

Windows：

```text
以太网 2 = 192.168.7.1
```

这说明电脑和开发板处于同一个 USB 网络中。

## SSH 登录

Windows / WSL 登录开发板：

```bash
ssh debian@192.168.7.2
```

密码：

```text
temppwd
```

注意：

```text
串口 root 能登录，不代表 SSH root 一定允许登录。
野火 Debian 镜像推荐 SSH 使用 debian 用户。
```

## scp 文件传输

在 WSL 中创建测试文件：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects
mkdir -p day40_board_bringup
echo hello_imx6ull > day40_board_bringup/hello_board.txt
```

上传到开发板：

```bash
scp day40_board_bringup/hello_board.txt debian@192.168.7.2:/home/debian/hello_board.txt
```

在开发板中验证：

```bash
ls -l /home/debian/hello_board.txt
cat /home/debian/hello_board.txt
```

## 终端环境区分

今天踩到的关键点是区分三个终端：

```text
PS C:\...>             Windows PowerShell
anllenge@localhost:~$  WSL Ubuntu
debian@npi:~$          开发板 Linux
root@npi:~#            开发板 root
```

不同终端使用不同路径和命令：

```text
Windows PowerShell 使用 C:\Users\...
WSL 使用 /mnt/c/Users/...
开发板使用 /home/debian 或 /root
```

## 今日完成状态

- [x] MobaXterm 串口登录成功
- [x] 识别 i.MX6ULL 板端系统信息
- [x] 明确 USB 转串口和 USB OTG 的区别
- [x] Windows 识别 USB/RNDIS 网络
- [x] USB 网络 IP 配置成功
- [x] SSH 使用 `debian@192.168.7.2` 登录成功
- [x] WSL 使用 `scp` 上传文件到开发板成功
- [x] 建立 `board_info.md` 记录板端信息

## 今日总结

Day40 完成了嵌入式 Linux 开发的第一条真实链路：

```text
PC / WSL
   |
   v
USB 网络
   |
   v
i.MX6ULL Debian Linux
   |
   v
SSH / scp
```

从下一天开始，可以进入交叉编译：在 WSL 上编译 ARMv7 可执行文件，上传到 i.MX6ULL 开发板运行。
