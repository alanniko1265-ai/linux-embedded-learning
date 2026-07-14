# ConnectPanel — Qt6 网络/串口通信调试面板

基于 Qt6 的跨平台网络通信调试面板，集成 TCP Socket 与串口通信功能。

[![Qt](https://img.shields.io/badge/Qt-6.5%2B-brightgreen?logo=qt)](https://www.qt.io/)

## 功能特性

- **TCP 客户端** — 支持自定义 IP/端口连接，发送文本消息，实时接收并显示服务器响应
- **串口通信** — 自动扫描系统可用串口，支持波特率、数据位、停止位、校验位等参数配置
- **日志系统** — 带时间戳的通信日志，支持清空与导出为 .txt 文件
- **设置持久化** — 使用 QSettings 自动保存/恢复用户配置（IP、端口、串口参数等）
- **后台线程** — 通过 QThread + Worker 模式处理耗时任务，保持 UI 流畅
- **Fusion 风格** — 跨平台一致的界面外观

## 项目结构

```
ConnectPanel/
├── main.cpp                    # 应用入口，设置 Fusion 风格
├── mainwindow.h                # 主窗口声明
├── mainwindow.cpp              # 主窗口逻辑（TCP/串口交互、UI状态管理）
├── mainwindow.ui               # Qt Designer 界面布局文件
├── worker.h / worker.cpp       # 后台 Worker 线程
├── logmanager.h / .cpp         # 日志管理器（添加、清空、导出）
├── tcpclientmanager.h / .cpp   # TCP 客户端管理器（连接、收发、断线处理）
├── serialportmanager.h / .cpp  # 串口管理器（扫描、打开、关闭、收发）
├── settingsmanager.h / .cpp    # 设置持久化管理器
├── CMakeLists.txt              # CMake 构建配置
└── qt_day04_first_app/         # 附属：Qt 第一个应用
```

## 技术栈

| 模块 | 说明 |
|------|------|
| **Qt 6.5+** | Core / Widgets / Network / SerialPort |
| **构建系统** | CMake 3.19+ |
| **编译器** | MinGW 64-bit (GCC) / MSVC / Clang |
| **界面风格** | Fusion Style |
| **平台** | Windows / Linux / macOS |

## 构建运行

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/mingw_64
cmake --build build
./build/ConnectPanel
```

或在 Qt Creator 中直接打开 `CMakeLists.txt` 构建运行。

## 使用说明

### TCP 客户端
1. 在 **TCP Connection** 面板输入目标 IP 地址和端口号
2. 点击 **Connect** 建立连接
3. 在消息输入框输入文本，点击 **Send** 发送
4. 点击 **Disconnect** 断开连接

### 串口通信
1. 切换到 **Serial Port** 选项卡
2. 点击 **Scan** 扫描可用串口，配置参数
3. 点击 **Open** / **Close** 管理串口连接

### 日志管理
- **Clear Log** — 清空当前日志
- **Save Log** — 将日志导出为 .txt 文件

---

*仅供学习使用。*
