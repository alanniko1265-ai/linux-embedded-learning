# Day29 TCP 请求-响应协议服务器

## 今日目标

在 Day28 `length + payload` 应用层协议基础上，把单向发送升级为请求-响应模型。

今天完成的通信模式：

```text
client 输入命令
  ↓
client send_message(request)
  ↓
server read_message(request)
  ↓
server build_response(request)
  ↓
server send_message(response)
  ↓
client read_message(response)
  ↓
client 打印响应
```

## 工程路径

Windows：

```text
C:\Users\20858\Documents\实习\linux_projects\day29_request_response
```

WSL：

```bash
/mnt/c/Users/20858/Documents/实习/linux_projects/day29_request_response
```

VS Code：

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/day29_request_response
code .
```

## 工程文件

```text
day29_request_response/
├── src/
│   ├── server.c
│   └── client.c
├── build/
│   ├── server
│   └── client
└── Makefile
```

## 编译运行

手动编译：

```bash
mkdir -p build
gcc -Wall -g -O0 src/server.c -o build/server
gcc -Wall -g -O0 src/client.c -o build/client
```

Makefile 编译：

```bash
make clean
make
```

终端一：

```bash
make run1
```

终端二：

```bash
make run2
```

## 协议格式

今天继续使用 Day28 的协议：

```text
+----------------+----------------+
| 4 字节长度字段  | payload 正文    |
+----------------+----------------+
```

但方向变成双向：

```text
client -> server：request
server -> client：response
```

也就是说，client 和 server 都需要：

```text
read_exact
send_exact
read_message
send_message
```

## read_exact 与 send_exact

`read_exact`：

```text
一直 recv，直到读满 size 字节
```

`send_exact`：

```text
一直 send，直到发满 size 字节
```

它们解决的是 TCP 字节流的两个基本问题：

```text
read_exact  解决接收不完整
send_exact  解决发送不完整
```

内部共同逻辑：

```text
total 表示已经处理的字节数
ptr + total 表示下一次继续处理的位置
size - total 表示剩余字节数
```

`send_exact` 使用：

```c
const char *ptr = buffer;
```

因为发送时只读取 `buffer`，不修改它。

`read_exact` 使用：

```c
char *ptr = buffer;
```

因为接收时要把数据写进 `buffer`。

## read_message

读取一条完整消息：

```text
read_exact 读取 4 字节 net_length
  ↓
ntohl 转成本机 length
  ↓
检查 length 是否合法
  ↓
read_exact 读取 payload
  ↓
补 '\0'
  ↓
返回 payload 长度
```

关键检查：

```c
if (length == 0 || length > MAX_PAYLOAD_SIZE || length >= buffer_size)
```

`length >= buffer_size` 必须拦住，因为后面会写：

```c
buffer[length] = '\0';
```

如果 `length == buffer_size`，这个下标已经越界。

## send_message

发送一条完整消息：

```text
strlen 得到 payload 长度
  ↓
htonl 转成网络字节序
  ↓
send_exact 发送 4 字节长度字段
  ↓
send_exact 发送 payload 正文
```

注意：

```c
send_exact(fd, message, length);
```

这里的 `length` 只是本地参数，表示从 `message` 开始发多少字节，不需要 `htonl`。

只有真正作为协议字段发送的整数才需要 `htonl`。

## build_response

server 根据请求生成响应：

```text
status   -> OK device=online temp=32 voltage=24
led on   -> OK led=on
led off  -> OK led=off
reboot   -> OK reboot scheduled
其他命令 -> ERR unknown command
```

代码中比较字符串带 `\n`：

```c
strcmp(request, "status\n") == 0
```

原因是 client 使用：

```c
fgets(request, sizeof(request), stdin);
```

`fgets` 会保留用户按下回车产生的换行符。

## server 流程

```text
create_server_socket
  ↓
accept 一个 client
  ↓
while (1)
  ↓
read_message 读取 request
  ↓
build_response 生成 response
  ↓
send_message 发回 response
```

今天 server 只处理一个 client。

这是正常设计：Day29 重点是请求-响应协议闭环，不是并发模型。后面可以把这套协议函数合进 `epoll` 多客户端 server。

## client 流程

```text
connect_to_server
  ↓
循环 fgets 读取命令
  ↓
quit 则退出
  ↓
send_message 发送 request
  ↓
read_message 读取 response
  ↓
打印 response
```

request / response 都使用：

```c
char request[MAX_PAYLOAD_SIZE + 1];
char response[MAX_PAYLOAD_SIZE + 1];
```

`MAX_PAYLOAD_SIZE` 表示最大有效内容长度，额外的 `+1` 留给字符串结束符 `'\0'`。

## 运行结果

client 端：

```text
connected to request-response server
> status
[response] OK device=online temp=32 voltage=24
> led on
[response] OK led=on
> led off
[response] OK led=off
> reboot
[response] OK reboot scheduled
> abc
[response] ERR unknown command
```

server 端：

```text
request-response server listening on port 9000
client connected
[request] status
[request] led on
[request] led off
[request] reboot
[request] abc
client disconnected
```

## 今日遇到的问题

1. `send_exact` 的 `total` 必须初始化为 `0`，否则循环使用随机值。
2. `uint32_t` 是无符号类型，不能判断 `< 0`。
3. `send_exact` 只有 3 个参数，不能把 `send` 的 flags 参数也传进去。
4. `accept` 的第一个参数必须是 `server_fd`，返回值才是 `client_fd`。
5. `build_response` 第三个参数应该是 `sizeof(response)`。
6. `fgets` 返回指针，应判断 `== NULL`。
7. 同一作用域中不能重复定义两次 `int n`。
8. `request` 缓冲区也应使用 `MAX_PAYLOAD_SIZE + 1`。

## 今日完成状态

- [x] 完成 `read_exact`
- [x] 完成 `send_exact`
- [x] 完成 `read_message`
- [x] 完成 `send_message`
- [x] 完成 server 命令解析 `build_response`
- [x] 完成单连接 request-response server
- [x] 完成交互式 request-response client
- [x] 手动编译 server/client
- [x] 编写 Makefile
- [x] 完成 client/server 联调
