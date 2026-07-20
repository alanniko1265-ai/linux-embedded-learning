# Day 16 - /proc、/sys、/dev 基础

日期：2026-07-20

## 今日目标

- [x] 补回原计划中的 `/proc`、`/sys`、`/dev` 基础。
- [x] 实现 `system_probe` 工具。
- [x] 读取 `/proc/cpuinfo`。
- [x] 读取 `/proc/meminfo`。
- [x] 写入 `/dev/null`。
- [x] 读取 `/dev/zero`。
- [x] 理解虚拟文件和设备文件的区别。
- [x] 编写 Makefile 构建项目。

## 项目目录

```text
linux_projects/day16_proc_sys_dev
```

## VS Code 打开命令

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day16_proc_sys_dev
code .
```

## 今日程序

程序名：

```text
system_probe
```

运行方式：

```bash
./build/system_probe <cpu|mem|null|zero>
```

支持命令：

```text
cpu   读取 /proc/cpuinfo 前 10 行
mem   读取 /proc/meminfo 前 10 行
null  写入 /dev/null
zero  读取 /dev/zero
```

## /proc

`/proc` 是内核提供的虚拟文件系统。

它里面很多文件看起来像普通文本文件，但内容不是保存在磁盘上的，而是内核运行时动态生成的。

例如：

```text
/proc/cpuinfo   CPU 信息
/proc/meminfo   内存信息
```

读取方式可以像普通文件：

```c
fp = fopen("/proc/cpuinfo", "r");
fgets(line, sizeof(line), fp);
```

今天代码只输出前 10 行：

```c
while (fgets(line, sizeof(line), fp) != NULL && count < 10)
```

这里的 `10` 是人为限制，避免终端刷太多内容，不是系统规定。

## /dev

`/dev` 里是设备文件。

设备文件看起来像文件，但背后通常对应内核驱动或内核提供的特殊设备。

用户态程序可以通过：

```c
open
read
write
close
```

像操作文件一样操作设备。

这就是嵌入式 Linux 用户态访问设备的核心入口。

以后会遇到：

```text
/dev/ttyS1        串口
/dev/ttyUSB0      USB 转串口
/dev/i2c-1        I2C
/dev/spidev0.0    SPI
/dev/input/event0 输入事件
/dev/watchdog     看门狗
```

## /dev/null

`/dev/null` 像一个“黑洞设备”。

特性：

```text
写入：数据被丢弃，但 write 通常返回成功
读取：立刻返回 0，表示 EOF
```

今天代码：

```c
fd = open("/dev/null", O_WRONLY);
n = write(fd, message, strlen(message));
```

这里用 `strlen(message)`，因为 `message` 是：

```c
const char *message
```

它是指针，`sizeof(message)` 得到的是指针大小，不是字符串长度。

## /dev/zero

`/dev/zero` 像一个“无限 0 生成器”。

特性：

```text
读取：持续返回 0x00 字节
写入：通常丢弃数据
```

今天代码：

```c
unsigned char buffer[16];
n = read(fd, buffer, sizeof(buffer));
```

使用 `unsigned char` 是因为这里把数据当作原始字节处理。

输出：

```text
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
```

## 文件描述符 fd

`open` 成功后返回：

```c
int fd;
```

虽然 `fd` 是 `int`，但它不是普通数字，而是当前进程中打开资源的编号。

后续：

```c
read(fd, ...)
write(fd, ...)
close(fd)
```

都是通过这个编号让内核找到对应资源。

所以：

```text
open 成功后，用完必须 close。
```

如果一直 `open` 不 `close`，会造成文件描述符泄漏，长期运行程序可能出现：

```text
Too many open files
```

## message 和 &message

```c
const char *message = "hello /dev/null\n";
```

这里：

```text
message   是字符串内容的起始地址
&message  是 message 这个指针变量自己的地址
```

写字符串内容应该用：

```c
write(fd, message, strlen(message));
```

不能用：

```c
write(fd, &message, sizeof(&message));
```

因为那会写出指针变量本身的内存内容，而不是字符串内容。

## 编译和运行

```bash
make clean
make
make run-cpu
make run-mem
make run-null
make run-zero
make run-bad
```

`make run-bad` 用来测试未知命令：

```text
unknown command: abc
```

这是正常错误处理。

## 今日遇到的问题

1. `show_cpuinfo` / `show_meminfo` 忘记写函数调用括号。
2. `show_meminfo` 一开始少了 `fclose(fp)` 和 `return 0`。
3. 把 `unknown command: abc` 误认为程序报错。
4. 不理解为什么 `/dev/null` 和 `/proc/cpuinfo` 读写方式不同。
5. 不理解 `strlen(message)` 和 `sizeof(message)` 的区别。
6. 不理解为什么 `fd` 是 int 还要 `close`。

## 今日总结

今天我学会了：

1. `/proc` 是内核导出运行信息的虚拟文件系统。
2. `/dev` 是用户态访问设备/驱动的重要入口。
3. `/dev/null` 写入会丢弃数据。
4. `/dev/zero` 读取会得到连续的 0 字节。
5. Linux 中很多设备都可以通过文件接口访问。
6. 嵌入式 Linux 后续操作串口、I2C、SPI、watchdog 时，也会用到 `open/read/write/close/ioctl`。

今天还需要加强：

1. `/sys` 的作用和设备属性文件。
2. 设备文件背后和驱动的关系。
3. 后续学习 `ioctl`，理解为什么有些设备控制不能只靠 read/write。
