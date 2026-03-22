# 项目经历

---

## 一、简历版（精简）

---

### 项目一：AI Agent 工程化实践 —— 面向自然语言生成前后端应用的工具调用系统

**项目：个人工程实践　　角色：独立设计与开发　　技术栈：Node.js, Anthropic Claude, Tool Calling, SSE/Event Stream, JSON Schema, MongoDB**

- 面向自然语言驱动的应用搭建场景，独立设计并实现工具调用型 Agent 系统，支持从用户需求生成完整前后端项目，包括前端页面、后端接口与数据表结构。
- 设计并实现基于 ReAct 的多步决策与工具调用链路，支持 run 生命周期管理、流式事件返回与多轮任务执行。
- 封装页面、数据表与后端能力相关工具，支持页面生成/修改、MongoDB 数据表定义生成，以及通用 CRUD 接口与 `user` 等 feature 接口生成，前端代码可直接调用后端接口。
- 为 Agent 增加步数限制、doom loop 检测、工具成功/失败事件上报等控制机制，并结合结构校验降低模型直接写业务对象的风险，提升多步推理场景下的稳定性与可诊断性。

---

### 项目二：Citrix Workspace App — Client-Session 文件双向拷贝

**公司：Citrix (Cloud Software Group)　　角色：主力开发　　技术栈：C/C++, FUSE, POSIX IPC, gRPC, Protobuf**

- 负责 Linux 客户端与远程 Windows 会话之间的文件双向拷贝能力建设，支持用户在远程会话与本地文件管理器之间直接复制粘贴文件。
- 设计并实现基于 FUSE 的虚拟文件系统，将远程文件以标准文件系统语义暴露给本地，支持按需读取与流式传输，避免预下载带来的存储与时延开销。
- 设计三层通信链路：gRPC 负责跨会话剪贴板协调，POSIX 共享内存/信号量负责 wfica 与 FUSE daemon 间高效传输，GDT 协议负责通过 ICA 虚拟通道传递文件数据。
- 实现跨会话文件路由与大文件分块重组机制，支持 Server-to-Client / Client-to-Server 双向拷贝场景。
- 实现流级别并发隔离与一致性控制，通过独立 SHM/Semaphore、per-stream mutex 和 `direct_io=1` 避免并发读竞争及缓存不一致问题。

---

### 项目三：Citrix Workspace App — HDX Direct v2 网络直连优化

**公司：Citrix (Cloud Software Group)　　角色：主力开发　　技术栈：C, Golang, STUN (RFC 5389), UDP/TCP, pthread**

- 负责 HDX Direct v2 功能设计与实现，推动客户端绕过 Citrix Gateway 直连 VDA，将 RTT 从约 80ms 降低至 10-15ms，显著提升远程桌面交互体验。
- 设计并实现三路并行连接竞争机制，同时尝试 HDX Direct UDP、Gateway UDP 中继和 TCP via Gateway，首个成功连接胜出，其余自动取消，兼顾低延迟与连接成功率。
- 实现基于 STUN 的 NAT 穿透能力，并通过多服务器并发探测与指数退避重试支持外网场景下的 UDP 打洞直连。
- 实现连接热切换机制，通过读写锁完成传输层原子替换，使会话能够从 Gateway 中继无感切换到直连链路。
- 实现 Session Reliability + Auto Client Reconnect 两阶段重连，并基于 CTX_LAYER 设计可插拔协议栈，提升复杂网络环境下的可用性与扩展性。

---

### 项目四：Citrix Workspace App — Client-Session USB 设备重定向

**公司：Citrix (Cloud Software Group)　　角色：主力开发　　技术栈：C/C++, Linux USB 子系统, ICA 虚拟通道, inotify, 共享内存**

