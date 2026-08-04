# Day35 设备网关加入设备状态模块

## 今日目标

在 Day34 的设备网关基础上新增 `device_state` 模块，把 `status` 响应中的设备状态从 `command.c` 的硬编码中拆出来。

原来：

```c
snprintf(response, response_size, "OK device=online temp=32 voltage=24");
```

现在：

```text
command.c
    |
    v
device_state_get_status()
    |
    v
DeviceState 结构体
```

今日重点：

```text
命令解析和设备状态分离
用结构体保存设备状态
让 status 响应来自状态模块
```

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day35_device_gateway_state
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day35_device_gateway_state
```

VS Code 打开：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day35_device_gateway_state
code .
```

## 新增文件

```text
include/device_state.h
src/device_state.c
```

## 设备状态结构体

`include/device_state.h`：

```c
#define DEVICE_NAME_SIZE 64

struct DeviceState {
    char name[DEVICE_NAME_SIZE];
    int online;
    int temperature;
    int voltage;
};
```

字段含义：

```text
name         被管理的设备名
online       在线状态，1 表示在线
temperature  温度
voltage      电压
```

## 设备状态接口

```c
void device_state_init(struct DeviceState *state);

void device_state_get_status(const struct DeviceState *state,
                             char *buffer,
                             int buffer_size);
```

`device_state_init()` 负责初始化状态：

```c
snprintf(state->name, sizeof(state->name), "sensor_node_01");
state->online = 1;
state->temperature = 32;
state->voltage = 24;
```

`device_state_get_status()` 负责把状态格式化成响应：

```c
snprintf(buffer,
         buffer_size,
         "OK device=%s online=%d temp=%d voltage=%d",
         state->name,
         state->online,
         state->temperature,
         state->voltage);
```

注意这里必须使用 `buffer_size`，不能写 `sizeof(buffer)`。

因为 `buffer` 是函数参数，进入函数后本质是指针，`sizeof(buffer)` 得到的是指针大小，不是外部数组大小。

## command.c 接入状态模块

新增头文件：

```c
#include "device_state.h"
```

新增模块内部状态：

```c
static struct DeviceState g_device_state;
static int g_device_state_inited = 0;
```

`static` 表示这两个变量只在 `command.c` 内部可见。

在 `build_response()` 开始处初始化：

```c
if (!g_device_state_inited) {
    device_state_init(&g_device_state);
    g_device_state_inited = 1;
}
```

这样只会在第一次处理命令时初始化一次设备状态。

`status` 分支改成：

```c
if (strcmp(request, "status\n") == 0) {
    device_state_get_status(&g_device_state, response, response_size);
}
```

## Makefile 链接关系

`device_state.c` 被 `command.c` 调用，而 `command.c` 属于 server，所以 `device_state.c` 要链接到 server：

```makefile
src1 := src/server.c src/protocol.c src/command.c src/logger.c src/config.c src/device_state.c
```

client 不直接使用设备状态模块：

```makefile
src2 := src/client.c src/protocol.c src/config.c
```

如果把 `src/device_state.c` 只加到 client，server 仍然会链接失败。

## 运行验证

编译：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day35_device_gateway_state
make clean
make
```

终端 1：

```bash
make run1
```

终端 2：

```bash
make run2
```

客户端输入：

```text
status
quit
```

期望响应：

```text
[response] OK device=sensor_node_01 online=1 temp=32 voltage=24
```

日志验证：

```text
[2026-08-04 09:39:42] [INFO] response built: OK device=sensor_node_01 online=1 temp=32 voltage=24
```

## 今日易错点

1. `DEVICE_STATE_H` 拼写要一致，不能写成 `DEVIEC_STATE_H`。
2. `struct DeviceState` 拼写要一致，不能写成 `struct Deveice`。
3. 初始化字段时使用 `=`，不能写成 `==`。
4. `snprintf()` 第二个参数要用 `buffer_size`，不能用 `sizeof(buffer)`。
5. `g_device_state` 应该加 `static`，限制在 `command.c` 内部。
6. `device_state.c` 要链接到 server 的编译列表中。

## 今日完成状态

- [x] 新增 `include/device_state.h`
- [x] 新增 `src/device_state.c`
- [x] 定义 `struct DeviceState`
- [x] 实现 `device_state_init()`
- [x] 实现 `device_state_get_status()`
- [x] 修改 `command.c` 使用状态模块
- [x] 修改 Makefile 链接 `src/device_state.c`
- [x] 验证 `status` 响应来自设备状态模块

## 今日总结

Day35 把设备网关从“固定字符串响应”推进到了“内部状态响应”。

这一步很重要，因为真实嵌入式设备不是只回复写死文本，而是根据当前设备状态、传感器数据、控制状态来生成响应。

现在项目里已经出现了更清晰的职责划分：

```text
protocol.c       负责协议收发
command.c        负责命令分发
device_state.c   负责设备状态
logger.c         负责日志
config.c         负责配置
server.c         负责网络事件
```

这就是一个小型嵌入式 Linux 服务开始成型的样子。
