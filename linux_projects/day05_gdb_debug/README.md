# Day 5 - gdb 调试

今日目标：学会用 gdb 定位程序崩溃、查看调用栈、查看变量值，并初步理解调试信息的作用。

## 项目结构

```text
day05_gdb_debug/
├── CMakeLists.txt
├── include/
│   └── device.h
├── src/
│   ├── device.c
│   └── main.c
└── README.md
```

## 今日要解决的问题

这个项目故意准备了三种运行模式：

```text
normal      正常运行
logic-bug   程序不崩溃，但结果可能不对
crash       程序直接段错误
```

今天你要学会：

- 编译带 `-g` 调试信息的程序。
- 用 `gdb ./debug_demo` 启动调试。
- 用 `run` 传参数运行程序。
- 用 `break` 设置断点。
- 用 `next` 单步执行但不进入函数。
- 用 `step` 单步执行并进入函数。
- 用 `print` 查看变量。
- 用 `backtrace` 查看调用栈。
- 用 `list` 查看附近源码。
- 用 `quit` 退出 gdb。

## 第一步：构建项目

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day05_gdb_debug
mkdir -p build
cd build
cmake ..
cmake --build .
```

确认程序存在：

```bash
ls -lh debug_demo
file debug_demo
```

`file` 输出里应该能看到：

```text
with debug_info
```

这说明 `-g` 生效了。

## 第二步：正常运行

```bash
./debug_demo normal
```

预期输出：

```text
device id: 1001
temperature: 32
voltage: 24
```

## 第三步：运行崩溃版本

```bash
./debug_demo crash
```

你会看到类似：

```text
Segmentation fault
```

段错误的意思是：

```text
程序访问了不该访问的内存。
```

本项目里是因为空指针：

```c
DeviceStatus *status = NULL;
print_device_status(status);
```

`print_device_status` 里又访问了：

```c
status->id
```

所以崩溃。

## 第四步：用 gdb 定位崩溃

进入 gdb：

```bash
gdb ./debug_demo
```

在 gdb 里运行：

```gdb
run crash
```

程序崩溃后执行：

```gdb
backtrace
```

你应该看到类似调用栈：

```text
#0  print_device_status
#1  crash_by_null_pointer
#2  main
```

理解：

```text
main 调用了 crash_by_null_pointer
crash_by_null_pointer 调用了 print_device_status
print_device_status 访问空指针时崩溃
```

查看当前崩溃位置附近代码：

```gdb
list
```

查看变量：

```gdb
print status
```

如果显示：

```text
$1 = (DeviceStatus *) 0x0
```

说明它是空指针。

退出：

```gdb
quit
```

## 第五步：练习断点和单步

重新进入：

```bash
gdb ./debug_demo
```

设置断点：

```gdb
break main
break run_normal_demo
break calculate_average
```

运行：

```gdb
run normal
```

常用命令：

```gdb
next
step
print argc
print argv[1]
continue
```

理解：

- `next`：执行下一行，如果这一行是函数调用，不进入函数内部。
- `step`：执行下一行，如果这一行是函数调用，会进入函数内部。
- `continue`：继续运行到下一个断点或程序结束。

退出：

```gdb
quit
```

## 第六步：调试逻辑错误

普通运行：

```bash
./debug_demo logic-bug
```

进入 gdb：

```bash
gdb ./debug_demo
```

设置断点：

```gdb
break calculate_average_buggy
run logic-bug
```

逐行执行：

```gdb
next
print i
print sum
print count
```

你要观察这行：

```c
for (int i = 0; i <= count; i++) {
```

问题在于：

```text
i <= count 会访问 values[count]
```

数组合法下标是：

```text
0 到 count - 1
```

所以这里应该改成：

```c
for (int i = 0; i < count; i++) {
```

今天先不急着改代码，重点是用 gdb 观察问题。

## gdb 常用命令表

```text
gdb ./program        启动 gdb
run args            运行程序并传参数
break main          在 main 函数设置断点
break file.c:10     在指定文件行号设置断点
next                下一行，不进入函数
step                下一行，进入函数
continue            继续运行
print var           打印变量
backtrace           查看调用栈
list                查看附近源码
info locals         查看局部变量
info args           查看函数参数
quit                退出 gdb
```

## 今日验收

你能回答：

1. `-g` 为什么重要？
2. 什么是 segmentation fault？
3. `backtrace` 能告诉你什么？
4. `next` 和 `step` 有什么区别？
5. `print status` 显示 `0x0` 说明什么？
6. `logic-bug` 模式为什么可能算错？
