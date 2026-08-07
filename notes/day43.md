# Day43 LED 控制：通过 sysfs 操作开发板硬件

## 今日目标

Day43 的目标是让 C 程序控制 i.MX6ULL 开发板上的真实 LED。

今天不写驱动，而是在应用层使用 Linux 已经暴露出来的 LED 子系统接口：

```text
/sys/class/leds/<led_name>/brightness
```

控制链路：

```text
C 程序
  -> open/write/close
  -> /sys/class/leds/red/brightness
  -> Linux LED 子系统
  -> 内核驱动
  -> GPIO
  -> 开发板 LED 亮灭
```

## 开发板 LED 节点

在开发板执行：

```bash
ls -l /sys/class/leds
```

发现当前板子有：

```text
blue
cpu
green
mmc0::
mmc1::
red
```

今天只操作普通三色 LED：

```text
red
green
blue
```

暂时不操作：

```text
cpu
mmc0::
mmc1::
```

因为它们可能与系统状态、CPU 状态、存储活动指示有关。

## 手动控制 LED

进入 LED 目录：

```bash
cd /sys/class/leds
```

手动点亮和熄灭：

```bash
echo 1 > red/brightness
sleep 1
echo 0 > red/brightness

echo 1 > green/brightness
sleep 1
echo 0 > green/brightness

echo 1 > blue/brightness
sleep 1
echo 0 > blue/brightness
```

读取状态：

```bash
cat red/brightness
cat green/brightness
cat blue/brightness
```

输出均为：

```text
0
```

说明三个 LED 最终都处于关闭状态。

## trigger 状态

查看 LED trigger：

```bash
cat red/trigger
cat green/trigger
cat blue/trigger
```

当前均为：

```text
[none]
```

说明 LED 处于手动控制模式，可以通过写 `brightness` 控制。

常见 trigger 含义：

```text
none       手动控制
timer      定时闪烁
heartbeat  系统心跳闪烁
mmc0/mmc1  存储设备活动指示
```

## 项目路径

本地路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day43_led_control
```

开发板路径：

```text
/home/debian/apps/day43_led_control
```

源码：

```text
src/main.c
```

目标程序：

```text
ledctl
```

## ledctl 功能

支持命令：

```text
./ledctl red on
./ledctl red off
./ledctl green on
./ledctl green off
./ledctl blue on
./ledctl blue off
```

非法参数示例：

```text
./ledctl yellow on
./ledctl red abc
```

程序会提示：

```text
invalid led
invalid action
```

## 程序核心流程

参数解析：

```text
argv[1] -> LED 名字：red / green / blue
argv[2] -> 动作：on / off
```

动作转换：

```text
on  -> "1"
off -> "0"
```

路径拼接：

```text
/sys/class/leds/red/brightness
/sys/class/leds/green/brightness
/sys/class/leds/blue/brightness
```

系统调用流程：

```text
open(path, O_WRONLY)
write(fd, value, strlen(value))
close(fd)
```

## 头文件作用

```c
#include <fcntl.h>
```

提供：

```text
open()
O_WRONLY
```

```c
#include <unistd.h>
```

提供：

```text
write()
close()
```

```c
#include <limits.h>
```

提供：

```text
PATH_MAX
```

用于定义路径缓冲区：

```c
char path[PATH_MAX];
```

## 工具链问题

第一次编译 `ledctl` 时，如果使用 Ubuntu 22.04 自带的：

```text
/usr/bin/arm-linux-gnueabihf-gcc
gcc 11.4.0
```

上传到开发板后会报：

```text
GLIBC_2.34 not found
```

原因是开发板 Debian 10 的 glibc 版本较旧：

```text
glibc 2.28
```

解决方式：

使用已经验证成功的 Arm GNU Toolchain 8.3：

```text
/home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
```

Makefile 中固定工具链路径，避免重新打开终端后 PATH 丢失。

## Makefile

当前 Makefile 固定使用 Arm GNU Toolchain 8.3：

```makefile
cc := /home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
ta := ledctl
st := -Wall -g -O0
src := src/main.c

all:$(ta)
$(ta):$(src)
	$(cc) $(st) $(src) -o $(ta)

deploy:
	scp $(ta) debian@192.168.7.2:/home/debian/apps/day43_led_control/

clean:
	rm -rf $(ta)

.PHONY:all clean deploy
```

编译：

```bash
make clean
make
file ledctl
```

部署：

```bash
make deploy
```

## 今日踩坑

### 拼错 brightness

错误：

```bash
echo 0 > blue/brighness
```

正确：

```bash
echo 0 > blue/brightness
```

因为 sysfs 不允许随便创建未知文件，所以拼错时可能出现：

```text
Permission denied
```

### ARM 程序不能在 WSL 运行

在 WSL 执行 ARM 程序会报：

```text
cannot execute binary file: Exec format error
```

原因：

```text
ledctl 是 ARM 程序
WSL 是 x86_64 环境
```

如果只想在 WSL 测试参数逻辑，可以用：

```bash
gcc -Wall -g -O0 src/main.c -o ledctl_x86
```

但真正控制 LED 必须在开发板上运行 ARM 版本。

### open 失败时不能 close(fd)

错误逻辑：

```c
int fd = open(path, O_WRONLY);
if (fd < 0) {
    perror("open");
    close(fd);
    return 1;
}
```

`fd < 0` 表示打开失败，此时 `fd` 不是有效文件描述符，不应该 `close(fd)`。

正确逻辑：

```c
int fd = open(path, O_WRONLY);
if (fd < 0) {
    perror("open");
    return 1;
}
```

## 今日结论

Day43 完成了从“程序在板子上运行”到“程序控制真实硬件”的跨越。

今天真正掌握的是：

```text
Linux 应用程序可以通过 sysfs 操作硬件设备
LED 子系统把硬件控制抽象成 brightness 文件
C 程序通过 open/write/close 就能控制 LED
交叉编译工具链必须与开发板系统版本匹配
```

这一步是后续把设备网关接入真实硬件的基础。
