# Day 1 - 环境确认和第一个 Linux C 程序

日期：2026-07-08

## 今日目标

- [x] 确认 WSL/Ubuntu 能打开。
- [x] 确认 `gcc` 可用。
- [x] 编译 `hello_linux.c`。
- [x] 运行 `hello_linux`。
- [x] 使用 `file` 查看可执行文件。
- [x] 故意制造一个编译错误并记录报错。

## 今日命令记录

```bash
gcc --version
g++ --version
make --version
gdb --version
gcc -Wall -g hello_linux.c -o hello_linux
./hello_linux
file hello_linux
ls -l hello_linux
```

## 命令理解

### gcc

记录：gcc阐述的是用什么编译器编译，用c编译器

### -Wall

记录：开启常见警告

### -g

记录：生成调试信息

### -o

记录：指定输出文件名字

### ./

记录：当前目录

### file

记录：探查文件真实类型

## 今日输出结果

程序运行输出：
Hello, Linux embedded application!
Day 1: build, run, and inspect a Linux C program.


`file hello_linux` 输出：

```text
hello_linux: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=b6b58c2b685e8283f55b8dcf788d53d54e4c83e0, for GNU/Linux 3.2.0, with debug_info, not stripped
```

## 故意制造的错误

我改了哪里：我改了print("Hello, Linux embedded applicatio")后的;

编译器报错：

```text
hello_linux.c: In function ‘main’:
hello_linux.c:5:51: error: expected ‘;’ before ‘printf’
    5 |     printf("Hello, Linux embedded application!\n")
      |                                                   ^
      |                                                   ;
    6 |     printf("Day 1: build, run, and inspect a Linux C program.\n");
      |     ~~~~~~

```

我如何修复：加上;

## 今日总结

今天我学会了：

1.特定指令的基本含义

今天我还不理解：

1.无

明天 Day 2 要进入 gcc/g++ 编译流程：预处理、编译、汇编、链接。
