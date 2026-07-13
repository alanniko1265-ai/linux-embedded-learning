# Day 5 - gdb 调试

日期：2026-07-13

## 今日目标

- [ ] 能构建带调试信息的程序。
- [ ] 能使用 `gdb ./debug_demo` 启动调试。
- [ ] 能使用 `run` 运行并传参。
- [ ] 能使用 `break` 设置断点。
- [ ] 能使用 `next` 单步执行。
- [ ] 能使用 `step` 进入函数。
- [ ] 能使用 `print` 查看变量。
- [ ] 能使用 `backtrace` 查看调用栈。
- [ ] 能定位一次段错误。

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

```

`file debug_demo` 输出：

```text

```

我对 `with debug_info` 的理解：

## 正常运行

命令：

```bash
./debug_demo normal
```

输出：

```text

```

## 崩溃运行

命令：

```bash
./debug_demo crash
```

输出：

```text

```

我对 segmentation fault 的理解：

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

```

`print status` 输出：

```text

```

崩溃原因：

## 断点和单步

gdb 命令：

```gdb
break main
break run_normal_demo
break calculate_average
run normal
next
step
print argc
print argv[1]
continue
```

我的观察：

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

我发现的问题：

## 今日概念

### -g

记录：

### gdb

记录：

### break

记录：

### run

记录：

### next

记录：

### step

记录：

### print

记录：

### backtrace

记录：

### segmentation fault

记录：

## 今日总结

今天我学会了：

1.
2.
3.

今天我还不理解：

1.
2.

明天 Day 6 要进入静态库和动态库：学习如何把功能封装成 `.a` 和 `.so`。
