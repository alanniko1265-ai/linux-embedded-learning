# Day47 RS485 接口验证与板级排障

## 今日目标

Day47 的目标不是编写新的业务程序，而是完成 EBF6ULL Pro 开发板 RS485-1 接口的硬件 bring-up：

```text
确认设备树配置
  -> 确认板上跳帽和收发器供电
  -> 连接 USB-RS485 模块
  -> 验证 PC 与开发板双向通信
  -> 找出通信失败的真实原因
```

最终结果：

```text
Windows -> USB-RS485 -> RS485-1 -> /dev/ttymxc1：成功
/dev/ttymxc1 -> RS485-1 -> USB-RS485 -> Windows：成功
```

## 本日没有独立项目目录

Day47 主要进行开发板硬件接口验证，没有创建独立 C 工程。

相关环境：

```text
开发板：野火 EBF6ULL S1 Pro
处理器：NXP i.MX6ULL
系统：Debian 10
内核：Linux 4.19.35-imx6
板端接口：RS485-1
板端设备文件：/dev/ttymxc1
方向控制 GPIO：GPIO22
PC 适配器：MacroSilicon MS3020 USB-RS485
Windows 端口：本次为 COM11，重新插拔后可能变化
```

## RS485 与普通串口的区别

UART 本身产生的是 TX/RX 串行数据，不能直接形成 RS485 差分信号。

开发板上的完整链路是：

```text
应用程序
  -> /dev/ttymxc1
  -> i.MX6ULL UART2
  -> MAX3485 收发器
  -> RS485-1 A/B 接线柱
```

PC 端链路是：

```text
Windows COM11
  -> USB-RS485 模块
  -> T/R+、T/R-
  -> 开发板 RS485-1 A、B
```

因此，Linux 中出现 `/dev/ttymxc1` 只说明 UART 驱动和设备树基本正常，不代表 MAX3485 已供电，也不代表 A/B 总线上一定有信号。

## 半双工与方向控制

两线 RS485 使用同一对 A/B 线完成发送和接收，属于半双工通信：

```text
同一时刻只能有一端驱动总线发送
其他设备应处于接收状态
```

RS485-1 使用 GPIO22 控制 MAX3485 的收发方向：

```text
GPIO22 = 1：发送模式
GPIO22 = 0：接收模式
```

空闲时应恢复为接收状态，避免持续驱动总线。

## 板上跳帽的作用

官方原理图说明：

```text
J7 的 1、2 脚控制 RS485-1 和 RS485-2 的 3.3V 电源
J8 的 1、2 脚控制 CAN1 和 CAN2 的电源
```

Day47 最关键的遗漏是：

```text
J7 的 3V3 与 RS485_3V3 没有使用跳帽连接
```

这会造成：

```text
/dev/ttymxc1 存在
向串口 write() 不报错
UART 的 tx 计数增加
但是 MAX3485 没有供电
所以 RS485 接线柱上没有有效差分信号
```

连接 J7 的 RS485 电源跳帽后，双向通信成功。

## 物理接线

本次使用开发板 RS485-1 与 PC 的 USB-RS485 模块通信。

两线半双工接线：

```text
USB-RS485 T/R+  -> 开发板 RS485-1 A
USB-RS485 T/R-  -> 开发板 RS485-1 B
```

本次接线不需要连接 USB-RS485 模块的 `RXD+`、`RXD-` 和 `GND` 端子。

注意：不同厂商可能对 A/B 或正负标记采用不同命名。如果设备都已正确供电但完全收不到数据，可以交换 A/B 做一次验证；本次实际接线不需要交换。

## 设备树最终状态

开发板 `/boot/uEnv.txt` 中最终保持：

```text
启用：imx-fire-485r1.dtbo
启用：imx-fire-485r2.dtbo
禁用：imx-fire-uart2.dtbo
禁用：imx-fire-uart3.dtbo
禁用：imx-fire-can1.dtbo
禁用：imx-fire-can2.dtbo
```

原因：

```text
RS485-1 与 UART2 复用引脚
RS485-2 与 UART3 复用引脚
同一组引脚不能同时启用普通 UART、CAN 和 RS485 功能
```

设备节点映射：

```text
/dev/ttymxc0：UART1，系统串口控制台，不能用于本实验
/dev/ttymxc1：UART2，对应 RS485-1
/dev/ttymxc2：UART3，对应 RS485-2
```

## GPIO 初始化

sysfs 导出的 GPIO 在重启后不会自动保留，因此每次重启后需要重新导出：

```bash
echo 22 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio22/direction
```

切换为接收：

```bash
echo 0 > /sys/class/gpio/gpio22/value
```

切换为发送：

```bash
echo 1 > /sys/class/gpio/gpio22/value
```

如果 GPIO 已经导出，再次执行 `export` 可能提示资源忙，此时直接使用已有的 `gpio22` 目录即可。

## 串口参数

本次双方使用：

```text
波特率：9600
数据位：8
校验位：None
停止位：1
硬件流控：关闭
软件流控：关闭
```

开发板配置命令：

