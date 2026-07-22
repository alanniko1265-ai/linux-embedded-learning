# Day20 线程同步和生产者消费者队列

## 今日目标

学习 Linux 多线程程序中常见的生产者消费者模型，完成一个线程安全设备数据队列。

今天重点：

- 环形队列
- `pthread_mutex_t`
- `pthread_cond_t`
- `pthread_cond_wait`
- `pthread_cond_signal`
- 生产者消费者模型
- 队列空和队列满时的等待逻辑

## 工程路径

代码路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day20_thread_queue
```

WSL 路径：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day20_thread_queue
```

VS Code 打开命令：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day20_thread_queue
code .
```

## 编译运行

使用 Makefile：

```bash
make clean
make
make run
```

手动编译：

```bash
mkdir -p build
gcc -Wall -g -O0 -pthread src/main.c -o build/thread_queue_demo
./build/thread_queue_demo
```

## 核心结构体

设备数据：

```c
struct DeviceData {
    int id;
    int value;
};
```

线程安全队列：

```c
struct DataQueue {
    struct DeviceData items[QUEUE_CAPACITY];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};
```

字段含义：

- `items`：保存设备数据的数组
- `head`：下一次从哪里取
- `tail`：下一次往哪里放
- `count`：当前队列里有多少条数据
- `mutex`：保护队列共享状态
- `not_empty`：队列非空条件
- `not_full`：队列未满条件

## 环形队列

队列容量是固定的：

```c
#define QUEUE_CAPACITY 8
```

数组下标只能是：

```text
0 1 2 3 4 5 6 7
```

所以 `head` 和 `tail` 移动时要取余：

```c
queue->tail = (queue->tail + 1) % QUEUE_CAPACITY;
queue->head = (queue->head + 1) % QUEUE_CAPACITY;
```

作用是让下标循环：

```text
0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 0
```

`count` 负责判断队列满不满、空不空；`head/tail` 负责记录具体读写位置。

## queue_push

生产者调用 `queue_push` 往队列里放数据。

如果队列满了，生产者不能继续写，否则会覆盖还没有被消费者取走的数据。

完整逻辑：

```text
加锁
如果队列满，等待 not_full
写入数据
更新 tail 和 count
通知 not_empty
解锁
```

关键代码：

```c
while (queue->count == QUEUE_CAPACITY) {
    pthread_cond_wait(&queue->not_full, &queue->mutex);
}
```

这里表示：队列满时，生产者睡眠等待，直到消费者取走数据并通知 `not_full`。

## queue_pop

消费者调用 `queue_pop` 从队列里取数据。

如果队列为空，消费者不能继续取，否则没有数据可取。

完整逻辑：

```text
加锁
如果队列空，等待 not_empty
取出数据
更新 head 和 count
通知 not_full
解锁
```

关键代码：

```c
while (queue->count == 0) {
    pthread_cond_wait(&queue->not_empty, &queue->mutex);
}
```

这里表示：队列为空时，消费者睡眠等待，直到生产者放入数据并通知 `not_empty`。

## pthread_cond_wait 的理解

```c
pthread_cond_wait(&queue->not_empty, &queue->mutex);
```

它不是单纯睡眠，而是做三件事：

```text
1. 释放 mutex
2. 当前线程进入等待
3. 被唤醒后重新获得 mutex
```

如果等待时不释放锁，生产者就无法进入 `queue_push` 放数据，程序会死锁。

## 为什么用 while 不用 if

条件变量被唤醒后，不代表条件一定满足。

所以不能写：

```c
if (queue->count == 0) {
    pthread_cond_wait(&queue->not_empty, &queue->mutex);
}
```

应该写：

```c
while (queue->count == 0) {
    pthread_cond_wait(&queue->not_empty, &queue->mutex);
}
```

醒来后重新检查条件，这是条件变量的标准写法。

## 程序不退出的问题

今天遇到过程序不停止的问题。

原因是最初版本中：

```text
队列满时 producer 直接丢数据
consumer 仍然固定等待消费 10 条数据
```

如果 producer 丢了一条，consumer 就永远等不到第 10 条，于是卡在：

```c
pthread_cond_wait(&queue->not_empty, &queue->mutex);
```

主线程又在等待 consumer：

```c
pthread_join(consumer, NULL);
```

所以主程序不会退出。

修正方式：

```text
队列空：consumer 等 not_empty
队列满：producer 等 not_full
producer 放入数据：通知 not_empty
consumer 取走数据：通知 not_full
```

## 和嵌入式应用的关系

这个模型非常常见：

```text
串口接收线程 -> 数据队列 -> 协议处理线程
传感器采集线程 -> 数据队列 -> 网络上报线程
日志写入请求 -> 日志队列 -> 日志线程
TCP 接收线程 -> 命令队列 -> 设备控制线程
```

生产者消费者队列可以把不同线程解耦：

- 生产者只负责产生数据
- 消费者只负责处理数据
- 队列负责缓冲和同步

## 今日完成状态

- [x] 定义设备数据结构
- [x] 实现环形队列
- [x] 实现 `queue_push`
- [x] 实现 `queue_pop`
- [x] 使用 mutex 保护共享队列
- [x] 使用 `not_empty` 让消费者等待数据
- [x] 使用 `not_full` 让生产者等待空位
- [x] 实现 producer 线程
- [x] 实现 consumer 线程
- [x] 理解程序不退出的原因和修法
