# Day 11 - 目录遍历：dir_scan

日期：2026-07-16

## 今日目标

- [x] 创建 `dir_scan` 项目。
- [x] 使用 `opendir` 打开目录。
- [x] 使用 `readdir` 遍历目录项。
- [x] 使用 `closedir` 关闭目录。
- [x] 理解 `.` 和 `..` 的含义。
- [x] 过滤 `.` 和 `..`。
- [x] 将目录路径和文件名拼成完整路径。
- [x] 结合 `stat` 输出文件类型和大小。
- [x] 编写 Makefile 构建项目。

## 项目目录

```text
linux_projects/day11_dir_scan
```

## VS Code 打开命令

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day11_dir_scan
code .
```

## 项目结构

```text
day11_dir_scan
├── Makefile
├── data
│   ├── config.txt
│   ├── empty.txt
│   ├── log.txt
│   └── subdir
└── src
    └── main.c
```

## 今日程序

程序名：

```text
dir_scan
```

运行方式：

```bash
./build/dir_scan <dir>
```

示例：

```bash
./build/dir_scan data
```

输出类似：

```text
[file] data/config.txt 17 bytes
[file] data/empty.txt 0 bytes
[file] data/log.txt 11 bytes
[dir ] data/subdir
```

## 核心 API

### opendir

```c
DIR *dir = opendir(argv[1]);
```

作用：

```text
打开一个目录，成功返回 DIR*，失败返回 NULL。
```

失败时使用：

```c
perror("opendir");
```

### readdir

```c
struct dirent *entry;

while ((entry = readdir(dir)) != NULL) {
    ...
}
```

作用：

```text
每次读取目录中的一个目录项。
```

`entry->d_name` 是目录项的名字，比如：

```text
config.txt
log.txt
subdir
```

注意：

```text
readdir 只返回当前目录第一层的名字。
它不会递归进入子目录。
它也不会自动展开上级目录。
输出顺序不保证是字母序。
```

### closedir

```c
closedir(dir);
```

作用：

```text
关闭目录，释放资源。
```

## `.` 和 `..`

当扫描：

```bash
./build/dir_scan data
```

程序打开的是：

```text
data
```

`readdir` 可能读到：

```text
.
..
config.txt
empty.txt
log.txt
subdir
```

含义：

```text
.   表示当前目录，也就是 data 自己
..  表示当前目录的上一级，也就是 day11_dir_scan
```

它们只是目录项名字，不会自动展开。

对应关系：

```text
.           data/.
..          data/..
config.txt  data/config.txt
subdir      data/subdir
```

普通 `ls data` 默认不显示 `.` 和 `..`。

如果执行：

```bash
ls -a data
```

就能看到它们。

## 过滤 `.` 和 `..`

需要包含：

```c
#include <string.h>
```

过滤逻辑：

```c
if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
    continue;
}
```

`continue` 的意思：

```text
跳过本轮循环，继续读取下一个目录项。
```

## 拼接完整路径

`entry->d_name` 只有名字，不包含目录路径。

比如：

```text
config.txt
```

为了后面调用 `stat`，需要拼成：

```text
data/config.txt
```

使用：

```c
snprintf(full_path, sizeof(full_path), "%s/%s", argv[1], entry->d_name);
```

含义：

```text
argv[1]        目录路径，例如 data
entry->d_name  目录项名字，例如 config.txt
full_path      拼接后的完整路径，例如 data/config.txt
```

## 结合 stat

头文件：

```c
#include <sys/stat.h>
```

调用：

```c
struct stat st;

if (stat(full_path, &st) < 0) {
    perror("stat");
    continue;
}
```

这里使用 `continue`，不是 `return 1`。

原因：

```text
扫描目录时，某一个条目 stat 失败，不应该让整个目录扫描直接结束。
应该跳过这个条目，继续扫描其他条目。
```

## 类型和大小输出

普通文件：

```c
if (S_ISREG(st.st_mode)) {
    printf("[file] %s %ld bytes\n", full_path, st.st_size);
}
```

目录：

```c
else if (S_ISDIR(st.st_mode)) {
    printf("[dir ] %s\n", full_path);
}
```

其他类型：

```c
else {
    printf("[other] %s\n", full_path);
}
```

## Makefile

常用命令：

```bash
make clean
make
make run
make run-m
```

`make run`：

```bash
./build/dir_scan data
```

`make run-m`：

```bash
./build/dir_scan not_exist_dir
```

用于测试目录不存在时的错误处理。

## 今日遇到的问题

1. 不理解为什么扫描 `data` 时会显示 `.` 和 `..`。
2. 不清楚 `readdir` 的显示顺序和扫描范围。
3. `PATH_MAX` 在 VS Code 中出现未识别红线。
4. Makefile 中把变量使用写成 `$ta`，实际应该是 `$(ta)`。
5. Makefile 中 `:=` 误写为 `:==`。

## 问题解释

### readdir 的扫描范围

`readdir` 只读取被 `opendir` 打开的目录的第一层目录项。

它不会递归扫描：

```text
subdir
```

也不会自动展开：

```text
..
```

### readdir 的顺序

`readdir` 不保证按字母顺序输出。

输出顺序取决于文件系统内部保存目录项的顺序。

如果以后需要排序，需要自己把目录项保存起来，再排序。

### PATH_MAX

`PATH_MAX` 来自：

```c
#include <limits.h>
```

如果 VS Code 有红线，但 gcc 能编译，可能是 IntelliSense 环境识别问题。

当前阶段也可以先使用：

```c
char full_path[1024];
```

重点是理解完整路径拼接。

## 今日总结

今天我学会了：

1. 使用 `opendir/readdir/closedir` 遍历目录。
2. 理解 `.` 是当前目录，`..` 是上一级目录。
3. 使用 `strcmp` 过滤 `.` 和 `..`。
4. 使用 `snprintf` 拼接完整路径。
5. 结合 Day10 的 `stat`，输出目录项类型和大小。
6. 理解目录扫描不会自动递归。

今天还需要加强：

1. 路径拼接和缓冲区大小处理。
2. Makefile 变量定义和变量使用。
3. 目录递归扫描的思路。
