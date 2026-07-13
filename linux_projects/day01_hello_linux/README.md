# Day 1 - Hello Linux

今日目标：打通 Linux C 程序的基本开发链路。

## 练习内容

1. 编写 `hello_linux.c`
2. 使用 `gcc` 编译
3. 运行可执行文件
4. 使用 `file` 查看程序类型
5. 故意制造一个编译错误，观察报错信息

## 推荐命令

```bash
cd ~/your-workspace/linux_projects/day01_hello_linux
gcc -Wall -g hello_linux.c -o hello_linux
./hello_linux
file hello_linux
ls -l hello_linux
```

如果你是在 WSL 中访问 Windows 目录，路径可能类似：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day01_hello_linux
```

## 今日必须理解

- `gcc`：调用 C 编译器。
- `-Wall`：打开常见警告。
- `-g`：生成调试信息，后面给 gdb 使用。
- `hello_linux.c`：源文件。
- `-o hello_linux`：指定输出的可执行文件名。
- `./hello_linux`：运行当前目录下的可执行文件。
- `file hello_linux`：查看文件类型和目标架构。

## 今日验收

你能回答：

1. 源文件和可执行文件有什么区别？
2. `gcc -Wall -g hello_linux.c -o hello_linux` 每一部分是什么意思？
3. 为什么运行当前目录程序要写 `./hello_linux`？
4. `file hello_linux` 显示了什么架构信息？
