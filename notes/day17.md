# Day 17 - ioctl 和 mmap 入门

日期：2026-07-20

## 今日目标

- [x] 补回原计划中的 `ioctl` 和 `mmap` 入门。
- [x] 实现 `ioctl_mmap_intro` 工具。
- [x] 使用 `ioctl` 查询终端窗口大小。
- [x] 使用 `mmap` 映射普通文件。
- [x] 理解 `read/write/ioctl/mmap` 的基本区别。
- [x] 编写 Makefile 构建项目。

## 项目目录

```text
linux_projects/day17_ioctl_mmap_intro
```

## VS Code 打开命令

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day17_ioctl_mmap_intro
code .
```

## 今日程序

程序名：

```text
ioctl_mmap_intro
```

运行方式：

```bash
./build/ioctl_mmap_intro <winsize|mmap>
```

支持命令：

```text
winsize  使用 ioctl 查询终端窗口大小
mmap     使用 mmap 映射 data/input.txt 并输出内容
```

## ioctl

头文件：

```c
#include <sys/ioctl.h>
```

今日调用：

```c
ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
```

含义：

```text
对 STDOUT_FILENO 对应的终端设备发送 TIOCGWINSZ 控制命令，
让内核把终端窗口大小写入 ws 结构体。
```

结构体：

```c
struct winsize ws;
```

输出：

```text
rows: 17
cols: 75
```

窗口大小会随着终端区域变化而变化，这是正常现象。

## ioctl 的意义

`read/write` 适合普通数据读写。

`ioctl` 适合：

```text
查询设备状态
设置设备参数
发送设备控制命令
```

以后嵌入式 Linux 中常见场景：

```text
串口参数控制
I2C/SPI 设备控制
摄像头 V4L2 控制
显示设备控制
自定义字符设备驱动控制接口
```

## mmap

头文件：

```c
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
```

今日流程：

```text
open 打开 data/input.txt
fstat 获取文件大小
mmap 把文件映射到进程虚拟内存
write 输出映射区域内容
munmap 解除映射
close 关闭文件
```

核心调用：

```c
addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
```

参数含义：

```text
NULL          让系统选择映射地址
st.st_size    映射文件大小
PROT_READ     映射区域允许读取
MAP_PRIVATE   私有映射，修改不会写回原文件
fd            要映射的文件
0             从文件偏移 0 开始映射
```

返回值：

```text
成功：返回映射后的内存起始地址
失败：返回 MAP_FAILED
```

输出：

```c
write(STDOUT_FILENO, addr, st.st_size);
```

含义：

```text
从 addr 指向的映射内存开始，输出 st.st_size 个字节。
```

## 空文件保护

空文件大小是：

```text
st.st_size == 0
```

不能直接映射 0 字节。

处理：

```c
if (st.st_size == 0) {
    printf("file empty\n");
    close(fd);
    return 0;
}
```

空文件不是错误，所以不应该 `perror("fstat")`。

## mmap 的意义

普通 `read` 模型：

```text
文件 -> read -> buffer -> 程序处理
```

`mmap` 模型：

```text
文件/设备 -> mmap -> 进程虚拟内存地址 -> 程序直接访问
```

今天映射的是普通文件。

以后嵌入式 Linux 常见映射对象：

```text
/dev/fb0 framebuffer 显示缓冲
V4L2 摄像头缓冲
共享内存
DMA buffer
驱动映射给用户态的内存
```

## read / write / ioctl / mmap 区别

```text
read   从文件或设备读取普通数据
write  向文件或设备写入普通数据
ioctl  对设备发送控制命令或查询状态
mmap   把文件或设备内存映射到进程地址空间
```

## Makefile

常用命令：

```bash
make clean
make
make run-winsize
make run-mmap
make run-bad
```

## 今日遇到的问题

1. 把程序输出 `rows:`、`cols:` 又粘贴回终端执行，导致 `command not found`。
2. 不理解 `show_mmap_file` 的作用。
3. 空文件处理一开始误写成 `perror("fstat")`。
4. Makefile 中目标一开始没有依赖源文件和 build 目录。

## 问题解释

### show_mmap_file 有什么用

它不是为了复杂地打印文件。

它的作用是演示：

```text
不用 read，而是用 mmap 把文件映射到内存，再从内存输出文件内容。
```

这是后续学习 framebuffer、摄像头、共享内存和驱动映射的基础。

### 为什么 Makefile 要写依赖

正确规则：

```makefile
$(ta): $(sr) | $(bu)
```

含义：

```text
目标程序依赖源文件。
编译前确保 build 目录存在。
源文件变化后，make 能正确重新编译。
```

## 今日总结

今天我学会了：

1. `ioctl` 可以对设备发送控制命令。
2. `TIOCGWINSZ` 可以查询终端窗口大小。
3. `mmap` 可以把文件映射成进程内存。
4. `munmap` 用于解除映射。
5. `read/write/ioctl/mmap` 是用户态访问文件和设备的不同方式。
6. 嵌入式中显示、摄像头、DMA、驱动接口会经常见到 `mmap/ioctl`。

今天还需要加强：

1. `mmap` 的虚拟内存模型。
2. `MAP_SHARED` 和 `MAP_PRIVATE` 的区别。
3. 真实设备驱动中 `ioctl` 命令设计。
