# Client TransService / NetWorker 重构设计总结（Draft）

## 0. 背景与目标

本项目正在进行客户端传输层（Client Transport）的重构，核心目标是：

- 将 UI 与网络通信逻辑解耦，避免 UI 直接管理线程、socket、收发与协议细节。
- 建立统一的异步传输服务：同一套机制覆盖 `/test`、下载、块请求等所有网络通信。
- 明确 `TransService` 与 `NetWorker` 的职责边界，形成可维护、可扩展、面试可讲述的架构。
- 采用 Qt 的事件循环模型实现异步 I/O（`QTcpSocket` + signals/slots），同时支持连接池、并发与 idle 回收。

## 1. 关键术语

- **ClientApp**：应用挂载点（composition root），负责组装 UI 与业务组件及其生命周期。
- **TransService**：对外的“传输用例服务/门面（Facade）”，面向业务提供 API 与结果信号，负责路由、关联、错误语义。
- **NetWorker**：网络引擎（I/O engine），运行在专用网络线程中，负责 socket I/O、连接池、组帧与网络事件。
- **FrameAssembler**：对单条 TCP 字节流做组帧（从字节流中恢复出完整 frame）。必须 per-connection。
- **request_id**：请求关联 ID（correlation id），用于将某个请求与其响应精确匹配。
- **task_id / transfer_id**：业务任务 ID（可选），用于将多个请求聚合成一个下载/上传任务。

## 2. 总体架构与依赖方向

### 2.1 推荐的对象关系（第一版）

- `ClientApp` 负责创建并持有：
  - UI：如 `ClientMainWindow`、对话框
  - 业务编排器：如 `TransManager`
  - `TransService`
- `TransService` 内部私有持有：
  - 网络线程 `QThread`
  - `NetWorker`（moveToThread 到网络线程）

> 关键点：`NetWorker` 作为 `TransService` 的内部实现细节对外不暴露，避免出现“两个并列 Service”导致边界模糊。

### 2.2 依赖与交互原则

- **编译依赖方向**：`TransService -> NetWorker`（TransService 使用 NetWorker）；`NetWorker` 不依赖 `TransService`。
- **运行时交互方式**：
  - `TransService` 通过 signal/slot（queued）向 `NetWorker` 发送“发送请求”等命令。
  - `NetWorker` 通过 signal/slot（queued）向 `TransService` 发布“收到完整帧/连接状态”等事件。

> 这不是“反向调用导致并列”，而是事件驱动（Pub/Sub）。`NetWorker` 不“调用 TransService 方法”，只 emit 事件。

## 3. 职责划分（定稿）

### 3.1 TransService（Facade / 路由层）

- **对外 API**：
  - `send_test_request(endpoint, payload)`
  - `send_download_request(endpoint, payload)`
  - `send_block_request(endpoint, payload)`
  - 未来统一为 `send(Request)` 也可
- **核心职责**：
  - 生成并管理 `request_id`（`std::atomic<uint64_t>`）
  - 构建请求（调用 `MessageHandler` 等序列化设施）
  - 维护 `pending_map`（`request_id -> RequestContext`）
  - 接收 `NetWorker` 的 `frameReady` 事件后：解析响应、通过 request_id 精确匹配、分发到对应信号
  - 输出业务语义结果：成功/失败/进度/数据块

### 3.2 NetWorker（I/O 引擎 / 连接池管理）

- **运行线程**：专用网络线程（`QThread::exec()` 事件循环）。
- **核心职责**：
  - 管理连接池：按 endpoint 维护多连接
  - socket 创建与复用：动态创建/复用连接
  - 读写：`readyRead` 驱动读；写入通过 `QTcpSocket::write` 非阻塞
  - per-connection 组帧：持有 `FrameAssembler`
  - idle 超时回收：空闲连接超过阈值关闭并移出池
  - 产生事件：`frameReady(...)`、`connectionError(...)`、`connected(...)` 等

- **不做的事（边界约束）**：
  - 不解析业务语义（不区分 test/download/block）
  - 不维护 request_id 对应的业务映照表
  - 不直接接触 UI/TransManager

## 4. 线程模型与 Qt 事件循环

### 4.1 Qt 网络线程正确姿势

