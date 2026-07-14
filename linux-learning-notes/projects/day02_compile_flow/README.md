# Day 2 - gcc 编译流程

今日目标：理解 C 程序从源文件变成可执行文件的过程。

核心链路：

```text
.c 源文件 -> .i 预处理文件 -> .s 汇编文件 -> .o 目标文件 -> 可执行文件
```

## 项目结构

```text
day02_compile_flow/
├── include/
│   └── add.h
├── src/
│   ├── add.c
│   └── main.c
└── README.md
```

## 进入目录

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day02_compile_flow
mkdir -p build
```

## 1. 一步到位编译

先确认程序能正常编译运行：

```bash
gcc -Wall -g -O0 -Iinclude src/main.c src/add.c -o build/calc_demo
./build/calc_demo
```

预期输出：

```text
20 + 8 = 28
20 - 8 = 12
```

## 2. 预处理：生成 .i 文件

```bash
gcc -E -Iinclude src/main.c -o build/main.i
```

查看前几行：

```bash
head build/main.i
```

理解：

- `-E` 表示只做预处理。
- 预处理会展开 `#include`。
- 预处理会处理宏定义。
- `.i` 文件通常很大，因为系统头文件也被展开了。

## 3. 编译：生成 .s 汇编文件

```bash
gcc -S -Iinclude src/main.c -o build/main.s
```

查看：

```bash
head build/main.s
```

理解：

- `-S` 表示生成汇编文件。
- `.s` 文件是汇编代码。
- 这一步还没有生成机器码。

## 4. 汇编：生成 .o 目标文件

```bash
gcc -c -Wall -g -O0 -Iinclude src/main.c -o build/main.o
gcc -c -Wall -g -O0 -Iinclude src/add.c -o build/add.o
```

查看：

```bash
ls -l build/*.o
file build/main.o
file build/add.o
```

理解：

- `-c` 表示只编译成目标文件，不链接。
- `.o` 文件已经是二进制目标文件。
- 单个 `.o` 文件通常还不能直接运行。

## 5. 链接：生成可执行文件

```bash
gcc build/main.o build/add.o -o build/calc_demo
./build/calc_demo
```

理解：

- 链接器会把多个 `.o` 文件合成一个可执行文件。
- `main.o` 里调用了 `add` 和 `sub`。
- `add.o` 里提供了 `add` 和 `sub` 的实现。

## 6. 故意制造链接错误

只链接 `main.o`：

```bash
gcc build/main.o -o build/bad_demo
```

你应该会看到类似：

```text
undefined reference to `add'
undefined reference to `sub'
```

这是链接错误，不是语法错误。

## 今日必须理解

- `#include "add.h"` 只是让编译器知道函数声明。
- `add.c` 里才是真正的函数实现。
- 编译每个 `.c` 文件会得到 `.o` 文件。
- 链接时必须把需要的 `.o` 都交给 gcc。
- `-Iinclude` 是告诉编译器去 `include` 目录找头文件。

## 今日验收

你能回答：

1. `.c`、`.i`、`.s`、`.o`、可执行文件分别是什么？
2. `-E`、`-S`、`-c` 分别停在哪一步？
3. `add.h` 和 `add.c` 的区别是什么？
4. 为什么只链接 `main.o` 会报 `undefined reference`？
5. `-Iinclude` 的作用是什么？
