# Day46 按键状态接入设备网关

## 今日目标

Day46 的目标是把 Day45 的按键输入能力接入 Day44 的设备网关。

最终效果：

```text
按下开发板用户按键
  -> key_input 线程读取 /dev/input/event1
  -> 更新 DeviceState.key_pressed
  -> client 输入 status
  -> server 返回 key=pressed

松开按键
  -> key_input 线程更新 DeviceState.key_pressed
  -> status 返回 key=released
```

这一天的核心不是“多写一个线程”，而是理解：

```text
网络请求是被动查询
硬件按键是主动事件
两者通过共享设备状态连接
```

## 项目路径

本地路径：

```text
C:\Users\20858\Documents\实习\linux_projects\day46_gateway_key_status
```

开发板路径：

```text
/home/debian/apps/day46_gateway_key_status
```

## 为什么不能直接在 status 里 read 按键

LED 状态可以这样读：

```text
/sys/class/leds/red/brightness
```

因为它像一个“当前状态文件”。

但按键事件不同：

```text
/dev/input/event1
```

它是事件流，不是普通状态文件。

如果没有按键事件：

```c
read(fd, &event, sizeof(event));
```

会阻塞等待。

如果把这个阻塞 read 放进 `status` 处理里，结果就是：

```text
client 输入 status
server 卡住等待按键
不按键就不返回
```

所以 Day46 的设计是：

```text
后台线程持续监听按键
按键状态缓存到 DeviceState
status 只读取缓存状态
```

## 为什么要统一 DeviceState

Day44 时，`command.c` 内部有自己的：

```c
static struct DeviceState g_device_state;
```

这会带来问题：

```text
command.c 能访问这份状态
server.c 拿不到
key_input.c 也不能自然更新同一份状态
```

Day46 改成：

```text
device_state.c 持有唯一的全局 DeviceState
command.c 通过 device_state_global() 获取
server.c 也通过 device_state_global() 获取
key_input.c 拿到同一份状态指针并更新
```

核心接口：

```c
struct DeviceState *device_state_global(void);
```

这样整个程序共享的是同一份设备状态。

## DeviceState 扩展

结构体新增：

```c
int key_pressed;
```

完整状态包含：

```text
name
online
temperature
voltage
led_on
key_pressed
```

status 响应新增：

```text
key=pressed
key=released
```

## key_input 模块

新增文件：

```text
include/key_input.h
src/key_input.c
```

对外接口：

```c
int key_input_start(struct DeviceState *state);
```

含义：

```text
启动按键监听线程
把读到的按键状态写入传入的 DeviceState
```

## 为什么使用 pthread

server 主线程需要等待网络事件：

```c
epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
```

按键模块需要等待按键事件：

```c
read(fd, &event, sizeof(event));
```

这两个等待都会阻塞。

如果只用一个线程直接 read 按键：

```text
server 会卡在按键 read
无法继续处理 TCP client
```

所以 Day46 使用两个执行流：

```text
主线程：
  处理 TCP server、epoll、client 请求

按键线程：
  阻塞读取 /dev/input/event1
  发现 KEY_0 pressed/released
  更新 DeviceState.key_pressed
```

也可以不用 pthread，未来可以把 `/dev/input/event1` 加入 epoll，做单线程统一事件循环。  
但 Day46 先用 pthread，更容易看清：

```text
异步硬件事件如何进入网关状态
```

## key_input 线程逻辑

打开按键设备：

```c
open("/dev/input/event1", O_RDONLY);
```

循环读取：

```c
read(fd, &event, sizeof(event));
```

只关心：

```c
event.type == EV_KEY && event.code == KEY_0
```

按下：

```c
event.value == 1
device_state_set_key(state, 1);
```

松开：

```c
event.value == 0
device_state_set_key(state, 0);
```

## server 接入

server 启动时调用：

```c
key_input_start(device_state_global());
```

这句话的含义是：

```text
拿到全局 DeviceState
启动按键线程
按键线程和 command.c 使用同一份状态
```

## Makefile 修改

server 新增源文件：

```makefile
src/key_input.c
```

因为使用 pthread，编译参数加入：

```makefile
-pthread
```

原因：

```text
pthread_create()
pthread_detach()
```

需要 POSIX 线程支持。

## 编译和部署

编译：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day46_gateway_key_status

make clean
make
file build/server build/client
```

部署：

```bash
ssh debian@192.168.7.2 "mkdir -p /home/debian/apps/day46_gateway_key_status/config /home/debian/apps/day46_gateway_key_status/logs"

scp build/server build/client debian@192.168.7.2:/home/debian/apps/day46_gateway_key_status/

scp config/gateway.conf debian@192.168.7.2:/home/debian/apps/day46_gateway_key_status/config/
```

## 测试结果

client 第一次查询：

```text
[response] OK code=0 msg=status device=sensor_node_01 online=1 temp=32 voltage=24 led=off key=released
```

按住按键后查询：

```text
[response] OK code=0 msg=status device=sensor_node_01 online=1 temp=33 voltage=25 led=off key=pressed
```

松开按键后查询：

```text
[response] OK code=0 msg=status device=sensor_node_01 online=1 temp=34 voltage=24 led=off key=released
```

说明：

```text
按键线程成功读取 input event
DeviceState.key_pressed 更新成功
status 返回真实按键状态
```

## 日志验证

server.log 中记录：

```text
server starting
server config loaded
client connected
request received: status
response built: ... key=released
request received: status
response built: ... key=pressed
request received: status
response built: ... key=released
server stopped
```

日志证明 server 的 status 响应确实记录了按键状态变化。

## 今日踩坑

### 全局变量不能用函数调用初始化

错误写法：

```c
struct DeviceState *state = device_state_global();
```

如果放在函数外，就是全局变量初始化。

C 语言中，全局变量初始化需要是编译期常量，而：

```c
device_state_global()
```

是运行时函数调用，所以不能放在函数外初始化。

正确写法：

```c
void build_response(...)
{
    struct DeviceState *state = device_state_global();
}
```

### if 判断优先级

错误写法：

```c
if (!key_input_start(device_state_global()) < 0)
```

正确写法：

```c
if (key_input_start(device_state_global()) < 0)
```

含义：

```text
如果 key_input_start 返回负数，说明启动失败
```

## 今日结论

Day46 完成了设备网关的硬件输入接入：

```text
物理按键
  -> /dev/input/event1
  -> key_input 线程
  -> DeviceState.key_pressed
  -> command.c status
  -> TCP client 响应
```

现在设备网关已经同时具备：

```text
网络通信
配置文件
日志
LED 硬件输出
KEY 硬件输入
```

这已经是一个比较完整的嵌入式 Linux 应用网关雏形。