- `QTcpSocket` 必须在其所属线程的事件循环中工作。
- 推荐模式：
  - `NetWorker` 是 `QObject`，moveToThread 到网络线程
  - socket 在 `NetWorker` 所在线程创建并使用
  - 使用 `readyRead`/`disconnected`/`errorOccurred` 等信号进行事件驱动 I/O

### 4.2 跨线程通信

- `TransService`（主线程） <-> `NetWorker`（网络线程）通过 signals/slots。
- 连接方式使用 `Qt::QueuedConnection`，确保跨线程安全。

## 5. 接收路径：组帧与上抛

### 5.1 为什么组帧应放在 NetWorker

- 组帧是“字节流 -> frame”的传输层工作，属于 I/O engine。
- TransService 只应接收“完整帧”进行业务解析与路由。

### 5.2 FrameAssembler 必须 per-connection

- 每条 TCP 连接是一条独立字节流。
- 多连接共享一个 assembler 会导致不同连接数据混淆。

推荐结构：

- `ConnectionContext { socket, FrameAssembler assembler, lastActive, busyFlag, ... }`

### 5.3 ConnectionContext / Receiver（建议作为 NetWorker 内部实现）

为避免将 `FrameAssembler` 误放为 `NetWorker` 的“单例成员”（从而在多连接场景下产生状态混淆），建议在 `NetWorker` 内部为“每条连接”维护一个配套对象，常见命名为 `ConnectionContext` / `Receiver` / `Channel`。

- **职责定位**：
  - 封装“单连接”的 I/O 细节：`QTcpSocket` + 组帧状态机（`FrameAssembler`）。
  - 通过 `readyRead` 驱动读取字节流，持续尝试 `pop_frame()`。
  - 当组装出完整 frame 时，上抛事件给 `NetWorker`（或直接 emit 给 `TransService`）。

- **典型持有字段（概念层面）**：
  - `QPointer<QTcpSocket> socket`
  - `FrameAssembler assembler`
  - `Endpoint endpoint`
  - `connId`（可选，便于诊断与连接池管理）
  - `lastActive`（用于 idle 回收）
  - `busy/inFlight`（用于单连接串行约束）

- **典型事件流**：
  - `readyRead` -> `readAll()` 得到 bytes
  - `assembler.push_data(bytes)`
  - `while (auto frame = assembler.pop_frame())`：
    - 更新 `lastActive`
    - emit/通知：`frameReady(endpoint, connId, frameBytes)`

> 该结构与现有“连接 + 管理器”的思路一致：你早期的 `ConnectionManager/ConnectionGuard` 体现了“连接资源应被封装与回收”的思想；`ConnectionContext` 是将这一思想迁移到 Qt 事件驱动模型后，对“单连接接收路径（含组帧状态）”的更贴切表达。

### 5.4 上抛事件（建议）

`NetWorker` 对上抛：

- `frameReady(endpoint, connId, frameBytes)`

其中 connId 可选，但在调试、诊断、连接池管理时很有用。

> 为降低跨线程传递大块数据的拷贝成本，建议 `frameBytes` 使用 `QByteArray`（implicit sharing）。

### 5.5 “Context/上下文”概念区分（避免命名误解）

项目内同时存在多种“上下文”概念，语义不同，建议区分：

- **客户端 ConnectionContext（传输层/连接级）**：
  - 关注点是“socket + I/O 状态 + 组帧状态机”。
  - 目标是：可靠地产出完整 frame，并维护连接可复用/可回收的状态。

- **服务端 TransContext（业务处理级）**：
  - 当前实现更接近“业务处理上下文”（解析请求、路由到 `/test` `/download` `/block` 等处理函数）。
  - 它虽然也持有 `FrameAssembler`，但其核心价值在于“业务 handler 与资源（buffer/service）组合”。

这与 ffmpeg/boost 中常见的“context = 一组状态 + 资源句柄 + 回调”的思想一致；在本项目里建议用更明确的命名（如 `ConnectionContext` vs `TransContext`），避免“同叫 context 但语义不一致”带来的理解成本。

## 6. request_id 语义与 pending_map

### 6.1 request_id 的定义