```bash
stty -F /dev/ttymxc1 9600 cs8 -cstopb -parenb -ixon -ixoff -crtscts raw -echo
```

## PC 向开发板发送

开发板进入接收模式：

```bash
echo 0 > /sys/class/gpio/gpio22/value
: > /tmp/rs485_pc_test.txt
cat /dev/ttymxc1 > /tmp/rs485_pc_test.txt &
```

Windows PowerShell 通过 COM11 发送：

```powershell
$port = New-Object System.IO.Ports.SerialPort "COM11",9600,"None",8,"One"
$port.Open()
$port.WriteLine("pc_to_board_rs485")
$port.Close()
```

开发板停止后台读取并查看内容：

```bash
pkill -f "cat /dev/ttymxc1"
cat /tmp/rs485_pc_test.txt
```

实际输出：

```text
pc_to_board_rs485
```

串口统计信息中 `/dev/ttymxc1` 对应的接收计数变为：

```text
1: uart:IMX ... tx:0 rx:18 ...
```

`rx:18` 证明 UART2 确实从物理线路收到了 18 字节。

## 开发板向 PC 发送

Windows PowerShell 打开 COM11 并等待一行数据：

```powershell
$port = New-Object System.IO.Ports.SerialPort "COM11",9600,"None",8,"One"
$port.ReadTimeout = 15000
$port.Open()
$line = $port.ReadLine()
$port.Close()
$line
```

开发板切换到发送状态并发送：

```bash
echo 1 > /sys/class/gpio/gpio22/value
printf "board_to_pc_rs485\n" > /dev/ttymxc1
echo 0 > /sys/class/gpio/gpio22/value
```

Windows 实际输出：

```text
board_to_pc_rs485
```

发送结束后立即写回 `GPIO22=0`，让 RS485-1 返回接收状态。

## 为什么文件曾经是空的

测试中曾连续启动两个进程：

```bash
cat /dev/ttymxc1 > /tmp/rs485_pc_test.txt &
cat /dev/ttymxc1 > /tmp/rs485_pc_test.txt &
```

两个进程会同时竞争读取同一个串口。串口数据不是广播给所有读取者，一份数据可能被其中任意一个进程取走；两个进程还同时用 `>` 打开同一个文件，使结果更加不可预测。

正确做法是只保留一个读取进程，并用以下命令检查：

```bash
jobs
```

结束残留进程：

```bash
pkill -f "cat /dev/ttymxc1"
```

## `/boot/uEnv.txt` 故障与恢复经验

Day47 曾经在直接修改 `/boot/uEnv.txt` 后出现 U-Boot 报错：

```text
Invalid FAT entry
Unable to read file /uEnv.txt
```

这说明启动分区文件损坏或 FAT 目录项异常，而不是 Linux 内核、根文件系统或 eMMC 整体损坏。

以后不要直接对 `/boot/uEnv.txt` 使用原地修改命令。更安全的流程是：

```bash
cp /boot/uEnv.txt /root/uEnv.txt.work
```

先编辑和检查 `/root/uEnv.txt.work`，确认内容后再复制回启动分区：

```bash
cp /root/uEnv.txt.work /boot/uEnv.txt
sync
```

当前保留了备份：

```text
/boot/uEnv.txt.bak-day47
/boot/uEnv.txt.bak-before-rs232
/root/uEnv.txt.safe
```

## 本日排障方法

本次问题不能只看某一条命令，而要按完整链路逐层定位：

```text
1. 设备树插件是否启用
2. /dev/ttymxc1 是否生成
3. UART tx/rx 计数是否变化
4. GPIO22 是否处于正确方向
5. J7 是否给 MAX3485 供电
6. 板上信号跳帽是否连接
7. T/R+、T/R- 与 A、B 是否对应
8. Windows COM 号和串口参数是否正确
9. 是否有多个进程竞争串口或 COM 口
```

最重要的认识：

```text
软件 write() 成功，只能证明数据交给了内核或 UART；
它不能证明收发器已供电，也不能证明数据已经到达物理总线。
```

## 官方资料

RS485 使用说明：

```text
https://doc.embedfire.com/linux/imx6/quick_start/zh/latest/quick_start/485/485.html
```

EBF6ULL Pro 底板原理图：

```text
https://doc.embedfire.com/linux/imx6/hardware/zh/latest/_downloads/9182475d1eaf07002d7e3f63ca7696f0/野火_EBF6ULL%20S1%20Pro%20底板_原理图_20220308.pdf
```

## 今日结论

Day47 已经完成 RS485-1 的双向硬件验证：

```text
PC -> USB-RS485 -> A/B -> MAX3485 -> UART2 -> Linux
Linux -> UART2 -> MAX3485 -> A/B -> USB-RS485 -> PC
```

同时掌握了：

```text
RS485 半双工通信
UART 与 RS485 收发器的关系
GPIO 方向控制
设备树引脚复用
板上电源跳帽的作用
从软件层到物理层的逐层排障方法
```

Day48 将把本日手动执行的串口配置、GPIO 方向切换和收发流程封装为可复用的 C 模块，为接入设备网关做准备。