- 负责 Linux 客户端 USB 设备重定向功能开发与维护，实现客户端物理 USB 设备向远程会话的透传使用，支持 Control/Bulk/Interrupt/Isochronous 四类传输。
- 设计覆盖 Linux USB 子系统、Redirector 核心引擎、VDGUSB 虚拟通道和 Desktop Viewer 的四层架构，打通设备发现、策略过滤、状态管理与远程透传链路。
- 实现基于 inotify 的热插拔检测与 USB 描述符解析，并设计覆盖设备发现、接入、暂停、拔出和异常处理的 10 状态生命周期状态机。
- 设计并实现高性能 USB 设备重定向引擎，支持大规模并发设备重定向，确保低延迟、高吞吐的远程 USB 设备使用体验。
- 实现跨会话设备锁定机制，确保同一设备不会被多个远程会话同时访问，避免设备竞争和数据损坏。

---
---

## 二、详细描述版

---

### 项目一：AI Agent 工程化实践 —— 面向自然语言生成前后端应用的工具调用系统

**项目：个人工程实践　　角色：独立设计与开发**
**技术栈：Node.js, Anthropic Claude, Tool Calling, SSE/Event Stream, JSON Schema, ReAct Loop, MongoDB**

#### 1. 项目背景

这是一个面向自然语言生成前后端应用的个人工程实践项目。目标不是做一个只会对话的 Demo，而是让模型能够基于用户需求，在真实业务约束下生成前端页面、后端接口和数据结构，并通过受控工具完成应用搭建与修改，再以流式事件把执行过程返回给前端或上层系统。

项目参考了真实业务系统中的页面编辑与应用搭建场景，我将其抽象为一个独立的工具调用系统进行设计与实现。当前实现已经具备第一版工程骨架：能够创建 run、执行多步 Agent loop、调用工具、返回流式事件，并承载后续工程化升级。

#### 2. 核心目标

- 让 Agent 能够基于自然语言需求稳定执行多步任务，完成前端页面、后端接口与数据结构的协同生成，而不是只输出自然语言建议。
- 让页面编辑、MongoDB 数据表定义、通用 CRUD 接口与 feature 接口生成沉淀为可控的工具调用，而不是让模型直接生成不受约束的最终写入结果。
- 为后续生产化补齐评估、观测、恢复与治理能力，形成可持续演进的 Agent 系统。

#### 3. 系统链路设计

当前核心执行链路如下：

```text
createRun
  -> runService.executeRun
    -> runAgent
      -> agentLoop
        -> tool execution
          -> event stream
```

各模块职责划分：

- `agentRunsApi.js`：提供创建 run、查询 run、拉取事件流等 API。
- `runService.js`：维护 run 生命周期、事件记录、订阅与流式输出。
- `index.js`：组装工具集、上下文摘要与 system prompt，进入 Agent 主循环。
- `loop.js`：执行 ReAct 式多步循环，处理模型返回、工具调用、错误与终止条件。
- `tools/`：封装页面、数据表等领域工具，隔离模型与底层业务对象。

#### 4. 核心实现

##### 4.1 Run 生命周期与事件流

我设计了 run 级抽象，把一次 Agent 执行建模为独立 run，并维护以下状态与事件：

- 状态：`queued`、`running`、`completed`、`failed`、`cancelled`
- 事件：`run.created`、`run.started`、`run.token`、`run.tool_started`、`run.tool_succeeded`、`run.tool_failed`、`run.completed`、`run.failed`

前端或调用方可以通过 `sinceSeq` 增量拉取事件，从而拿到近实时执行过程。这一层让 Agent 不再是黑盒，而具备了最初级的“可追踪执行链路”。

##### 4.2 ReAct 式 Agent Loop

在 `agentLoop` 中，我实现了典型的“模型输出 -> 判断是否调用工具 -> 执行工具 -> 回填结果 -> 继续下一轮”的 ReAct 模式：

- 支持多步推理与多次工具调用
- 设置 `MAX_STEPS` 限制，避免无限循环
- 对 `stop_reason` 做分类处理，例如 `end_turn`、`tool_use`、`max_tokens`
- 把工具执行结果重新注入 history，驱动下一轮推理

相比单轮 prompt，这种设计更适合真实业务场景中的应用搭建任务，因为很多任务必须先读应用信息、再读页面与数据结构、再决定页面 patch、MongoDB 表结构或后端接口生成方案。

##### 4.3 工具层封装与结构约束

我将页面编辑、数据结构与后端能力抽象为独立工具，典型包括：

