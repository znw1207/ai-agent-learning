# agentV3 现状、能力边界与短板

目标：明确这个项目现在已经具备什么，距离高质量面试项目还差什么。

---

## 1. 当前已经具备的能力

### 1.1 工具调用主链路

当前已经有清晰的 Agent 主链路：

- API 收参
- createRun
- runAgent
- buildSystemPrompt
- agentLoop
- tool use / tool result
- event stream

这说明系统已经具备最基本的 agent harness 雏形。

### 1.2 页面读写工具闭环

当前工具集合已经覆盖核心动作：

- `read_app`
- `read_page`
- `write_page`
- `create_page`

这使它不是“只能回答问题”的 Agent，而是能实际改动目标对象的系统。

### 1.3 结构校验层

当前通过 validator 对 page/schema/children 做了显式校验。

这说明项目已经在做：

- 写入约束
- 结构治理
- 失败保护

这是一个明显高于纯 prompt 工程的地方。

### 1.4 事件流和运行时状态

当前具备 run 生命周期与事件流记录机制。

这为后续补可观测性、失败分类、恢复机制提供了入口。

---

## 2. 当前最明显的短板

### 2.1 Eval 体系不足

这是最大短板。

当前缺少：

- 评估数据集
- 成功率基线
- case 分类统计
- 优化前后对比

没有 Eval，就无法证明任何 prompt、tool、RAG、reflection 的优化是有效的。

### 2.2 可观测性仍偏“调试级”

当前有事件流，但还不等于完整 observability。

缺的主要是：

- trace_id 贯穿
- terminate_reason
- step_count
- tool latency
- 聚合指标面板

### 2.3 持久化与恢复能力不足

当前 run 的状态管理仍偏内存态。

这会带来：

- 服务重启丢 run
- 断流恢复不完整
- 多实例扩展困难

### 2.4 权限与高风险写入治理不足

`write_page` 这类工具是有副作用的。

当前项目更像“能写”，但还没有完全演进到“受控地写”：

- 权限分级
- dry-run
- policy check
- 审批节点

### 2.5 知识层偏静态 prompt

系统目前主要依赖静态 prompt 文件和较薄的 app summary。

这会导致：

- 规则越多，prompt 越重
- 示例越多，维护越难
- 业务知识和页面案例无法按需加载

这也是 RAG 最值得进入的地方。

---

## 3. 当前最适合在面试中怎么讲

比较稳妥的说法是：

- 这是一个已经完成工具调用主链路、事件流、结构校验的页面 Agent 原型系统。
- 它最强的部分是页面读写闭环，而不是花哨的多 Agent 架构。
- 它下一步最值得投入的是 Eval、Observability、Recovery、Security 和按需知识加载。

这比空泛说“我做了一个前端 AI Agent 平台”更可信。

---

## 4. 作为面试官，我会认为它值不值得聊

答案是：值得。

原因：

- 它是有副作用的真实 Agent，不是单轮问答
- 有真实的 source of truth 和工具层
- 有 prompt、tool、validator 的协同设计
- 有继续深化为高质量项目的空间

但能不能打动中高级面试官，取决于你后续能不能补上：

- 评估证据
- 失败复盘
- 恢复机制
- 权限治理
- 知识增强路径
