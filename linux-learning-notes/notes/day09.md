# Day 9 - 从零实现安全文件复制

日期：2026-07-15

## 今日原则

今天不提前给完整工程壳。

每一步只做一件事：

1. 我先创建或修改一个文件。
2. 我先写核心代码。
3. Codex 检查。
4. 再进入下一步。

## 今日目标

- [x] 创建 Day09 工程目录。
- [x] 自己写 `file_utils.h`。
- [x] 自己写 `write_all` 函数声明。
- [x] 自己写 `file_utils.c`。
- [x] 自己实现 `write_all`。
- [x] 自己写 `main.c` 调用 `write_all`。
- [x] 自己写 Makefile。
- [x] 编译并运行 `safe_cp`。

## 项目目录

```text
linux_projects/day09_file_io_utils
```

## VS Code 打开命令

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day09_file_io_utils
code .
```

## 项目结构

```text
day09_file_io_utils
├── Makefile
├── data
│   ├── input.txt
│   └── output.txt
├── include
│   └── file_utils.h
└── src
    ├── file_utils.c
    └── main.c
```

## 今日任务

Day08 的 `my_cp` 是直接调用 `write`：

```c
write(dst_fd, buffer, n);
```

Day09 的 `safe_cp` 把写入逻辑封装成：

```c
write_all(dst_fd, buffer, n);
```

这样主流程更清晰，也能处理 `write` 没有一次写完的情况。

## 核心函数：write_all

函数声明：

```c
ssize_t write_all(int fd, const void *buffer, size_t count);
```

作用：

```text
尽量把 buffer 中的 count 个字节全部写入 fd。
如果一次 write 没有写完，就从剩余位置继续写。
如果 write 出错，返回 -1。
```

关键逻辑：

```c
const char *p = buffer;
size_t total = 0;

while (total < count) {
    ssize_t written;

    written = write(fd, p + total, count - total);
    if (written < 0) {
        return -1;
    }

    total += written;
}

return (ssize_t)total;
```

## 今日概念

### 头文件保护

标准写法：

```c
#ifndef FILE_UTILS_H
#define FILE_UTILS_H

// declarations

#endif
```

含义：

```text
#ifndef 负责检查有没有标记
#define 负责打上标记
#endif 负责结束保护范围
```

这样可以防止同一个头文件被重复包含。

### const void *buffer

`write_all` 不直接写成 `const char *buffer`，是为了更通用。

`const void *` 表示：

```text
我不关心传进来的是什么类型，只把它当作一段内存。
```

所以以后不仅能写字符串，也能写结构体、数组、二进制数据。

### const char *p = buffer

函数内部需要用：

```c
p + total
```

做字节级偏移。

`void *` 不能直接做标准 C 指针运算，所以先转成 `const char *`。

### p + total

表示从已经写完的位置继续写。

比如 `count = 100`，第一次写了 40 字节：

```text
total = 40
p + total = p + 40
count - total = 60
```

下一次就从第 40 字节后面继续写剩下的 60 字节。

### Makefile 变量和目标

变量定义：

```makefile
BUILD := build
ta := $(BUILD)/safe_cp
```

使用变量：

```makefile
all: $(ta)
```

这里不是重新定义 `ta`，而是把 `$(ta)` 展开为：

```text
build/safe_cp
```

目标规则：

```makefile
$(ta): $(SRC) | $(BUILD)
	$(cc) $(st) $(SRC) -o $(ta)
```

含义：

```text
如果源文件变化，就重新生成 build/safe_cp。
生成前先确保 build 目录存在。
```

## 编译和运行

```bash
make clean
make
make run
cat data/output.txt
make check
```

`make run` 执行：

```bash
./build/safe_cp data/input.txt data/output.txt
```

这个命令成功时不会输出到终端，因为 `safe_cp` 的功能是复制文件，不是打印文件。

查看复制结果：

```bash
cat data/output.txt
```

比较两个文件是否一致：

```bash
cmp data/input.txt data/output.txt
```

如果 `cmp` 没有任何输出，表示两个文件完全一致。

## 遇到的问题

1. `file_utils.h` 里一开始把 `#ifndef` 写成了 `#ifdef`。
2. `types.h` 和 `write_all` 出现拼写错误。
3. `file_utils.c` 文件名一开始写成了 `file_untils.c`。
4. `main.c` 中 `return 1` 写在 `close` 前面，导致 `close` 永远不会执行。
5. 打开目标文件失败时，错误提示一开始写成了 `open src`。
6. Makefile 中 `$(BUILD)` 规则一开始漏了冒号。
7. `cmp` 路径一开始写成了 `output.txt`，实际应该是 `data/output.txt`。

## 今日总结

今天我学会了：

1. 从零创建 `file_utils.h`、`file_utils.c`、`main.c` 和 Makefile。
2. 用 `write_all` 封装更可靠的写入逻辑。
3. 理解 `.h` 声明、`.c` 实现、`main.c` 调用之间的关系。
4. 理解 `const void *` 对外通用，`const char *` 对内方便按字节偏移。
5. 理解 `safe_cp` 成功时没有终端输出是正常现象。
6. 用 `cmp` 判断复制前后的两个文件是否完全一致。

今天还需要继续加强：

1. Makefile 的目标、依赖、命令三者关系。
2. 多文件项目中函数声明和函数实现的对应关系。