- `read_app`
- `read_page`
- `update_page`
- `replace_page_schema`
- `read_data_catalog`
- `read_data_table`
- `create_data_table`

在目标能力上，这套工具体系承载的是从自然语言需求到完整应用结构的受控生成，包括：

- 前端页面与页面 schema 生成/修改
- 基于 MongoDB 的数据表定义生成
- 通用 CRUD 接口生成，供前端直接调用
- 业务 feature 接口生成，例如 `user` 相关查询、创建、更新等接口

工具层的核心价值有两点：

1. 把模型能力约束在明确的输入输出契约中，降低直接生成任意业务对象的风险。
2. 把“读”和“写”动作显式化，便于后续补权限控制、审计和 dry-run。

其中部分工具已经做了基础输入清洗与结构校验，例如数据表字段名、字段类型、允许的 setting key 等，避免模型直接写入非法结构。

##### 4.4 稳定性保护

为了防止 Agent 在多步循环中失控，我增加了几类基础保护：

- **步数限制**：超过最大步数后终止
- **doom loop 检测**：若同一工具以相同输入连续重复调用达到阈值，则判定为循环失控并返回错误结果
- **工具事件上报**：记录工具开始、成功、失败，方便后续定位问题
- **异常兜底**：对模型异常停止、token 超限、工具异常统一转为 run 失败事件

这些保护虽然还不是完整的生产级治理，但已经能显著提升 Agent 原型在真实页面修改任务中的可控性。

#### 5. 当前工程化缺口与升级路线

在审视现有实现后，我明确把后续升级重点放在 4 层：

##### 5.1 Eval

当前 Agent 能运行，但还缺少系统化评估。后续需要构建最小评估集，覆盖新建页面、修改页面、多步 patch、工具失败、非法 patch 等场景，并通过批量跑分来回答：

- 当前成功率是多少
- 失败主要集中在哪些 case
- 哪些优化真正带来了收益

##### 5.2 Observability

当前已有事件流，但仍属于“可看过程”，还不算“可诊断系统”。后续需要补：

- `trace_id`
- step/tool 级 latency
- terminate reason
- tool failure 分类
- run 级聚合指标

这样才能把一次失败 run 从“看到失败”升级到“定位根因”。

##### 5.3 Durability / Recovery

当前 run 和 events 主要存在内存中，进程重启后会丢失，因此后续需要把 run 元数据、事件流和状态机迁移到持久化存储中，并支持 `resume`、重连拉流与幂等保护。

##### 5.4 Security / Governance

当前已经通过工具层做了初步隔离，但对于页面写操作而言还不够。后续需要补：

- 工具权限分级（只读 / 低风险写 / 高风险写）
- policy check 或 dry-run
- patch 作用范围限制
- 审计与变更确认机制

这部分是让 Agent 从“能改页面”走向“受控改页面”的关键。

#### 6. 项目价值

这个项目的价值不在于“做了一个会调工具的 Agent”，而在于：

- 我把 Agent 落到了真实业务页面编辑场景，而不是停留在聊天 Demo
- 我把执行链路、工具边界、事件流和治理问题拆解成明确的工程模块
- 我把过去在复杂系统、状态管理、失败控制方面的工程经验迁移到了 Agent 场景，能够清楚回答一个 Agent 系统从原型走向生产化缺的不是更多功能，而是 Eval、观测、恢复和治理

这也是我把它作为 Agent 开发工程师方向核心项目的原因。

---

### 项目二：Citrix Workspace App — Client-Session 文件双向拷贝

**公司：Citrix (Cloud Software Group)　　角色：主力开发**
**技术栈：C/C++, FUSE (用户态文件系统), POSIX SHM/Semaphore, gRPC/Protobuf, GDT 协议**

#### 1. 项目背景

Citrix Workspace App (ICA Client) 是企业级远程桌面客户端，用户通过它连接到远程 Windows 会话进行办公。文件拷贝功能（File Copy S2C/C2S）是剪贴板重定向中最复杂的子功能——用户在远程 Windows 会话中复制文件后，可以在本地 Linux 文件管理器（Nautilus/Thunar/cp 命令）中直接粘贴，反之亦然。

#### 2. 技术挑战

