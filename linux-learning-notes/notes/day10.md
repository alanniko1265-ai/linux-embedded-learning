# Day 10 - Linux 文件属性与目录识别

日期：2026-07-16

## 今日目标

- [x] 创建 `file_probe` 项目。
- [x] 使用 `argc/argv` 接收路径参数。
- [x] 使用 `stat` 获取路径属性。
- [x] 判断路径是否存在。
- [x] 输出文件大小。
- [x] 判断普通文件、目录、其他类型。
- [x] 读取 `st_mode` 中的权限位。
- [x] 输出类似 `ls -l` 第一列的 mode 信息。
- [x] 编写 Makefile 管理构建和运行。

## 项目目录

```text
linux_projects/day10_file_stat
```

## VS Code 打开命令

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day10_file_stat
code .
```

## 项目结构

```text
day10_file_stat
├── Makefile
├── data
│   └── input.txt
└── src
    └── main.c
```

## 今日程序

程序名：

```text
file_probe
```

运行方式：

```bash
./build/file_probe <path>
```

示例：

```bash
./build/file_probe data/input.txt
./build/file_probe data
./build/file_probe data/not_exist.txt
```

## 核心系统调用：stat

头文件：

```c
#include <sys/stat.h>
```

核心变量：

```c
struct stat st;
```

调用方式：

```c
if (stat(argv[1], &st) < 0) {
    perror("stat");
    return 1;
}
```

含义：

```text
stat(argv[1], &st) 会读取 argv[1] 对应路径的属性，
并把结果填入 st 这个结构体。
```

如果路径不存在，会失败并通过 `perror("stat")` 输出原因。

## 输出文件大小

使用：

```c
st.st_size
```

输出：

```c
printf("size: %ld bytes\n", st.st_size);
```

## 判断文件类型

使用：

```c
S_ISREG(st.st_mode)
S_ISDIR(st.st_mode)
```

含义：

```text
S_ISREG：判断是否为普通文件
S_ISDIR：判断是否为目录
```

输出逻辑：

```text
regular file  普通文件
directory     目录
other         其他类型
```

## 输出 mode

`mode` 输出类似：

```text
-rwxrwxrwx
drwxrwxrwx
```

第一位表示文件类型：

```text
-  普通文件
d  目录
?  其他类型
```

后 9 位表示权限：

```text
rwx rwx rwx
owner group other
```

对应宏：

```text
S_IRUSR S_IWUSR S_IXUSR   owner
S_IRGRP S_IWGRP S_IXGRP   group
S_IROTH S_IWOTH S_IXOTH   other
```

判断方式：

```c
(st.st_mode & S_IRUSR) ? 'r' : '-'
```

含义：

```text
如果 owner read 这一位存在，就输出 r，否则输出 -。
```

## 编译和运行

```bash
make clean
make
make run-file
make run-dir
make run-missing
```

`make run-file`：

```bash
./build/file_probe data/input.txt
```

用于测试普通文件。

`make run-dir`：

```bash
./build/file_probe data
```

用于测试目录。

`make run-missing`：

```bash
./build/file_probe data/not_exist.txt
```

用于测试不存在路径。

不存在路径输出类似：

```text
stat: No such file or directory
```

这是正常现象，说明错误处理生效。

## 今日遇到的问题

1. `input.txt` 是文本文件，但显示有执行权限。
2. `%c` 配合了 `"r"` 这种字符串，导致编译 warning。
3. 一开始只输出 owner 权限，后来扩展成完整 9 位权限。
4. `permission` 输出包含文件类型位后，更准确的名字应该是 `mode`。

## 问题解释

### 为什么文本文件显示可执行

项目位于：

```text
/mnt/c/Users/20858/Documents/实习
```

这是 WSL 挂载 Windows C 盘的位置。`/mnt/c` 下的权限可能被映射成比较宽的形式，所以普通文本文件也可能显示 `x` 权限。

在真实 Linux 文件系统或嵌入式开发板上，权限会更接近实际情况。

### `'r'` 和 `"r"` 的区别

```text
'r'  是字符，配合 %c
"r"  是字符串，配合 %s
```

所以：

```c
printf("%c\n", 'r');
```

是正确的。

## 今日总结

今天我学会了：

1. 使用 `stat` 获取文件或目录属性。
2. 使用 `st.st_size` 获取文件大小。
3. 使用 `S_ISREG`、`S_ISDIR` 判断路径类型。
4. 使用 `st.st_mode` 和权限宏判断 rwx 权限。
5. 把文件类型和权限组合成类似 `ls -l` 的 mode 输出。
6. 理解 `/mnt/c` 下权限显示可能和真实 Linux 文件系统不同。

今天还需要加强：

1. `st_mode` 中类型位和权限位的关系。
2. Makefile 目标命名和格式的规范性。
