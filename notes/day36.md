# Day36 设备网关加入 LED 状态管理

## 今日目标

在 Day35 的设备状态模块基础上，让 `led on` / `led off` 命令真正修改 `DeviceState`，并让 `status` 能看到 LED 状态变化。

原来：

```text
led on  只返回 OK led=on
led off 只返回 OK led=off
status  不知道 LED 当前状态
```

现在：

```text
led on
    |
    v
g_device_state.led_on = 1
    |
    v
status 显示 led=on
```

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day36_device_gateway_led_state
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day36_device_gateway_led_state
```

VS Code 打开：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day36_device_gateway_led_state
code .
```

## DeviceState 新增字段

`include/device_state.h`：

```c
struct DeviceState {
    char name[DEVICE_NAME_SIZE];
    int online;
    int temperature;
    int voltage;
    int led_on;
};
```

`led_on` 含义：

```text
0  LED 关闭
1  LED 打开
```

## 新增接口

```c
void device_state_set_led(struct DeviceState *state, int led_on);

int device_state_get_led(const struct DeviceState *state);
```

实现：

```c
void device_state_set_led(struct DeviceState *state, int led_on)
{
    state->led_on = led_on ? 1 : 0;
}

int device_state_get_led(const struct DeviceState *state)
{
    return state->led_on;
}
```

为什么写成：

```c
state->led_on = led_on ? 1 : 0;
```

因为希望内部状态统一为：

```text
0
1
```

即使外部传入 `2`、`100`，也统一转换成 `1`。

## 初始化 LED 状态

`device_state_init()` 中新增：

```c
state->led_on = 0;
```

表示程序启动后，默认 LED 是关闭状态。

## status 显示 LED 状态

`device_state_get_status()` 改成：

```c
snprintf(buffer,
         buffer_size,
         "OK device=%s online=%d temp=%d voltage=%d led=%s",
         state->name,
         state->online,
         state->temperature,
         state->voltage,
         state->led_on ? "on" : "off");
```

这里：

```c
state->led_on ? "on" : "off"
```

含义：

```text
led_on 非 0  显示 on
led_on 为 0  显示 off
```

## command.c 修改

`led on` 分支：

```c
else if (strcmp(request, "led on\n") == 0) {
    device_state_set_led(&g_device_state, 1);
    snprintf(response, response_size, "OK led=on");
}
```

`led off` 分支：

```c
else if (strcmp(request, "led off\n") == 0) {
    device_state_set_led(&g_device_state, 0);
    snprintf(response, response_size, "OK led=off");
}
```

这一步之后，命令不再只是返回字符串，而是真的修改了服务端内部状态。

## 运行验证

编译：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day36_device_gateway_led_state
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
led on
status
led off
status
quit
```

期望响应：

```text
[response] OK device=sensor_node_01 online=1 temp=32 voltage=24 led=off
[response] OK led=on
[response] OK device=sensor_node_01 online=1 temp=32 voltage=24 led=on
[response] OK led=off
[response] OK device=sensor_node_01 online=1 temp=32 voltage=24 led=off
```

日志验证：

```text
[2026-08-04 10:12:56] [INFO] response built: OK device=sensor_node_01 online=1 temp=32 voltage=24 led=off
[2026-08-04 10:13:01] [INFO] response built: OK led=on
[2026-08-04 10:13:05] [INFO] response built: OK device=sensor_node_01 online=1 temp=32 voltage=24 led=on
[2026-08-04 10:13:11] [INFO] response built: OK led=off
[2026-08-04 10:13:16] [INFO] response built: OK device=sensor_node_01 online=1 temp=32 voltage=24 led=off
```

## 今日易错点

1. 只写 `int led_on;` 是局部变量，不能保存到结构体。
2. 应该写 `state->led_on = 0;`。
3. 头文件声明和 `.c` 文件实现的参数 `const` 要一致。
4. `led on` / `led off` 分支必须调用 `device_state_set_led()`。
5. 状态改变后，要通过 `status` 再次验证。

## 今日完成状态

- [x] `DeviceState` 新增 `led_on`
- [x] 初始化 LED 默认关闭
- [x] 新增 `device_state_set_led()`
- [x] 新增 `device_state_get_led()`
- [x] `status` 响应显示 `led=on/off`
- [x] `led on` 真正修改状态
- [x] `led off` 真正修改状态
- [x] 验证 `status -> led on -> status -> led off -> status`

## 今日总结

Day36 把设备网关从“能查询状态”推进到了“命令能改变状态”。

这一步是嵌入式控制类程序的关键：控制命令不能只是返回成功字符串，而应该改变设备内部状态，后续查询也必须能看到变化。

现在这个网关已经具备一个真实控制服务的基本闭环：

```text
client 发送控制命令
server 修改设备状态
client 查询状态
server 返回变化后的状态
```

等上开发板后，`led_on` 这个软件状态就可以替换为真实 GPIO 控制。