- 远程文件数据需要跨网络、跨进程、跨操作系统传输，涉及 Windows 文件描述符格式（FILEDESCRIPTORW, UTF-16LE）到 Linux POSIX 文件属性的转换。
- 文件可能很大（几百 MB），不能预先下载，必须按需读取。
- wfica（ICA 引擎进程）和 FUSE daemon 是两个独立进程，需要高效的零拷贝通信。
- 多个 ICA 会话之间需要协调剪贴板所有权和数据路由。

#### 3. 架构设计

##### 三层 IPC 通信架构

```
Remote Windows Session (VDA)
    │ ICA Virtual Channel ("Virtual_Clipboard")
    │ WFCLIP_PLACE → WFCLIP_RENDER_REQUEST → WFCLIP_RENDER_REPLY
    ▼
wfica 进程 (ICA Client Engine)
    │ POSIX 共享内存 + 命名信号量 (零拷贝)
    │ /fuse_shm_fgd_<PID>, /fuse_shm_stream_<PID>_<SID>
    ▼
CTXFUSE Daemon (FUSE 文件系统进程)
    │ FUSE 回调: getattr/readdir/open/read/release
    │ 挂载点: /tmp/citrix_clipboard_<PID>/
    ▼
本地文件管理器 (Nautilus / cp / 任意应用)
```

##### 跨会话协调（gRPC）

```
Session A (文件源)          icasessionmgr            Session B (请求方)
      │                          │                          │
  用户复制文件                     │                          │
      ├──BroadcastClipInfo()────>│──NotifyClipPlaceInfo()──>│
      │<─────────RequestClipContent()──────────────────────│
      ├──RENDER_REQUEST──>VDA    │                          │
      │  VDA返回RENDER_REPLY     │                          │
      ├──NotifyClipContent()────────────────────────────>──│
```

通过 `current_requester_from_ica` 状态标记（NONE/LOCAL/OTHER_SESSION）实现数据路由：RENDER_REPLY 到达时，若标记为 OTHER_SESSION 则走 gRPC 转发，否则走本地 X 剪贴板。

#### 4. 核心实现细节

##### FUSE 虚拟文件系统

| 回调 | 触发时机 | 实现逻辑 |
|------|---------|---------|
| `my_init()` | 挂载时 | 设置 max_read=256KB，启动 FGD 监听线程 |
| `my_getattr()` | `ls`, `stat` | 从 FileGroupDescriptor 读取文件属性 → `struct stat` |
| `my_readdir()` | `ls` 目录 | 从 `g_files_children_map` 返回子项 |
| `my_open()` | 打开文件 | 通过 SHM 协商获取 StreamID → 存入 `fi->fh` |
| `my_read()` | 读文件内容 | SHM 请求 → wfica 从 VDA 拉数据 → 返回（256KB/次）|
| `my_release()` | 关闭文件 | SHM 发 CloseStream → 清理 sem/shm |

**关键设计决策**：
- `fi->direct_io = 1`：绕过内核页缓存，每次 read 都走 FUSE，避免缓存一致性问题。
- `fi->nonseekable = 1`：不支持 seek，流式读取。
- 每个打开的流有独立的 mutex（`stream_mutex_map[StreamID]`），防止同一文件的并发 read() 在 SHM 上竞争。

##### 共享内存数据传输

三组 SHM/Semaphore 配对：

| 用途 | SHM 名 | Semaphore 名 | 说明 |
|------|--------|-------------|------|
| 文件列表传递 | `/fuse_shm_fgd_<PID>` | `/fuse_semaphore_fgd_<PID>` | wfica→FUSE: 整个 FileGroupDescriptor |
| 流打开协商 | `/fuse_shm_open_stream_<PID>` | `req_<PID>` / `rep_<PID>` | 双向: 请求打开文件流，返回 StreamID |
| 数据传输 | `/fuse_shm_stream_<PID>_<SID>` | `req_<PID>_<SID>` / `rep_<PID>_<SID>` | 每个打开的文件流独立 SHM (1MB buffer) |

