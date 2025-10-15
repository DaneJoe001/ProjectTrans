## cpp_project_trans

一个以“网络框架”为核心的个人项目：在自研的跨平台网络基础设施之上，提供客户端与服务端（含可唤醒的 Qt 面板）作为端到端演示，用于验证传输、存储与交互能力，并为后续能力扩展打底。

### 项目亮点（Highlights）
- 事件驱动网络：自研 epoll 事件循环 + 非阻塞 POSIX Socket，抽象 `IEventLoop/ISocketContext/Posix*Socket`，完成连接管理与可读/可写分发
- 自定义二进制序列化协议：网络字节序、字段头（name/type/flag/len）、Array/Map/Dictionary，支持泛型（反）序列化
- 端到端演示：Server/Client + Qt6 面板（可隐藏后唤醒），SQLite 持久化与日志体系
- 可扩展架构：上下文工厂与协议切换、模块化目录、清晰分层，便于二次开发

### 技术栈（Tech Stack）
- C++20、CMake、Qt6（Core/Gui/Widgets）
- POSIX Socket、epoll
- OpenSSL、SQLite/SQLiteCpp

### 快速开始（Quick Start）
依赖：CMake ≥ 3.20、C++20 编译器、Qt6（Core/Gui/Widgets）、OpenSSL、SQLite/SQLiteCpp。

```bash
# 构建（Release）
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# 生成可执行文件：server、client
```

运行：

```bash
# 服务端（默认监听 0.0.0.0:8080）
./server

# 客户端（GUI）
./client

# 或启用控制台客户端（在代码中打开 CONSOLE_CLIENT 宏后编译）
```

提示：当前仓库 CMake 使用了部分绝对路径引入外部库（如 SQLiteCpp、自研库），如需一键构建，请按环境修改 `CMakeLists.txt` 中相应路径，或使用本地子模块/FetchContent（Roadmap 中有计划迁移）。

### 功能与架构（Architecture）
- 网络层：`EpollEventLoop` 负责事件分发；`PosixServerSocket/PosixClientSocket` 封装套接字；`ISocketContextCreator` 注入业务上下文
- 序列化层：`DaneJoeSerializer` 定义 `MessageHeader/Field` 等线格式，统一网络字节序，支持变长字段与复合容器
- GUI：Qt6 Widgets 面板展示连接/文件等信息，默认随进程启动，关闭仅隐藏，可通过信号唤醒
- 持久化与日志：SQLite 记录元数据，配合自研日志组件做问题定位

简单链路：

```
Client -> (serialize) -> Socket -> EpollEventLoop -> Context -> (deserialize) -> Repo/Service/DB
```

### 协议概览（Protocol Overview）
- 统一网络字节序（big-endian）；消息由 `MessageHeader` + 若干 `Field` 组成
- Field: [name_len | name | type | flag | (value_len) | value]
- 支持 Array/Map/Dictionary；字符串/变长容器携带长度信息
- 版本与校验（checksum）字段已预留，后续将默认启用

参考代码：`include/common/network/danejoe_serializer.hpp`、`source/common/network/danejoe_serializer.cpp`

### 目录概览（Layout）
- `source/common/`：通用网络/数据库/日志（网络框架核心在此演进）
- `source/server/`：服务端入口、业务仓储、面板（Qt Widgets）
- `source/client/`：客户端入口与基础交互
- `include/`：对外头文件

### 面板唤醒（GUI Wake-up）
- 面板默认随进程启动显示，关闭仅隐藏，不影响服务端线程
- 可通过信号触发唤醒（如 SIGQUIT）：信号置位 → GUI 线程定时器检测 → `show/raise/activateWindow`

### 截图 / 演示（Screenshots / Demo）
- 面板截图（待补：`document/images/panel.png`）
- 10–15s 演示 GIF（待补：`document/images/demo.gif`）

### 路线与计划（Roadmap）
- 构建：移除外部库绝对路径，引入 FetchContent/子模块；提供 Dockerfile 与 CI
- 协议：默认启用 version/length/checksum 校验，完善边界与模糊测试
- 事件循环：背压策略与 EPOLLOUT 按需注册；必要时引入线程池处理读写
- 安全：TLS 通道接入（基于 OpenSSL）；日志脱敏与限流
- 文档：补充架构图、协议字段表、抓包/日志示例

### 维护（Maintenance）
- 个人维护项目：欢迎 Issue/建议，节奏以稳定与清晰为主

### 许可证（License）
- 本项目以 LGPL-3.0-or-later 授权