- `request_id` 是 **一次网络请求（request）与其响应（response）的关联标识**。
- 由 `TransService` 生成（atomic 自增即可），并写入请求 body。
- 服务端响应应回显相同 `request_id`（建议作为协议约束）。

> request_id **不等于** “完整业务交互/下载任务”。下载任务通常由多个 block 请求组成，需要另设 `task_id` 聚合。

### 6.2 pending_map 的作用

- `pending_map[request_id] = RequestContext{ kind, endpoint, task_id/file_id/block_id, startTime, timeout, ... }`
- 收到响应时：
  - 从响应 body 解析出 `request_id`
  - 查表匹配
  - 发射对应业务信号
  - 从表移除（或标记完成）

### 6.3 request_id 放 body 的可行性

- 当前序列化设施：不主动 `get_field` 不影响旧逻辑。
- request_id 放在 body 中可做到兼容性与低侵入。

## 7. 连接策略：短连接 vs 长连接/连接池

### 7.1 短连接（one request per connection）

- 优点：模型简单；一次收发闭合即可关闭 socket。
- 缺点：高频 block 请求会导致握手开销大、资源压力大（TIME_WAIT/fd）。

适用：低频控制面，如 `/test` 或极少量元数据请求。

### 7.2 长连接 + 连接池（推荐用于 /block）

- 按 endpoint 维护连接池（多连接支持并发）。
- 单连接约束：每连接单 in-flight（串行），并发靠多连接实现。
- idle 超时回收，避免连接无限增长。

适用：块请求、数据面传输。

### 7.3 推荐混合策略（第一版）

- `/test`：允许短连接（完成即关闭）。
- `/block`：使用长连接 + 连接池 + idle 回收。

## 8. 并发模型与约束（第一版建议）

- **endpoint 级并发**：同一 endpoint 可多连接并发。
- **connection 级串行**：单连接同一时刻最多一个 in-flight。
- **队列/调度**：
  - `NetWorker` 从等待队列取请求，选择可用连接发送。
  - 若无可用连接且未达上限则创建新连接。

## 9. 反馈路由到 UI/业务

### 9.1 最简单可落地：signals/slots

- TransService 对外提供：
  - `testFinished(request_id, ok, message)`
  - `downloadMetaReady(request_id, file_id, size, ...)`
  - `blockArrived(request_id, file_id, block_id, data)`
  - `requestFailed(request_id, error)`

UI/TransManager 只订阅 TransService 信号，不关心连接池与线程。

### 9.2 task_id（可选但建议预留）

- 若要支持多任务并发下载、进度汇总：
  - `task_id` 是“下载任务主键”
  - `request_id` 是“某一次块请求主键”

## 10. 迁移建议（从现状到目标）

### 10.1 现状关注点

- 当前存在多种方式收发：`ConnectionThread`（轮询 + sleep）、`ConnectionManager`、以及 Qt socket 的雏形 `NetworkService`。
- 目标是收敛到：`TransService + NetWorker` 单一通道。

### 10.2 建议的阶段性迁移

- 阶段 1：
  - 固定一个最小闭环（例如 `/test`）：NetWorker 组帧 -> TransService parse -> UI signal。
  - 引入 request_id + pending_map，但先只用于 `/test`。

- 阶段 2：
  - 将 `/block` 迁移到 NetWorker 事件驱动模型
  - 引入连接池与 idle 回收

- 阶段 3：
  - 统一下载/上传任务编排（TransManager/TransService 的边界再精炼）
  - 需要时引入 task_id + 下载状态机

## 11. 面试可讲述点（亮点提炼）

- **架构边界清晰**：Facade（TransService） vs Engine（NetWorker）。
- **线程模型正确**：Qt event loop + QTcpSocket 所在线程一致。
- **连接池设计**：多连接并发、单连接串行、idle 回收。
- **可扩展关联机制**：request_id + pending_map，解决并发响应归属。
- **演进路线明确**：先闭环、后扩展性能与可维护性。

---

## 12. 待确认/决策项（建议在实现前拍板）

- 服务端是否保证在 response 中回显 `request_id`。
- `/block` 第一版是否直接使用长连接 + 连接池（推荐）。
- 是否引入 `task_id`（多任务/断点续传会需要）。

