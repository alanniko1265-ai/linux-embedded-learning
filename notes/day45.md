# Day45 按键输入：读取 input event 并控制 LED

## 今日目标

Day45 的目标是读取 i.MX6ULL 开发板上的用户按键，并把按键事件转换成程序行为。

最终效果：

```text
按下用户按键
  -> 程序从 /dev/input/event1 读到事件
  -> 判断 KEY_0 pressed
  -> 切换 led_state
  -> 写 /sys/class/leds/red/brightness
  -> red LED 真实亮灭切换
```

这一天补齐了硬件输入能力：

```text
Day43/Day44：程序控制 LED 输出
Day45：程序读取按键输入
```

## 设备发现

查看 input 设备：

```bash
ls -l /dev/input
```

开发板上发现：

```text
event0 -> snvs-powerkey
event1 -> sgpio-keys
```

查看 input 设备信息：

```bash
cat /proc/bus/input/devices
```

关键结果：

```text
event0：20cc000.snvs:snvs-powerkey
event1：sgpio-keys
```

结论：

```text
/dev/input/event0 是电源键
/dev/input/event1 是普通 GPIO 用户按键
```

今天选择：

```text
/dev/input/event1
```

## 按键编号

查看 input1 名字：

```bash
cat /sys/class/input/input1/name
```

输出：

```text
sgpio-keys
```

查看对应 key code：

```bash
cat /sys/devices/soc0/sgpio-keys/keys
```

输出：

```text
11
```

Linux input 中十进制 `11` 对应：

```text
KEY_0
```

所以程序里判断：

```c
event.type == EV_KEY && event.code == KEY_0
```

## 项目路径

本地路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day45_key_input
```

开发板路径：

```text
/home/debian/apps/day45_key_input
```

源码：

```text
src/main.c
```

目标程序：

```text
key_monitor
```

## input_event 结构

程序读取：

```c
struct input_event event;
read(fd, &event, sizeof(event));
```

核心字段：

```text
event.type
  事件类型

event.code
  具体按键编号

event.value
  按键动作
```

今天读到的事件：

```text
type=1 code=11 value=1
type=0 code=0 value=0
type=1 code=11 value=0
type=0 code=0 value=0
```

解释：

```text
type=1
  EV_KEY，按键事件

code=11
  KEY_0

value=1
  按下

value=0
  松开

type=0 code=0 value=0
  EV_SYN，同步事件
```

所以一次按下再松开可以理解为：

```text
KEY_0 pressed
EV_SYN
KEY_0 released
EV_SYN
```

## 程序逻辑

打开按键事件设备：

```c
int fd = open("/dev/input/event1", O_RDONLY);
```

循环读取事件：

```c
while (1) {
    read(fd, &event, sizeof(event));
}
```

只处理 KEY_0：

```c
if (event.type == EV_KEY && event.code == KEY_0) {
    ...
}
```

按下时切换状态：

```c
if (event.value == 1) {
    led_state = !led_state;
}
```

状态变化：

```text
第一次按下 -> led_state=on
第二次按下 -> led_state=off
第三次按下 -> led_state=on
```

## 按键控制 LED

程序中定义：

```c
#define LED_BRIGHTNESS_PATH "/sys/class/leds/red/brightness"
```

控制 red LED：

```c
static int set_red_led(int on)
{
    const char *value = on ? "1" : "0";

    int fd = open(LED_BRIGHTNESS_PATH, O_WRONLY);
    write(fd, value, strlen(value));
    close(fd);

    return 0;
}
```

按键按下时：

```c
led_state = !led_state;
set_red_led(led_state);
```

最终效果：

```text
按下一次 KEY_0，red LED 点亮
再按一次 KEY_0，red LED 熄灭
```

## 编译和部署

Makefile 使用 Arm GNU Toolchain 8.3：

```makefile
cc := /home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
```

编译：

```bash
make clean
make
file key_monitor
```

部署：

```bash
make deploy
```

开发板运行：

```bash
cd /home/debian/apps/day45_key_input
./key_monitor
```

运行输出示例：

```text
KEY_0 pressed, led_state=on
KEY_0 released
KEY_0 pressed, led_state=off
KEY_0 released
```

## 今日踩坑

### evtest 不存在

开发板没有安装：

```text
evtest
```

所以没有依赖工具，而是自己写 C 程序读取：

```text
/dev/input/event1
```

这更接近真实应用开发。

### event 文件不是文本文件

直接执行：

```bash
cat /dev/input/event1
```

可能没有可读文本输出，因为 input event 是二进制结构体数据。

正确方式是：

```c
read(fd, &event, sizeof(event));
```

然后按 `struct input_event` 解析。

### 查路径不是死记命令

今天找按键路径的逻辑是：

```text
先看 /dev/input 有哪些 event
再看 /proc/bus/input/devices 判断 event 对应谁
再看 /sys/class/input/inputX/name 确认名字
再看 keys 确认 key code
最后写程序读取 /dev/input/eventX
```

以后遇到硬件设备，也会按“先找 Linux 暴露的设备节点，再写程序访问”的方式推进。

## 今日结论

Day45 完成了硬件输入链路：

```text
物理按键
  -> gpio-keys 驱动
  -> /dev/input/event1
  -> struct input_event
  -> KEY_0 pressed/released
  -> 程序状态切换
  -> red LED 真实亮灭
```

现在项目已经具备两个基础硬件能力：

```text
输出：控制 LED
输入：读取按键
```

下一步可以把按键状态接入设备网关，让 `status` 返回真实按键状态，或者让按键事件主动影响网关状态。

