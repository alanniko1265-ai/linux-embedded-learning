# Day 6 - 静态库和动态库

日期：2026-07-14

## 今日目标

- [x] 理解头文件和库文件的关系。
- [x] 能生成 `.o` 目标文件。
- [x] 能使用 `ar` 生成静态库 `.a`。
- [x] 能使用 `-fPIC` 和 `-shared` 生成动态库 `.so`。
- [x] 能链接静态库生成程序。
- [x] 能链接动态库生成程序。
- [x] 能使用 `ldd` 查看动态依赖。
- [x] 能理解 `LD_LIBRARY_PATH` 的作用。

## 今日项目

项目目录：

```text
linux_projects/day06_static_shared_lib
```

项目结构：

```text
Makefile
include/log.h
src/log.c
app/main.c
```

## 构建记录

### 进入目录

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day06_static_shared_lib
```

### 构建所有目标

```bash
make
```

输出：

```text
mkdir -p build
gcc -Wall -g -O0 -Iinclude -c src/log.c -o build/log.o
mkdir -p build/lib
ar rcs build/lib/liblog.a build/log.o
mkdir -p build/bin
gcc -Wall -g -O0 -Iinclude app/main.c -Lbuild/lib -llog -o build/bin/app_static
gcc -Wall -g -O0 -Iinclude -fPIC -c src/log.c -o build/log_pic.o
gcc -shared build/log_pic.o -o build/lib/liblog.so
gcc -Wall -g -O0 -Iinclude app/main.c -Lbuild/lib -llog -o build/bin/app_shared

```

生成了哪些文件：

```text
app_static和app_shared

```

## 静态库

### 手动命令理解

```bash
gcc -Wall -g -O0 -Iinclude -c src/log.c -o build/log.o
ar rcs build/lib/liblog.a build/log.o
gcc -Wall -g -O0 -Iinclude app/main.c -Lbuild/lib -llog -o build/bin/app_static
```

我的理解：将log.c打包成log.o,用ar把log.o打包成liblog.a,最后main.c链接liblog.a库,生成app_static

### 运行静态链接程序

```bash
make run-static
```

输出：

```text
./build/bin/app_static
[2026-07-14 09:23:06] [INFO] device gateway started
[2026-07-14 09:23:06] [WARN] serial device is not connected
[2026-07-14 09:23:06] [ERROR] failed to open config file
```

## 动态库

### 手动命令理解

```bash
gcc -Wall -g -O0 -Iinclude -fPIC -c src/log.c -o build/log_pic.o
gcc -shared build/log_pic.o -o build/lib/liblog.so
gcc -Wall -g -O0 -Iinclude app/main.c -Lbuild/lib -llog -o build/bin/app_shared
```

我的理解：先用 -fPIC 把 log.c 编译成适合动态库使用的 log_pic.o，再用 -shared 生成 liblog.so。主程序 app_shared 链接 liblog.so，但运行时还需要系统能找到这个动态库。

### 查看动态依赖

```bash
make check-shared
```

输出：

```text
ldd build/bin/app_shared
        linux-vdso.so.1 (0x00007ffc9afac000)
        liblog.so => not found
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x000071af5dc00000)
        /lib64/ld-linux-x86-64.so.2 (0x000071af5df84000)

```

### 运行动态链接程序

```bash
make run-shared
```

输出：

```text
LD_LIBRARY_PATH=build/lib ./build/bin/app_shared
[2026-07-14 09:41:17] [INFO] device gateway started
[2026-07-14 09:41:17] [WARN] serial device is not connected
[2026-07-14 09:41:17] [ERROR] failed to open config file

```

## 今日概念

### .a

记录：静态库，本质是多个 .o 目标文件的打包集合。链接静态库时，所需代码会被复制进最终可执行程序。

### .so

记录：动态库，也叫共享库。程序运行时动态加载它，多个程序可以共享同一个 .so。

### ar rcs

记录：ar 用来创建和管理静态库。rcs 表示创建/替换目标文件并生成索引。

### -fPIC

记录：生成位置无关代码，通常用于编译动态库需要的目标文件。

### -shared

记录：告诉 gcc 生成共享库 .so，而不是普通可执行文件。

### -L

记录：指定库文件搜索目录，比如 -Lbuild/lib。

### -l

记录：指定要链接的库名。-llog 会去找 liblog.a 或 liblog.so。

### ldd

记录：查看可执行文件运行时依赖哪些动态库。

### LD_LIBRARY_PATH

记录：临时指定动态库搜索路径，让程序运行时能找到自定义 .so 文件。

## 静态库和动态库区别

我的理解：

```text
静态库在链接时把需要的代码复制进可执行文件，程序运行时不依赖原来的 .a 文件；优点是部署简单，缺点是可执行文件可能更大，库更新后需要重新链接程序。

动态库在运行时被加载，程序本身不包含完整库代码；优点是多个程序可以共享同一个 .so，库可以单独更新，缺点是部署时必须保证程序能找到对应的 .so。
```

## 今日总结

今天我学会了：

1.链接静态库和动态库

今天我还不理解：

1.无

明天 Day 7 要做 Week 1 复盘和小项目。