**零拷贝优势**：wfica 写入 SHM，FUSE daemon 直接从同一物理内存读取，没有内核态的 `copy_from_user/copy_to_user`。对比 Unix Socket 需要两次拷贝，管道还有 64KB 缓冲区限制。

##### 多包重组

大文件的 FGD 或数据可能超过单个 WFCLIP 包的 1020 字节限制，通过 `PACKETQUEUE` 结构体累积多个包后完整交付：

```c
typedef struct _PACKETQUEUE {
    BOOL   fInProgress;    // 正在接收多包
    IU8   *pClipData;      // 累积缓冲区
    ULONG  cbLeft;         // 剩余字节数
} PACKETQUEUE;
```

#### 5. 关键代码路径

| 文件 | 职责 |
|------|------|
| `base/clip/vdclip.c` | VDCLIP 驱动：消息路由、请求方追踪、多包重组 |
| `unix/clip/vdclip_handler.cpp` | 协议握手：版本协商（≥ WFSHELL_VERSION_5 启用 S2C）|
| `vdgdt/ctxfuse/fuse.cpp` | FUSE 回调：open/read/release/readdir/getattr |
| `vdgdt/inc/gdt_protocol.h` | GDT 协议命令定义（OPEN_STREAM/READ/CLOSE_STREAM）|
| `sessionapi/sessionapi.proto` | gRPC 服务定义（NotifyClipPlaceInfo/RequestClipContent）|
| `CMAPI/session_manager_lib/ica_session_manager.cpp` | Session Manager gRPC 服务端 |

#### 6. 设计亮点

- **FUSE vs 临时文件**：FUSE 的 read() 是 lazy 的，只有用户真正粘贴时才从远端拉数据。零存储开销，流式传输。对外表现为标准文件系统，任何文件管理器都能直接操作。
- **SHM vs Unix Socket**：共享内存实现零拷贝，信号量同步开销极低（原子操作 + futex）。文件数据量大时性能优势显著。
- **gRPC + UDS**：Unix Domain Socket 天然文件系统权限控制，不暴露网络端口，避免 TCP 协议栈开销。

---

### 项目三：Citrix Workspace App — HDX Direct v2 网络直连优化

**公司：Citrix (Cloud Software Group)　　角色：主力开发**
**技术栈：C, Golang, STUN (RFC 5389), UDP/TCP, pthread, CTX_LAYER 装饰器**

#### 1. 项目背景

Citrix 远程桌面的传统架构是 Client→Gateway(NetScaler)→VDA，Gateway 做 SSL 终止、认证、负载均衡和流量中转。但 Gateway 中转引入额外的 RTT 延迟（~50ms），对交互式桌面操作（鼠标/键盘/画面刷新）体验影响显著。

HDX Direct 功能使客户端在通过 Gateway 建立初始连接后，后台探测并建立到 VDA 的直连通道，然后热切换过去，将 RTT 从 ~80ms 降低到 ~10-15ms。V1 版本仅支持内网直连，V2 版本通过 STUN NAT 穿透支持外网直连。

#### 2. 技术方案

##### 2.1 CTX_LAYER 可插拔协议栈

```c
// abstsock.h — 装饰器模式
struct ctx_layer {
    struct ctx_layer *downstream;  // 指向下一层
    // send(), recv(), connect(), close() 函数指针
};

// 实际层叠: CGP → SSL → Proxy → TCP/UDP
// 各层独立插拔，支持灵活的网络拓扑适配
```

我使用 Golang 编写了 HDX Direct v2 的原型（Transport interface + downstream 组合），验证三路竞争、STUN 打洞、热切换等核心逻辑后，将方案移植到 C 代码中。Go 的 interface 对应 C 的函数指针表，goroutine + channel 对应 pthread + mutex + condition variable。

##### 2.2 三路并行连接竞争

```
三个 pthread 线程同时启动:

Thread 1: HDX Direct UDP (立即) → 直连 VDA 内网/外网 IP
Thread 2: Standard UDP    (立即) → 通过 Gateway UDP 中继
Thread 3: TCP via Gateway (延迟 500ms) → 给 UDP 优势窗口

Winner 机制:
  pthread_mutex_lock(&ParallelConnectMutex);
  if (winner == NULL) { winner = this; signal(complete); }
  else { close(my_socket); }  // 其他线程收尾
  pthread_mutex_unlock(&ParallelConnectMutex);
```

