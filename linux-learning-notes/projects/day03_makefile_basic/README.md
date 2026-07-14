# Day 3 - Makefile 入门

今日目标：把 Day 2 手动执行的编译命令交给 Makefile 管理。

核心理解：

```text
目标: 依赖
	命令
```

注意：Makefile 命令前面必须是 Tab，不是普通空格。

## 项目结构

```text
day03_makefile_basic/
├── Makefile
├── include/
│   └── calc.h
├── src/
│   ├── calc.c
│   └── main.c
└── README.md
```

## 进入目录

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day03_makefile_basic
```

## 第一步：查看文件

```bash
find . -maxdepth 3 -type f
cat Makefile
cat include/calc.h
cat src/calc.c
cat src/main.c
```

## 第二步：直接构建

```bash
make
```

你应该看到类似：

```text
mkdir -p build
gcc -Wall -g -O0 -Iinclude -c src/main.c -o build/main.o
gcc -Wall -g -O0 -Iinclude -c src/calc.c -o build/calc.o
gcc build/main.o build/calc.o -o build/calc_tool
```

运行：

```bash
./build/calc_tool
```

预期输出：

```text
calc tool
12 + 4 = 16
12 - 4 = 8
12 * 4 = 48
```

## 第三步：使用 make run

```bash
make run
```

理解：

- `run` 依赖 `$(TARGET)`。
- 如果程序还没编译，`make run` 会先编译。
- 编译完成后再运行 `./build/calc_tool`。

## 第四步：观察增量编译

先执行一次：

```bash
make
```

如果没有修改文件，通常会看到：

```text
make: Nothing to be done for 'all'.
```

修改 `src/main.c`：

```bash
nano src/main.c
```

随便改一行输出文字，保存后执行：

```bash
make
```

这时应该只重新编译 `main.c`，不会重新编译 `calc.c`。

## 第五步：清理

```bash
make clean
ls
```

理解：

- `clean` 会删除 `build` 目录。
- 编译产物属于可再生成文件。
- 源代码不应该被 `clean` 删除。

重新构建：

```bash
make
make run
```

## 第六步：rebuild

```bash
make rebuild
```

理解：

- `rebuild` 依赖 `clean all`。
- 会先清理，再完整编译。

## Makefile 重点解释

```makefile
CC := gcc
```

指定编译器。

```makefile
CFLAGS := -Wall -g -O0 -Iinclude
```

指定编译参数。

```makefile
SRC := src/main.c src/calc.c
OBJ := $(SRC:src/%.c=$(BUILD_DIR)/%.o)
```

把源文件列表转换成目标文件列表。

```makefile
$@
```

表示当前规则的目标。

```makefile
$<
```

表示当前规则的第一个依赖。

## 今日验收

你能回答：

1. Makefile 的目标、依赖、命令分别是什么？
2. 为什么命令前必须用 Tab？
3. `make` 为什么知道要先编译 `.o` 再链接？
4. `make clean` 为什么要写成伪目标？
5. 为什么修改 `main.c` 后不会重新编译 `calc.c`？
