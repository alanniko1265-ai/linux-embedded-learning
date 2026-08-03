# Day33 设备网关加入配置文件

## 今日目标

在 Day32 的设备网关基础上加入配置文件读取能力，让端口、设备名、日志路径不再写死在代码里。

配置文件：

```text
config/gateway.conf
```

配置内容：

```text
server_port=9100
device_name=demo_gateway
log_path=logs/server.log
```

今日重点：

```text
配置文件 -> config.c 解析 -> struct GatewayConfig -> server/client 使用
```

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day33_device_gateway_config
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day33_device_gateway_config
```

VS Code 打开：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day33_device_gateway_config
code .
```

## 新增文件

```text
include/config.h
src/config.c
config/gateway.conf
```

## 配置结构体

`include/config.h`：

```c
#define CONFIG_TEXT_SIZE 128

struct GatewayConfig {
    int server_port;
    char device_name[CONFIG_TEXT_SIZE];
    char log_path[CONFIG_TEXT_SIZE];
};
```

配置文件中的文本：

```text
server_port=9100
device_name=demo_gateway
log_path=logs/server.log
```

会被读入：

```c
config.server_port
config.device_name
config.log_path
```

## 默认配置

`config_set_default()` 的作用是先提供兜底值：

```c
void config_set_default(struct GatewayConfig *config)
{
    config->server_port = 9000;
    snprintf(config->device_name, sizeof(config->device_name), "demo_gateway");
    snprintf(config->log_path, sizeof(config->log_path), "logs/server.log");
}
```

为什么先设置默认值：

```text
配置文件缺少字段时，程序仍然有基础可用配置
读到配置文件字段后，再覆盖默认值
```

## 配置读取流程

`load_config()` 的核心流程：

```text
设置默认配置
打开 config/gateway.conf
逐行读取
用 sscanf 拆出 key 和 value
根据 key 写入 GatewayConfig
关闭文件
```

关键解析语句：

```c
sscanf(line, "%127[^=]=%127s", key, value)
```

含义：

```text
%127[^=]  读取等号前面的内容，最多 127 个字符
=         匹配配置文件里的等号
%127s     读取等号后面的值，最多 127 个非空白字符
```

例如：

```text
server_port=9100
```

解析后：

```text
key   = server_port
value = 9100
```

## server 接入配置

服务端启动时先读取配置：

```c
struct GatewayConfig config;

if (load_config("config/gateway.conf", &config) < 0) {
    return 1;
}
```

日志路径来自配置：

```c
logger_init(config.log_path);
```

监听端口来自配置：

```c
int server_fd = create_server_socket(config.server_port);
```

启动输出来自配置：

```c
printf("%s server listening on port %d\n", config.device_name, config.server_port);
```

## client 接入配置

客户端也读取同一个配置文件：

```c
struct GatewayConfig config;

if (load_config("config/gateway.conf", &config) < 0) {
    return 1;
}

int client_fd = connect_to_server(config.server_port);
```

这样以后只改：

```text
config/gateway.conf
```

server 和 client 都会连接同一个端口，不需要手动改两个 `.c` 文件。

## Makefile 链接关系

server 使用配置模块，所以需要：

```makefile
src1 := src/server.c src/protocol.c src/command.c src/logger.c src/config.c
```

client 也使用配置模块，所以需要：

```makefile
src2 := src/client.c src/protocol.c src/config.c
```

如果漏掉 `src/config.c`，会出现：

```text
undefined reference to `load_config'
```

## 运行验证

编译：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day33_device_gateway_config
make clean
make
```

终端 1：

```bash
make run1
```

期望输出：

```text
demo_gateway server listening on port 9100
```

终端 2：

```bash
make run2
```

输入：

```text
status
led on
abc
quit
```

查看日志：

```bash
cat logs/server.log
```

实际日志包含：

```text
[INFO] server starting
[INFO] server config loaded
[INFO] client connected
[INFO] request received: status
[INFO] response built: OK device=online temp=32 voltage=24
[INFO] request received: led on
[INFO] response built: OK led=on
[INFO] request received: abc
[INFO] response built: ERR unknown command
[INFO] client disconnected
```

## 今日易错点

1. `load_config` 函数名要和头文件声明一致，不能写成 `log_config`。
2. 配置字段名要统一：`log_path` 不是 `load_path`。
3. `sscanf` 格式必须写成 `"%127[^=]=%127s"`，中间的 `=` 不能漏。
4. `char log_path[]` 是数组，不能写 `config->log_path = ...`。
5. `snprintf()` 已经负责把字符串写进数组，不需要再赋值。
6. `value` 数组长度最好和目标字段长度一致，避免截断警告。
7. server 和 client 都使用配置模块时，Makefile 两边都要链接 `src/config.c`。

## 今日完成状态

- [x] 新增 `config/gateway.conf`
- [x] 新增 `include/config.h`
- [x] 新增 `src/config.c`
- [x] 实现默认配置
- [x] 实现 `key=value` 配置解析
- [x] server 使用配置文件中的端口
- [x] server 使用配置文件中的日志路径
- [x] server 输出配置中的设备名
- [x] client 使用配置文件中的端口
- [x] Makefile 链接 `src/config.c`
- [x] 验证 server/client 在 9100 端口通信

## 今日总结

Day33 把设备网关从“代码里写死参数”推进到了“通过配置文件控制行为”。

这一步在嵌入式 Linux 应用开发里非常常见：程序部署到设备后，很多参数不应该重新编译，而应该通过配置文件修改。例如端口号、日志路径、设备名、采样周期、串口名、波特率等。

现在这个小网关已经具备了三个真实服务的基础特征：

```text
模块化代码结构
日志记录
配置文件
```
