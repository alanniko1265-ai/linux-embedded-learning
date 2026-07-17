# Day 12 - 进程基础：proc_runner

日期：2026-07-17

## 今日目标

- [x] 创建 `proc_runner` 项目。
- [x] 使用 `argc/argv` 接收要执行的程序和参数。
- [x] 使用 `fork` 创建子进程。
- [x] 理解父进程和子进程的分支。
- [x] 使用 `execvp` 在子进程中执行新程序。
- [x] 使用 `waitpid` 让父进程等待子进程结束。
- [x] 使用 `WIFEXITED` 判断子进程是否正常退出。
- [x] 使用 `WEXITSTATUS` 获取子进程退出码。
- [x] 编写 Makefile 测试成功命令、错误命令、错误参数。

## 项目目录

```text
linux_projects/day12_process_runner
```

## VS Code 打开命令

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day12_process_runner
code .
```

## 今日程序

程序名：

```text
proc_runner
```

运行方式：

```bash
./build/proc_runner <program> [args...]
```

示例：

```bash
./build/proc_runner /bin/ls data
./build/proc_runner /bin/echo hello linux
./build/proc_runner /bin/not_exist
./build/proc_runner /bin/ls not_exist_dir
```

## 核心流程

```text
proc_runner 启动
检查参数
fork 创建子进程
子进程 execvp 执行新程序
父进程 waitpid 等待子进程结束
父进程读取子进程退出码
```

## fork

调用：

```c
pid = fork();
```

返回值含义：

```text
pid < 0   fork 失败
pid == 0  当前处于子进程
pid > 0   当前处于父进程，pid 是子进程的进程号
```

所以判断逻辑是：

```c
if (pid < 0) {
    perror("fork");
    return 1;
}

if (pid == 0) {
    // child
} else {
    // parent
}
```

注意：

```text
pid < 0 表示 fork 失败，不能在这里 execvp。
execvp 应该放在 pid == 0 的子进程分支里。
```

## execvp

调用：

```c
execvp(argv[1], &argv[1]);
```

函数意义：

```text
第 1 个参数 argv[1]：要执行哪个程序。
第 2 个参数 &argv[1]：传给新程序的参数列表。
```

例如运行：

```bash
./build/proc_runner /bin/ls data
```

原程序参数：

```text
argv[0] = ./build/proc_runner
argv[1] = /bin/ls
argv[2] = data
argv[3] = NULL
```

传给 `execvp` 后，新程序收到：

```text
argv[0] = /bin/ls
argv[1] = data
argv[2] = NULL
```

等价于：

```bash
/bin/ls data
```

重点：

```text
execvp 不创建新进程。
execvp 会把当前进程替换成另一个程序。
所以要先 fork，再在子进程里 execvp。
```

如果 `execvp` 成功，它不会返回。

所以：

```c
execvp(argv[1], &argv[1]);
perror("execvp");
return 1;
```

含义是：

```text
只有 execvp 失败时，才会执行 perror 和 return 1。
```

## waitpid

调用：

```c
waitpid(pid, &status, 0);
```

作用：

```text
父进程等待指定 pid 的子进程结束。
子进程结束状态会写入 status。
```

常见失败情况：

```text
等待的 pid 不是自己的子进程
同一个子进程已经被 waitpid 回收过
等待过程中被信号打断
参数非法
```

当前程序中，`pid` 来自刚刚成功的 `fork`，所以正常情况下 `waitpid` 会成功。

## 退出状态

判断是否正常退出：

```c
if (WIFEXITED(status)) {
    printf("child exit code=%d\n", WEXITSTATUS(status));
} else {
    printf("child did not exit normally\n");
}
```

含义：

```text
WIFEXITED(status)     判断子进程是否正常 exit/return 退出。
WEXITSTATUS(status)  获取子进程退出码。
```

退出码常见含义：

```text
0      成功
非 0   失败或异常情况，由子程序自己定义
```

## 两类失败

### execvp 失败

示例：

```bash
./build/proc_runner /bin/not_exist
```

含义：

```text
要执行的程序本身不存在。
子进程连新程序都没有成功启动。
```

会触发：

```c
perror("execvp");
return 1;
```

### 子程序启动成功，但执行结果失败

示例：

```bash
./build/proc_runner /bin/ls not_exist_dir
```

含义：

```text
/bin/ls 成功启动了。
但是 ls 发现 not_exist_dir 不存在。
所以 ls 自己返回非 0 退出码。
```

这不是 `execvp` 失败，而是子程序自己的业务失败。

## 为什么 return 后面好像还在执行

因为 `fork` 后有父进程和子进程。

比如子进程：

```c
execvp(argv[1], &argv[1]);
perror("execvp");
return 1;
```

如果子进程 `return 1`，结束的是子进程。

父进程还会继续执行：

```c
waitpid(pid, &status, 0);
printf("child finished\n");
```

所以不是 `return` 后面的代码继续执行，而是另一个进程还在运行。

## Makefile

常用命令：

```bash
make clean
make
make run-ls
make run-echo
make run-fail-cmd
make run-fail-arg
```

测试含义：

```text
run-ls        /bin/ls data，正常退出，退出码 0
run-echo      /bin/echo hello linux，正常退出，退出码 0
run-fail-cmd  /bin/not_exist，execvp 失败
run-fail-arg  /bin/ls not_exist_dir，ls 启动成功但参数不存在
```

注意：

```text
not_exist_dir 中间不能有空格。
如果写成 not _exist_dir，ls 会认为是两个参数。
```

## 今日遇到的问题

1. usage 字符串中 `>` 位置写错。
2. `execvp` 一开始误放进了 `pid < 0` 分支。
3. 不理解 `execvp(argv[1], &argv[1])` 两个参数的意义。
4. 不理解为什么 `perror` 后 `return`，后面还会继续输出。
5. 不清楚 `waitpid` 什么时候会报错。
6. Makefile 中 `not_exist_dir` 如果中间有空格，会变成两个参数。

## 今日总结

今天我学会了：

1. `fork` 创建子进程后，父子进程会走不同分支。
2. `execvp` 会把当前进程替换成新程序。
3. `fork + execvp` 是 Linux 中启动外部程序的经典组合。
4. `waitpid` 可以让父进程等待子进程结束。
5. `WIFEXITED` 和 `WEXITSTATUS` 可以读取子进程退出码。
6. 要区分 `execvp` 失败和子程序执行失败。

今天还需要加强：

1. 父子进程并发执行时的输出顺序。
2. 退出码和错误处理策略。
3. 后续学习信号时，理解子进程被信号终止的情况。
