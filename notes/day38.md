# Day38 统一响应格式与错误码

## 今日目标

在 Day37 的设备网关基础上，统一 server 的响应格式，让成功和失败响应都具备稳定字段：

```text
OK code=0 msg=...
ERR code=1001 msg=...
```

原来的响应格式比较混杂：

```text
OK device=sensor_node_01 online=1 temp=32 voltage=24 led=off
OK led=on
OK reboot scheduled
ERR unknown command
```

统一后：

```text
OK code=0 msg=status device=sensor_node_01 online=1 temp=32 voltage=24 led=off
OK code=0 msg=led_on
OK code=0 msg=reboot_scheduled
ERR code=1001 msg=unknown_command
```

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day38_device_gateway_response_code
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day38_device_gateway_response_code
```

VS Code 打开：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day38_device_gateway_response_code
code .
```

## 响应码定义

`include/command.h` 新增：

```c
#define RESPONSE_OK 0
#define RESPONSE_ERR_UNKNOWN_COMMAND 1001
```

含义：

```text
0     成功
1001  未知命令
```

错误码是协议的一部分，放在头文件里可以让其他模块或客户端复用。

## status 响应格式

`src/device_state.c` 中的 `device_state_get_status()` 改为：

```c
snprintf(buffer,
         buffer_size,
         "OK code=0 msg=status device=%s online=%d temp=%d voltage=%d led=%s",
         state->name,
         state->online,
         state->temperature,
         state->voltage,
         state->led_on ? "on" : "off");
```

`status` 的具体设备数据仍然由 `device_state.c` 生成，`command.c` 不重新拼设备状态。

## command 响应格式

`led on`：

```c
device_state_set_led(&g_device_state, 1);
snprintf(response, response_size, "OK code=%d msg=led_on", RESPONSE_OK);
```

`led off`：

```c
device_state_set_led(&g_device_state, 0);
snprintf(response, response_size, "OK code=%d msg=led_off", RESPONSE_OK);
```

`reboot`：

```c
snprintf(response, response_size, "OK code=%d msg=reboot_scheduled", RESPONSE_OK);
```

未知命令：

```c
snprintf(response,
         response_size,
         "ERR code=%d msg=unknown_command",
         RESPONSE_ERR_UNKNOWN_COMMAND);
```

## 为什么 msg 不用空格

不推荐：

```text
msg=reboot scheduled
msg=unknown command
```

推荐：

```text
msg=reboot_scheduled
msg=unknown_command
```

因为空格会让客户端解析变复杂。后面 Qt 上位机、脚本或其他客户端可以按空格拆字段：

```text
OK
code=0
msg=led_on
```

## 运行验证

编译：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day38_device_gateway_response_code
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
reboot
abc
quit
```

实际日志：

```text
[2026-08-04 14:17:05] [INFO] response built: OK code=0 msg=status device=sensor_node_01 online=1 temp=32 voltage=24 led=off
[2026-08-04 14:17:05] [INFO] response built: OK code=0 msg=led_on
[2026-08-04 14:17:05] [INFO] response built: OK code=0 msg=status device=sensor_node_01 online=1 temp=33 voltage=25 led=on
[2026-08-04 14:17:05] [INFO] response built: OK code=0 msg=led_off
[2026-08-04 14:17:05] [INFO] response built: OK code=0 msg=reboot_scheduled
[2026-08-04 14:17:05] [INFO] response built: ERR code=1001 msg=unknown_command
```

验证结果：

```text
所有成功响应都有 OK code=0 msg=...
所有失败响应都有 ERR code=... msg=...
msg 字段没有空格
```

## 今日易错点

1. `msg=led=on` 不如 `msg=led_on`，字段里不要再嵌套等号。
2. `msg=reboot scheduled` 有空格，不利于解析。
3. `ERR code=1001 unknown command` 缺少 `msg=` 字段。
4. `status` 响应由 `device_state.c` 生成，不要把设备状态拼接逻辑搬回 `command.c`。
5. 响应码宏放在 `command.h`，不要在多个 `.c` 文件里散落硬编码数字。

## 今日完成状态

- [x] 在 `command.h` 定义成功响应码
- [x] 在 `command.h` 定义未知命令错误码
- [x] 统一 `status` 响应格式
- [x] 统一 `led on` 响应格式
- [x] 统一 `led off` 响应格式
- [x] 统一 `reboot` 响应格式
- [x] 统一未知命令错误响应格式
- [x] 通过日志验证所有响应格式

## 今日总结

Day38 把设备网关的响应从“人能看懂”推进到了“程序容易解析”。

这一步对后续很有价值。无论是 Qt 上位机、命令行脚本，还是开发板上的其他进程，只要拿到响应，都可以稳定解析：

```text
OK / ERR
code
msg
附加状态字段
```

这就是一个小协议开始变得可靠的标志。
