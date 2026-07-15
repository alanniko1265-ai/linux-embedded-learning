# Day 8 - 文件 IO 基础

日期：2026-07-15

## 今日目标

- [x] 理解文件描述符。
- [x] 能使用 `open` 打开文件。
- [x] 能使用 `read` 读取文件。
- [x] 能使用 `write` 输出内容。
- [x] 能使用 `close` 关闭文件。
- [x] 完成第一个系统调用版 `read_file`。
- [x] 扩展成 `my_cat <file>`。
- [x] 完成最小版 `my_cp <src> <dst>`。
- [x] 使用 Makefile 同时构建 `my_cat` 和 `my_cp`。

## 今日项目

项目目录：

```text
linux_projects/day08_file_io
```

代码结构：

```text
day08_file_io
├── Makefile
├── data
│   ├── input.txt
│   ├── big.txt
│   └── output_make.txt
└── src
    ├── read_file.c
    └── my_cp.c
```

## Step 1：读取固定文件

目标：

```bash
./build/read_file
```

读取：

```text
data/input.txt
```

并输出到终端。

### 我写的命令

```bash
gcc -Wall -g -O0 src/read_file.c -o build/read_file
./build/read_file
```

### 运行输出

```text
hello linux file io
this file is read by open/read/write/close
day08 starts system programming
```

## Step 2：改成命令行参数版本 my_cat

目标：

```bash
./build/my_cat data/input.txt
```

关键代码：

```c
int main(int argc, char *argv[])
```

参数关系：

```text
argc = 2
argv[0] = ./build/my_cat
argv[1] = data/input.txt
```

`argv[0]` 是程序启动名，主要用于 usage 提示；真正的业务参数通常从 `argv[1]` 开始。

## Step 3：加入 perror

原来的错误提示：

```c
printf("open file failed\n");
```

改成：

```c
perror("open");
```

故意读取不存在的文件：

```bash
./build/my_cat data/not_exist.txt
```

输出：

```text
open: No such file or directory
```

这说明 `open` 调用失败，失败原因是文件或目录不存在。

## Step 4：循环读取完整文件

不能只调用一次 `read`，因为文件可能比缓冲区大。

关键代码：

```c
while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
    write(STDOUT_FILENO, buffer, n);
}
```

`read` 返回值含义：

```text
n > 0   实际读到 n 个字节
n == 0  文件读完，也就是 EOF
n < 0   读取失败，要看 perror/errno
```

测试大文件：

```bash
seq 1 300 > data/big.txt
./build/my_cat data/big.txt
```

结果：能完整输出 `1` 到 `300`。

## Step 5：完成最小版 my_cp

目标：

```bash
./build/my_cp data/input.txt data/output.txt
./build/my_cat data/output.txt
```

核心流程：

```text
open 源文件 -> open/creat 目标文件 -> read 源文件 -> write 目标文件 -> close 两个文件
```

目标文件打开方式：

```c
open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

含义：

```text
O_WRONLY  只写
O_CREAT   文件不存在就创建
O_TRUNC   文件存在就清空后重新写入
0644      新文件权限
```

## Step 6：检查 write 返回值

不能只调用 `write` 而不检查返回值。

关键代码：

```c
written = write(dst_fd, buffer, n);
if (written < 0) {
    perror("write");
    close(src_fd);
    close(dst_fd);
    return 1;
}

if (written != n) {
    printf("write incomplete\n");
    close(src_fd);
    close(dst_fd);
    return 1;
}
```

结论：

```text
read 返回值表示实际读到多少字节
write 返回值表示实际写入多少字节
```

普通文件里 `written != n` 不常见，但串口、socket、管道里会更常见。

## Step 7：使用 Makefile 管理两个程序

构建关系：

```text
src/read_file.c -> build/my_cat
src/my_cp.c     -> build/my_cp
```

常用命令：

```bash
make clean
make
make run-cat
make run-cp
```

`make run-cp` 输出：

```text
./build/my_cp data/input.txt data/output_make.txt
./build/my_cat data/output_make.txt
hello linux file io
this file is read by open/read/write/close
day08 starts system programming
```

## 今日概念

### 文件描述符

文件描述符是 Linux 内核给进程打开文件后返回的整数编号。后续 `read`、`write`、`close` 都通过这个编号操作文件。

### open

打开文件，成功返回文件描述符，失败返回 `-1`。

### read

从文件描述符读取数据到缓冲区。返回实际读取的字节数。

### write

把缓冲区的数据写入文件描述符。返回实际写入的字节数。

### close

关闭文件描述符，释放进程占用的文件资源。

### STDOUT_FILENO

标准输出的文件描述符，值通常是 `1`。使用：

```c
write(STDOUT_FILENO, buffer, n);
```

可以直接把数据写到终端。

### argc 和 argv

`argc` 是参数个数，`argv` 是参数数组。

运行：

```bash
./build/my_cp data/input.txt data/output.txt
```

对应：

```text
argc = 3
argv[0] = ./build/my_cp
argv[1] = data/input.txt
argv[2] = data/output.txt
```

## 遇到的问题

1. 不理解为什么运行 `./build/my_cat data/big.txt` 时前面必须写程序名。
2. 不理解 `argv[0]` 在当前程序中为什么看起来没有实际业务作用。
3. `make run-cp` 时，`my_cp` 打印了文件内容，却没有创建 `output_make.txt`。

## 我如何解决

1. 明确命令结构：`./build/my_cat` 是要运行的程序，`data/big.txt` 是传给程序的参数。
2. 明确 `argv[0]` 是程序启动名，常用于 usage 提示，业务参数通常从 `argv[1]` 开始。
3. 检查 Makefile 后发现 `$(MY_CP)` 规则错误地使用了 `src/read_file.c`，导致 `build/my_cp` 实际上也是 `my_cat` 的代码。修正为 `src/my_cp.c` 后问题解决。

## 今日总结

今天我学会了：

1. 用 `open/read/write/close` 完成 Linux 文件读取。
2. 用 `argc/argv` 写命令行工具。
3. 用 `perror` 查看系统调用失败原因。
4. 用循环读取处理超过缓冲区大小的文件。
5. 写最小版 `my_cp`，并检查 `write` 的返回值。
6. 用 Makefile 同时构建多个可执行程序。

今天我还需要继续加强：

1. Makefile 目标、依赖、命令三者之间的对应关系。
2. `write` 不完整写入时如何更稳妥地循环写完。

明天 Day 9 会继续做文件 IO 工程化：封装安全读写函数，并加入更完整的错误处理。
