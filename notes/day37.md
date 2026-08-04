# Day37 设备状态动态采样

## 今日目标

在 Day36 的设备网关基础上，让 `temperature` 和 `voltage` 不再永远固定，而是在每次查询 `status` 时更新一次模拟采样数据。

这一步不是无关 demo，而是为后续上开发板做接口预留：

```text
现在：device_state_update_sample() 模拟生成温度和电压
以后：device_state_update_sample() 读取 ADC / I2C 传感器 / sysfs / 驱动节点
```

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day37_device_gateway_dynamic_state
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day37_device_gateway_dynamic_state
```

VS Code 打开：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day37_device_gateway_dynamic_state
code .
```

## 新增接口

`include/device_state.h`：

```c
void device_state_update_sample(struct DeviceState *state);
```

职责：

```text
更新设备采样数据
```

当前更新：

```text
temperature
voltage
```

不修改：

```text
led_on
online
name
```

## 模拟采样实现

`src/device_state.c`：

```c
void device_state_update_sample(struct DeviceState *state)
{
    static int sample_count = 0;

    state->temperature = 32 + (sample_count % 3);
    state->voltage = 24 + (sample_count % 2);

    sample_count++;
}
```

变化规律：

```text
sample_count = 0  temp=32 voltage=24
sample_count = 1  temp=33 voltage=25
sample_count = 2  temp=34 voltage=24
sample_count = 3  temp=32 voltage=25
sample_count = 4  temp=33 voltage=24
```

## static 局部变量

```c
static int sample_count = 0;
```

普通局部变量：

```text
函数调用结束后消失
下一次调用重新初始化
```

`static` 局部变量：

```text
函数调用结束后仍然保留值
下一次调用继续使用上一次的值
```

所以它适合用来记录：

```text
这是第几次采样
```

## command.c 接入

`status` 分支原来：

```c
if (strcmp(request, "status\n") == 0) {
    device_state_get_status(&g_device_state, response, response_size);
}
```

改成：

```c
if (strcmp(request, "status\n") == 0) {
    device_state_update_sample(&g_device_state);
    device_state_get_status(&g_device_state, response, response_size);
}
```

流程：

```text
收到 status
    |
    v
更新一次模拟采样
    |
    v
根据最新 DeviceState 生成响应
```

## 运行验证

编译：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day37_device_gateway_dynamic_state
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
status
status
status
led on
status
quit
```

实际日志：

```text
[2026-08-04 13:42:30] [INFO] response built: OK device=sensor_node_01 online=1 temp=32 voltage=24 led=off
[2026-08-04 13:42:30] [INFO] response built: OK device=sensor_node_01 online=1 temp=33 voltage=25 led=off
[2026-08-04 13:42:30] [INFO] response built: OK device=sensor_node_01 online=1 temp=34 voltage=24 led=off
[2026-08-04 13:42:30] [INFO] response built: OK device=sensor_node_01 online=1 temp=32 voltage=25 led=off
[2026-08-04 13:42:30] [INFO] response built: OK led=on
[2026-08-04 13:42:30] [INFO] response built: OK device=sensor_node_01 online=1 temp=33 voltage=24 led=on
```

验证结果：

```text
temperature / voltage 会随 status 变化
led_on 不会被采样函数误改
```

## 今日易错点

1. 新函数要同时在 `.h` 声明、在 `.c` 实现。
2. `status` 分支要先 update，再 get status。
3. 采样函数只应该更新采样数据，不应该重置 LED 状态。
4. `static int sample_count` 会保留上一次调用后的值。
5. 动态采样后，要验证 LED 状态不会丢失。

## 今日完成状态

- [x] 新增 `device_state_update_sample()`
- [x] 使用 `static sample_count` 模拟采样次数
- [x] 每次 `status` 前更新温度和电压
- [x] 验证连续 `status` 数据会变化
- [x] 验证 `led on` 后 LED 状态保持为 `on`

## 今日总结

Day37 让设备网关的状态从“静态状态”变成“动态状态”。

这一步非常贴近后面的嵌入式迁移：现在的模拟采样函数，将来可以替换成真实硬件读取函数，而 `command.c` 和网络服务层不需要大改。

这就是模块边界的价值：

```text
上层继续调用 device_state_update_sample()
底层从模拟数据换成真实硬件数据
```
