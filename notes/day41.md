# Day41 交叉编译入门：WSL 编译 ARM 程序并在 i.MX6ULL 运行

## 今日目标

Day41 的目标是打通嵌入式 Linux 应用开发的最小闭环：

```text
WSL 主机编写 C 程序
使用 ARM 交叉编译器生成 ARM 可执行文件
通过 scp 上传到 i.MX6ULL 开发板
在开发板上运行程序
```

这一步标志着学习重点从“只在电脑上写 Linux 程序”进入“主机开发、目标板运行”的嵌入式 Linux 开发方式。

## 开发环境角色

```text
Host 主机：
  WSL Ubuntu，架构 x86_64

Target 目标板：
  野火 EBF6ULL S1 Pro，架构 ARMv7 / armv7l

Toolchain 交叉工具链：
  arm-linux-gnueabihf-gcc
```

主机和开发板 CPU 架构不同，所以不能直接把 WSL 上的 x86 程序拿到开发板运行。

## 示例程序

文件路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day41_cross_compile\hello_board.c
```

程序内容：

```c
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("Hello, i.MX6ULL board!\n");
    printf("This program is built for embedded Linux.\n");
    printf("pid=%d\n", getpid());

    return 0;
}
```

`getpid()` 用来输出当前进程号，说明程序已经作为 Linux 进程在开发板系统中运行。

## 编译结果对比

在 WSL 中编译得到三个程序：

```text
hello_x86
hello_arm
hello_arm_static
```

查看大小：

```text
hello_arm          9.0K
hello_arm_static   452K
hello_x86          18K
```

查看文件类型：

```text
hello_x86:
  ELF 64-bit，x86-64，动态链接

hello_arm:
  ELF 32-bit，ARM，动态链接

hello_arm_static:
  ELF 32-bit，ARM，静态链接
```

结论：

```text
hello_x86 是给 WSL / PC 运行的
hello_arm 是给 ARM 开发板运行的动态链接版本
hello_arm_static 是给 ARM 开发板运行的静态链接版本
```

## 动态链接版本的问题

直接运行 `hello_arm` 时，开发板报错：

```text
/lib/arm-linux-gnueabihf/libc.so.6: version `GLIBC_2.34' not found
```

原因：

```text
hello_arm 是在 WSL Ubuntu 22.04 工具链下编译的
它需要较新的 glibc 版本
开发板 Debian 10 上的 glibc 版本较旧
所以程序架构正确，但运行库版本不匹配
```

这不是代码错误，而是交叉编译环境和开发板 rootfs 不匹配。

## 静态链接版本上板运行

使用静态编译生成：

```bash
arm-linux-gnueabihf-gcc -Wall -g -O0 -static hello_board.c -o hello_arm_static
```

上传到开发板：

```bash
scp hello_arm_static debian@192.168.7.2:/home/debian/apps/day41_cross_compile/
```

在开发板运行：

```bash
/home/debian/apps/day41_cross_compile/hello_arm_static
```

运行结果：

```text
Hello, i.MX6ULL board!
This program is built for embedded Linux.
pid=681
```

说明交叉编译和上板运行链路已经打通。

## Makefile

文件路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day41_cross_compile\Makefile
```

当前 Makefile 可以一键生成三个目标：

```text
hello_x86
hello_arm
hello_arm_static
```

核心理解：

```text
同一份源码 + 不同编译器 = 不同架构的可执行程序
```

建议后续把 `.PHONY` 写完整：

```makefile
.PHONY: all clean
```

这样即使目录中出现名为 `clean` 的文件，`make clean` 仍然会按清理命令执行。

## 今日结论

今天完成了嵌入式 Linux 应用开发的关键闭环：

```text
写代码
交叉编译
识别 ELF 架构
处理动态库版本问题
上传开发板
在 ARM Linux 上运行
```

短期学习阶段可以使用静态链接快速验证程序。

长期工程项目更推荐使用和开发板系统匹配的 SDK、sysroot 或厂家工具链，避免 glibc 版本不一致。

