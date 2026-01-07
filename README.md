## cpp_project_trans

一个以“网络传输/协议/运行时”为核心的个人项目：在自研的 C++ 网络与序列化基础设施之上，提供可运行的 Client/Server 端到端 demo（`client/`、`server/`）。当前仓库中的 `server/common/client` 核心代码均由我独立实现。

说明：基础设施部分在我还有一个独立的库仓库中维护；由于库组件仍在迭代完善，为了便于快速联调与调整，我将其源码直接纳入本仓库的 `common/` 中作为组件模块编译，后续会将成熟的实现反推回独立库仓库。

说明：单元测试主要在独立的基础设施库仓库中维护；本仓库侧重端到端演示与联调。

### 项目亮点（Highlights）
- 清晰的分层与边界：`common/`（基础设施）+ `server/`（运行时/业务）+ `client/`（交互/调度），便于迭代与扩展
- 双栈网络实现：Client 侧使用 Qt 网络组件（如 `QTcpSocket`）进行收发；Server 侧使用自研 POSIX 网络封装（`PosixSocketHandle` + `PosixEpollEventLoop`）驱动 IO
- 事件驱动网络（Linux）：`PosixEpollEventLoop` + 非阻塞 `PosixSocketHandle`，IO 线程负责 accept/读写分发；业务线程独立处理请求
- 跨线程解耦：`ReactorMailBox` 作为 IO 线程与业务线程之间的帧通道，使用 `eventfd` 唤醒 `epoll_wait`
- 自定义二进制协议：`SerializeHeader/SerializeField/SerializeCodec` 负责字段级序列化；`FrameAssembler` 负责流式组帧
- 端到端演示：Client/Server（Qt Widgets）+ SQLite3 持久化与日志体系；支持 `/test`、`/download`、`/block` 请求

### 技术栈（Tech Stack）
- C++20、CMake、Qt6（Core/Gui/Widgets/Sql/Network）
- POSIX Socket、epoll
- OpenSSL、SQLite3

### 快速开始（Quick Start）
依赖：CMake ≥ 3.20、C++20 编译器、Qt6（Core/Gui/Widgets/Sql/Network）、OpenSSL、SQLite3。

```bash
# 推荐：使用 CMake Presets（Linux 示例）
cmake --preset linux-gcc-release
cmake --build --preset linux-gcc-release

# 或者：传统方式
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

运行：

```bash
# 可执行文件名以 CMake 目标为准（通常位于 build 目录下）
# - 服务端：ProjectTransServerApp
# - 客户端：ProjectTransClientApp

./build/<preset>/server/ProjectTransServerApp
./build/<preset>/client/ProjectTransClientApp
```

提示：当前服务端网络监听默认绑定 `127.0.0.1:8080`（见 `server/source/runtime/network_runtime.cpp`），因此客户端默认以本机回环访问。

提示：
- 默认 `ADD_QT_LIB=ON` 且 `BUILD_*_GUI_APP=ON`，会构建 Qt Widgets GUI。
- 当前 `console_main.cpp` 也依赖 `QApplication`，因此即使 `BUILD_CLIENT_GUI_APP=OFF` 也仍需要 Qt（仅是入口不同）。

提示：当前尚未引入完整的配置管理系统，因此部分参数（如监听地址/端口、日志路径、数据库路径）暂以代码内默认值为准，后续会再演进。

可选构建开关（示例）：

```bash
# 仅构建 client 或 server（顶层 superbuild 开关）
cmake -S . -B build -DBUILD_CLIENT_SUBPROJECT=ON -DBUILD_SERVER_SUBPROJECT=OFF

# 关闭 GUI 入口（但当前仍需 Qt 依赖；详见上方提示）
cmake -S . -B build -DBUILD_CLIENT_GUI_APP=OFF
```

### Simple Server（示例用极简服务端）
`simple_server/` 为早期用于演示协议收发与文件分块下载逻辑的极简服务端（不依赖 Qt）。

提示：重构后该子工程当前处于不可用状态，README 暂保留其说明作为历史参考，后续会在修复后恢复。

构建：

```bash
cmake -S simple_server -B build/simple_server -DCMAKE_BUILD_TYPE=Release
cmake --build build/simple_server -j
```

运行：

```bash
./build/simple_server/ProjectTransSimpleServerApp --port 8080

# 指定 fixture 文件（否则默认从 simple_server/resource/file/ 下按序选择文件）
./build/simple_server/ProjectTransSimpleServerApp --port 8080 --fixture /path/to/file.bin
```

### 功能与架构（Architecture）
- 线程模型：
  - Server：`ServerApp` 启动 `NetworkRuntime`（IO 线程）与 `BusinessRuntime`（业务线程）
  - Client：`ClientApp` 启动 GUI、`TransService`（内部网络线程）、以及 `BlockScheduleController` 调度线程
- 网络层（Server）：`NetworkRuntime` 初始化 `eventfd + epoll + server socket`，并启动 `PosixEpollEventLoop::run()`
- 跨线程通道：`ReactorMailBox`
  - to_server：IO 线程将收到的 `PosixFrame` 投递给业务线程
  - to_client：业务线程投递响应帧，由 IO 线程 flush；当有待发送数据时会按需开启 `EPOLLOUT`
- 业务层（Server）：`BusinessRuntime` 解析请求信封并分发 `/test`、`/download`、`/block`
- 协议层：`SerializeCodec` 基于 `SerializeHeader/SerializeField` 编解码；`FrameAssembler` 将字节流组装成完整帧
- 持久化与日志：
  - Server：`./database/server/server_database.db`、`./log/server.log`
  - Client：`./database/client/client_database.db`、`./log/client.log`

简单链路：

```
Client(TransService/NetworkService)
  -> (SerializeCodec)
  -> TCP
  -> Server(PosixEpollEventLoop)
  -> ReactorMailBox(to_server)
  -> BusinessRuntime(ServerMessageCodec + Service/Repository/DB)
  -> ReactorMailBox(to_client)
  -> Server(PosixEpollEventLoop)
  -> TCP
  -> Client(ClientMessageCodec)
```

### 协议概览（Protocol Overview）
- 帧由 `SerializeHeader` + 若干 `SerializeField` 构成；`FrameAssembler` 负责从 socket 字节流中按头部长度字段组帧
- `SerializeField` 逻辑结构：`name_length | name | type | flag | (value_length) | value`
- 字符串与变长数组通过 `SerializeFieldFlag::HasValueLength` 携带长度信息
- 头部 `SerializeHeader` 预留了 `flag/checksum/version` 等字段（目前主要用于 framing 与字段计数）

参考代码：
- `common/include/danejoe/network/codec/serialize_header.hpp`
- `common/include/danejoe/network/codec/serialize_field.hpp`
- `common/include/danejoe/network/codec/serialize_codec.hpp`
- `common/include/danejoe/network/codec/frame_assembler.hpp`

### 目录概览（Layout）
- `common/`：通用库（网络/并发/诊断/日志/数据库/序列化）
- `server/`：服务端（Qt Widgets + NetworkRuntime/BusinessRuntime + repository/service）
- `client/`：客户端（Qt Widgets + TransService/BlockScheduleController 等）
- `simple_server/`：不依赖 Qt 的极简服务端示例（便于快速验证协议/下载链路）

### 面板唤醒（GUI Wake-up）
- Server/Client 默认随进程启动显示 Qt 窗口
- Server 支持通过快捷键切换窗口显示/隐藏：`Ctrl + Shift + G`（见 `ServerApp::eventFilter()`）

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