TCP 延迟 500ms（`CGPParallelConnectTCPDelay`）是因为 UDP 无三次握手，延迟更低，给 UDP 一个优势窗口。

##### 2.3 STUN NAT 穿透

```
Client (192.168.1.5:12345)
    → NAT 映射 → 203.0.113.5:40000
    → STUN Server 看到源地址 203.0.113.5:40000
    → 写进 XOR-MAPPED-ADDRESS 返回
    → Client 知道自己的公网地址
    → 通过 Gateway 信令交换地址 → UDP 打洞直连
```

实现细节：
- 并行查询 4 个 STUN 服务器（Azure/AWS/Cloudflare/Google），取最快响应。
- 指数退避重试：`{500, 1000, 2000, 4000, 8000, 16000, 32000, 32000}ms`。
- Transaction ID 使用 `/proc/sys/kernel/random/uuid` 生成（fallback 软件 RNG）。
- 成功后通过 `pthread_cond_signal()` 通知主线程。

##### 2.4 热切换（Switchover）

```c
SwitchToUDPConnection() {
    // 1. 验证新连接可用（发 ping 确认可达性）
    // 2. pthread_mutex_lock(&directConnectMutex)  // 写锁，短暂阻塞所有发送
    // 3. 原子替换传输层指针
    // 4. pthread_mutex_unlock()
    // 5. 关闭旧连接 + 发 CGP_CHANNEL_CLOSE 通知 Gateway 释放资源
}
// CGP 层序列号保证切换过程数据不丢
```

用户在使用过程中完全无感知。

##### 2.5 两阶段重连

| 阶段 | 超时 | 行为 |
|------|------|------|
| Session Reliability (SR) | 180s | 服务端缓存数据，客户端 UI 变暗 80%，触发三路竞争重连 |
| Auto Client Reconnect (ACR) | 245s | 使用 Cookie 重建连接，再次三路竞争 |
| 彻底失败 | — | Session lost, UI 关闭 |

##### 2.6 连接版本

```c
typedef enum {
    CGP_HDX_DIRECT_PRIMARY,    // V0: Gateway (初始/兜底)
    CGP_HDX_DIRECT_INTERNAL,   // V1: 内网直连 (Private IP)
    CGP_HDX_DIRECT_EXTERNAL    // V2: 外网直连 (STUN 打洞)
} CGP_HDX_DIRECT_CONNECTION_VERSION;
```

Gateway 在握手阶段下发 `HDX_DIRECT_DATA`（VDA FQDN、Private IP 链表、Public IP 链表），客户端据此决定直连策略。

#### 3. Golang 原型验证

我使用 Golang 编写了完整的 HDX Direct v2 原型，包含：
- `transport.go`：Transport interface（对应 CTX_LAYER），支持 TCP/UDP/Logging 装饰器层叠。
- `race.go`：三路并行竞争（goroutine + buffered channel），context.Cancel 取消其他尝试。
- `stun.go`：STUN Binding Request/Response 编解码，XOR-MAPPED-ADDRESS 解析，多服务器并行 + 指数退避。
- `cgp.go`：CGP 状态机（9 种状态），RWMutex 热切换，SR/ACR 两阶段重连。
- `gateway.go`：模拟 Citrix Gateway 的 TCP/UDP 中继和 HDX_DIRECT_DATA 下发。
- `vda.go`：模拟 VDA 的 TCP/UDP Echo 服务器。

原型同时包含 single-process demo（所有组件在一个进程）和 distributed 版本（gateway/vda/client 分进程部署），验证了各种网络拓扑下的行为。

#### 4. 关键指标

| 指标 | Gateway 模式 | HDX Direct V1 | HDX Direct V2 |
|------|-------------|---------------|---------------|
| RTT | ~80ms | ~10ms | ~15-25ms |
| 吞吐 | Gateway 瓶颈 | 直连带宽 | 直连带宽 |
| 可用场景 | 任意网络 | 内网 | 外网 (NAT 友好) |

#### 5. 概念对照表

