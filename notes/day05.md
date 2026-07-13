# Day 5 - gdb 调试

日期：2026-07-13

## 今日目标

- [x] 能构建带调试信息的程序。
- [x] 能使用 `gdb ./debug_demo` 启动调试。
- [x] 能使用 `run` 运行并传参。
- [x] 能使用 `break` 设置断点。
- [x] 能使用 `next` 单步执行。
- [x] 能使用 `step` 进入函数。
- [x] 能使用 `print` 查看变量。
- [x] 能使用 `backtrace` 查看调用栈。
- [x] 能定位一次段错误。

## 今日项目

项目目录：

```text
linux_projects/day05_gdb_debug
```

项目结构：

```text
CMakeLists.txt
include/device.h
src/device.c
src/main.c
```

## 构建记录

命令：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day05_gdb_debug
mkdir -p build
cd build
cmake ..
cmake --build .
```

输出中的关键信息：

```text
-- Configuring done
-- Generating done
-- Build files have been written to: /mnt/c/Users/20858/Documents/实习/linux_projects/day05_gdb_debug/buildConsolidate compiler generated dependencies of target debug_demo
[100%] Built target debug_demo

```

`file debug_demo` 输出：

```text
debug_demo: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=5b3e3fb82927b1e5e2a8f664617bddbc4d98813f, for GNU/Linux 3.2.0, with debug_info, not stripped

```

我对 `with debug_info` 的理解：with debug_info 表示可执行文件中包含调试信息。gdb 可以根据这些信息显示源码行号、函数名和变量名。如果没有 -g，gdb 可能只能看到地址或汇编，调试会困难很多。

## 正常运行

命令：

```bash
./debug_demo normal
```

输出：

```text
device id: 1001
temperature: 32
voltage: 24

```

## 崩溃运行

命令：

```bash
./debug_demo crash
```

输出：

```text
Segmentation fault (core dumped)
```

我对 segmentation fault 的理解：表示程序访问了不该访问的内存。本例中 crash_by_null_pointer 创建了 NULL 指针 status，并传给 print_device_status，函数内部访问 status->id 时发生空指针解引用，所以崩溃。

## gdb 定位崩溃

命令：

```bash
gdb ./debug_demo
```

gdb 内执行：

```gdb
run crash
backtrace
list
print status
```

`backtrace` 输出：

```text
#0  0x000055555555539c in print_device_status (status=0x0)
    at /mnt/c/Users/20858/Documents/实习/linux_projects/day05_gdb_debug/src/device.c:7
#1  0x00005555555554ab in crash_by_null_pointer ()
    at /mnt/c/Users/20858/Documents/实习/linux_projects/day05_gdb_debug/src/device.c:38
#2  0x0000555555555358 in main (argc=2, argv=0x7fffffffddf8)
    at /mnt/c/Users/20858/Documents/实习/linux_projects/day05_gdb_debug/src/main.c:48

```

`print status` 输出：

```text
$1 = (const DeviceStatus *) 0x0

```

崩溃原因：

## 断点和单步

gdb 命令：

```gdb
break main
break run_normal_demo
break calculate_average
run normal
print argc
print argv[1]
next
next
continue
```

我的观察：argc = 2，argv[1] = "normal"。如果程序没有先停在 main，或者已经运行结束，就无法 print argc/argv。调试时必须先 break main，再 run normal，让程序停在 main 的作用域里,在 strcmp、printf 这类库函数上不要随便使用 step，否则会进入库函数内部甚至汇编代码。一般用 next 跳过库函数；遇到自己写的函数时再用 step。。

## 逻辑错误调试

命令：

```bash
./debug_demo logic-bug
gdb ./debug_demo
```

gdb 内执行：

```gdb
break calculate_average_buggy
run logic-bug
next
print i
print sum
print count
```

我发现的问题：在 calculate_average_buggy 中，count = 4，数组 values 有 4 个元素，合法下标是 0、1、2、3。但循环条件写成 i <= count，导致 i = 4 时访问 values[4]，发生数组越界。这个错误不一定导致程序崩溃，但会读到不属于数组的数据，导致结果不可靠。正确写法应该是 i < count。

## 今日概念

### -g

记录：生成调试信息

### gdb

记录：Linux 下常用的命令行调试器，可以设置断点、单步执行、查看变量、查看调用栈，用来定位崩溃和逻辑错误。

### break

记录：设置断点

### run

记录：运行程序

### next

记录：执行下一行，不进入函数内部，适合跳过 printf、strcmp 等库函数。

### step

记录：执行下一行，如果遇到函数调用会进入函数内部，适合进入自己写的函数。

### print

记录：打印对应参数

### backtrace

记录：查看函数调用栈，能看出程序是沿着哪些函数调用走到崩溃位置的

### segmentation fault

记录：空指针错误

## 今日总结

今天我学会了：

1.debug指令

今天我还不理解：

1.无
明天 Day 6 要进入静态库和动态库：学习如何把功能封装成 `.a` 和 `.so`。
