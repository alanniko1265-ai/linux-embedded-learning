# Day19 pthread 线程基础

## 今日目标

学习 Linux 应用开发中的线程基础，理解：

- `pthread_create` 如何创建线程
- `pthread_join` 如何等待线程结束
- 如何给线程函数传递参数
- 线程和 `fork` 创建进程的区别
- 多线程共享全局变量时为什么会出现竞态条件
- 如何使用 `pthread_mutex_t` 保护共享数据

## 工程路径

代码路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day19_pthread_basic
```

WSL 路径：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day19_pthread_basic
```

VS Code 打开命令：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day19_pthread_basic
code .
```

## 编译运行

使用 Makefile 编译：

```bash
make clean
make
make run
```

也可以直接使用 gcc 编译：

```bash
gcc -Wall -g -O0 -pthread src/main.c -o build/thread_status_demo
./build/thread_status_demo
```

注意：只要程序使用了 `pthread_create`、`pthread_join`、`pthread_mutex_lock` 等 pthread 相关函数，工程里就应该加 `-pthread`。

## 核心代码结构

线程函数必须符合 pthread 要求的函数签名：

```c
static void *sensor_thread(void *arg)
```

含义：

- 返回值是 `void *`
- 参数是 `void *`
- 线程执行结束时可以 `return NULL`

线程参数通常用结构体打包：

```c
struct ThreadArg {
    const char *name;
    int count;
};
```

创建线程时把结构体地址传进去：

```c
pthread_create(&sensor, NULL, sensor_thread, &sensor_arg);
```

在线程函数内部再把 `void *` 转回原来的结构体指针：

```c
struct ThreadArg *thread_arg = (struct ThreadArg *)arg;
```

## pthread_create 参数理解

```c
pthread_create(&sensor, NULL, sensor_thread, &sensor_arg);
```

四个参数分别是：

- `&sensor`：保存新线程 ID
- `NULL`：使用默认线程属性
- `sensor_thread`：线程启动后执行的函数
- `&sensor_arg`：传给线程函数的参数

## pthread_join 的作用

```c
pthread_join(sensor, NULL);
```

作用是让主线程等待 `sensor` 线程执行结束。

如果没有 `pthread_join`，主线程可能提前退出，整个进程结束，子线程还没有来得及执行完。

## 线程和 fork 的区别

`fork` 创建的是新进程，`pthread_create` 创建的是同一进程里的新线程。

区别：

- `fork` 后父子进程有各自独立的地址空间
- `pthread_create` 创建的线程共享同一个进程的地址空间
- 线程可以直接共享全局变量
- 进程之间要通信通常需要 pipe、socket、共享内存等机制

所以线程适合做同一个程序内部的并发任务，例如：

- 一个线程采集设备数据
- 一个线程处理协议
- 一个线程写日志
- 一个线程进行网络上报

## 竞态条件

今天的程序中两个线程同时执行：

```c
g_count++;
```

这行代码看起来是一句，但底层不是一个不可分割的动作，大致包含：

```text
1. 读取 g_count
2. 加 1
3. 写回 g_count
```

如果两个线程同时执行，就可能出现互相覆盖结果的情况。

两个线程各自累加 1000000 次，理论结果应该是：

```text
final count 2000000
```

如果不加锁，结果可能小于 2000000，这就是竞态条件。

## 互斥锁

定义互斥锁：

```c
static pthread_mutex_t g_count_mutex = PTHREAD_MUTEX_INITIALIZER;
```

保护共享变量：

```c
pthread_mutex_lock(&g_count_mutex);
g_count++;
pthread_mutex_unlock(&g_count_mutex);
```

含义：

- `pthread_mutex_lock`：进入临界区前加锁
- `g_count++`：访问共享变量
- `pthread_mutex_unlock`：离开临界区后解锁

加锁后，同一时间只有一个线程能修改 `g_count`，最终结果应稳定为：

```text
final count 2000000
```

## 今日总结

今天的重点不是把线程 API 背下来，而是建立 Linux 多线程程序的基本模型：

```text
主线程创建工作线程
工作线程执行各自任务
多个线程共享进程内存
共享数据需要互斥锁保护
主线程通过 pthread_join 等待线程结束
```

这个模型后面会用于嵌入式 Linux 应用：

- 采集线程
- 通信线程
- 日志线程
- 状态监控线程
- 网络上报线程

## 今日完成状态

- [x] 创建 pthread 线程
- [x] 使用结构体给线程传参
- [x] 使用 `pthread_join` 等待线程结束
- [x] 理解线程和 `fork` 的区别
- [x] 观察共享变量的竞态问题
- [x] 使用互斥锁保护共享变量
- [x] 编写 Makefile 并加入 `-pthread`