| Go 概念 | C 项目代码 | 核心思想 |
|---------|-----------|---------|
| `Transport` interface | `CTX_LAYER` + 函数指针表 | 层叠可插拔 |
| goroutine + channel | pthread + mutex + cond_var | 并行竞争 |
| `context.WithTimeout` | `sem_timedwait` / 手动计时 | 超时控制 |
| `sync.RWMutex` | `pthread_rwlock_t` | 读写锁 (Switchover) |
| `defer conn.Close()` | 手动在每个 error path 调 close | 资源清理 |

---

### 项目四：Citrix Workspace App — Client-Session USB 设备重定向

**公司：Citrix (Cloud Software Group)　　角色：主力开发**
**技术栈：C/C++, Linux USB 子系统 (usbdevice_fs.h), ICA 虚拟通道, inotify, POSIX SHM, udev**

#### 1. 项目背景

USB 重定向是 Citrix 虚拟桌面的核心功能，允许用户将本地 USB 设备（U 盘、打印机、扫描仪、Bloomberg 终端等）透传到远程会话中使用。系统涉及 Linux 内核 USB 子系统交互、设备策略管理、ICA 虚拟通道传输以及客户端侧设备访问与远程透传等多个层面。

#### 2. 四层架构

```
┌──────────────────────────────────┐
│  Desktop Viewer (GUI)            │
│  设备列表、手动连接/断开           │
└──────────────┬───────────────────┘
               │ 用户操作
┌──────────────▼───────────────────┐
│  Virtual Driver (VDGUSB)         │
│  虚拟通道 "CTXGUSB"              │
│  URB 消息序列化/反序列化           │
└──────────────┬───────────────────┘
               │ ICA Virtual Channel
┌──────────────▼───────────────────┐
│  Redirector (核心引擎)            │
│  ├── policy.cpp  (策略引擎)       │
│  ├── redirector_state.h (状态机)  │
│  ├── redirector_fd.c (内核交互)   │
│  └── redirector_shm.h (跨会话)   │
└──────────────┬───────────────────┘
               │ ioctl / inotify
┌──────────────▼───────────────────┐
│  Linux Kernel USB Subsystem      │
│  /dev/bus/usb/[bus]/[device]     │
│  usbdevice_fs.h (URB 操作)       │
└──────────────────────────────────┘
```

#### 3. 核心实现

##### 3.1 设备发现 — inotify 热插拔检测

- 使用 Linux inotify API 监听 `/dev/bus/usb` 目录的 `IN_CREATE` 事件，实现热插拔实时检测。
- 同时周期性扫描已有设备确保不遗漏。
- 发现设备后读取设备文件获取 USB 描述符（VID/PID/Class/SubClass/Protocol），经策略引擎过滤后决定是否重定向。
- 配合 udev 规则（`ica-usb.rules`），`ctx_usb_isactive` 检查 ctxusb 进程是否活跃，阻止系统自动处理被重定向的 USB 设备。

##### 3.2 设备生命周期状态机（10 种状态）

```
UNSET → 策略检查 → ANNOUNCED (通知服务器)
                  → VETOED   (策略拒绝)

ANNOUNCED → ACCEPTED (服务器接受, 开始使用)
          → REJECTED (服务器拒绝)

ACCEPTED → STOPPED    (已停止)
         → SUSPENDED  (用户暂停)
         → GONE       (已拔出)

任意阶段 → ERROR
```

##### 3.3 策略引擎 — 类防火墙规则链

```
规则类型:
  ALLOW:   vid=1234 pid=5678           # 允许特定设备
  DENY:    class=01                     # 禁止音频设备类
  CONNECT: vid=1188 pid=A101           # 自动重定向 (如 Bloomberg)
  DENY:    class=03 subclass=01 prot=01 # 禁止 HID 启动键盘

匹配字段: VID, PID, Release, Class, SubClass, Protocol, Split, Interface
规则来源: DDC 服务器下发 > 本地 usb.conf (DDC 优先)
匹配逻辑: 自上而下，首次匹配生效
```

支持两个版本：v1 设备级匹配，v2 增加接口级匹配支持复合设备拆分。

##### 3.4 复合设备拆分（Interface Split）

以 Bloomberg 终端为例，一个物理设备包含多个 USB 接口：
- 接口 0: 键盘 → `DENY split=01 intf=00`（保留给本地）
- 接口 1: HID → `CONNECT split=01 intf=01`（重定向到会话）
- 接口 2: 生物识别 → `CONNECT split=01 intf=02`（重定向到会话）

通过 `selected_interfaces` 字段实现接口级别的细粒度控制，避免整体重定向导致本地键盘丢失。

##### 3.5 URB 转发 — ICA 虚拟通道

USB 数据通过 CTXGUSB 虚拟通道传输，支持四种 USB 传输类型：

| 传输类型 | 命令码 | 典型设备 | 特点 |
|---------|--------|---------|------|
| Control | 0x24 | 所有设备 | 设备配置/查询，可靠传输 |
| Bulk | 0x25 | U 盘/打印机 | 大量数据，可靠但无时间保证 |
| Interrupt | 0x26 | 键盘/鼠标 | 小量数据，低延迟有保证 |
| Isochronous | 0x27 | 音视频设备 | 实时流，有时间保证但不重传 |

##### 3.6 客户端本地设备访问与远程透传

```
Client USB Device                CWA / CTXGUSB                 Remote Session
  本地 USB 请求/响应     →        ICA 虚拟通道透传        →        远程会话消费设备数据
  设备描述符/URB 访问     ←        控制命令/数据返回        ←        会话侧响应
```

客户端直接访问本地 USB 设备节点并处理 USB 请求/响应，CWA 再通过 CTXGUSB 虚拟通道将控制命令和数据透传给远程桌面，使会话侧能够使用该设备。

##### 3.7 跨会话设备锁定

使用 POSIX 共享内存（SHM）记录 `(bus_num, dev_num) → session` 映射：
- `create_in_shm(bus, dev)`：注册设备占用
- `read_from_shm(bus, dev)`：检查设备是否被其他会话使用
- `remove_from_shm(bus, dev)`：释放设备占用

防止同一 USB 设备被多个远程会话同时竞争。

##### 3.8 多线程架构

**客户端**：
- 主线程：select 循环处理 ICA socket 和消息 socket
- 每设备：独立文件描述符进行 USB 操作

**VDA 侧**：
- USBVirtualChannel：Read 线程（接收 ICA 包）+ Write 线程（发送响应）
- 会话侧处理模块按设备维度处理透传后的控制命令与数据响应
- 线程间通过 `CtxUSBMsgQueue`（`std::queue` + mutex + condition_variable）通信

#### 4. 关键代码路径

| 文件 | 职责 |
|------|------|
| `ctxusb/redirector.c` | 主重定向引擎，设备发现（inotify）与管理 |
| `ctxusb/policy.cpp` | 策略规则解析与匹配（v1/v2）|
| `ctxusb/redirector_state.h` | 10 状态设备生命周期状态机 |
| `ctxusb/redirector_fd.c` | 内核 USB 文件描述符操作（ioctl/URB）|
| `ctxusb/redirector_shm.h` | 共享内存跨会话设备锁定 |
| `ctxusb/usb_proto.h` | 协议命令与能力定义 |
| `vdgusb/vdgusb.c` | 虚拟通道驱动入口 |
| `vdgusb/vc_msg.c` | 虚拟通道消息编解码 |
| `library/linux_descriptors.c` | USB 描述符解析 |

#### 5. 设计亮点

- **状态机模式**：10 种状态覆盖设备完整生命周期，每次状态转换都有明确的前置条件和副作用，便于调试和追踪。
- **策略引擎**：类防火墙规则链设计，支持设备级和接口级匹配，DDC 服务器可远程下发策略，实现企业级安全管控。
- **本地访问 + 通道透传**：客户端直接访问 Linux USB 子系统，避免额外的远程挂载协议层；同时通过 ICA 虚拟通道透传控制命令与数据，兼顾兼容性与传输效率。
- **接口级拆分**：解决复合设备（如 Bloomberg 终端）的部分重定向需求，在不影响本地键盘使用的前提下重定向 HID 和生物识别接口
