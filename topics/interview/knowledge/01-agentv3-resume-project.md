# agentV3 简历项目稿

目标：把 `agentV3` 提炼成可以写进简历、也能在面试中讲得清楚的项目材料。

---

## 1. 项目名称（简历版）

推荐写法：

- 面向页面生成与修改的工具调用型 Agent 系统（agentV3）
- 基于工具调用和事件流的前端页面构建 Agent（agentV3）

如果偏英文简历，可写：

- Built a tool-using page generation agent for App Render V2
- Designed an event-driven agent system for frontend page creation and modification

---

## 2. 项目背景

这个项目服务于页面创建和修改场景。用户通过自然语言提出需求，Agent 读取当前 app/page 状态，再通过工具调用对页面 JSON 结构进行创建或 patch 更新。

项目的本质不是聊天，而是：

- 基于真实上下文理解用户意图
- 调用受控工具读写页面
- 通过结构校验和事件流提升稳定性和可观测性

---

## 3. 你可以写在简历里的职责

建议写成“做了什么 + 为什么重要”。

- 设计并实现 `agentV3` 的工具调用主链路，支持已有页面修改与新页面创建。
- 搭建 run 生命周期管理与事件流机制，覆盖 `run.started`、token、tool 生命周期、完成/失败收尾。
- 设计页面 patch 校验规则，约束 schema/children 结构，降低模型写入脏数据的风险。
- 优化 prompt 与工具协作方式，使 Agent 优先读取真实页面结构，再最小化写入 patch。
- 迭代页面读写语义，支持更稳定的 `children` 更新和对象 deep merge。

---

## 4. 项目亮点（面试可展开）

### 4.1 真实工具调用场景

这不是问答型 Agent，而是有副作用的系统：

- 读 app
- 读 page
- 写 page
- 创建 page

这会自然引出面试里的几个重点：

- 工具契约
- 写入安全
- 失败恢复
- 权限边界

### 4.2 有明确的运行时链路

当前链路相对清晰：

```text
API -> createRun -> runAgent -> buildSystemPrompt -> agentLoop -> tool call -> event stream
```

这意味着项目具备继续补：

- Eval
- Observability
- Recovery
- Governance

的良好基础。

### 4.3 有结构校验，不是纯 prompt 硬赌

`agentV3` 并不只是把 prompt 写长，而是通过 validator 把页面 patch 的关键约束显式化。

这点在面试里很重要，因为它体现的是“工程治理”而不是“靠模型运气”。

---

## 5. 简历 bullet 示例

你可以选 3-4 条，不要全抄。

- 设计并实现一个面向前端页面创建/修改的工具调用型 Agent，支持基于自然语言对 App Render V2 页面 JSON 进行结构化读写。
- 搭建 Agent 运行时链路与事件流，覆盖 token 输出、tool 生命周期、run 状态切换等关键观测点，提升调试与排障效率。
- 设计页面 patch 校验与写入语义，支持对象 deep merge 和受控数组更新，降低模型生成非法 schema 的风险。
- 优化 prompt + tool 协作方式，使 Agent 优先读取真实页面结构作为 source of truth，再执行最小 patch 更新。

---

## 6. 面试时怎么定义这个项目的级别

当前最准确的说法是：

- 它已经不是 demo，而是一个真实业务约束下的 Agent 原型系统。
- 它的强项在于工具调用、结构化写入、事件流与页面构建场景结合。
- 它的提升空间主要在 Eval、Observability、Recovery、Security、RAG 知识增强等工程层面。

这个表述既不虚高，也不会把项目说轻。

---

## 7. 不要在简历里过度承诺的点

当前不建议直接写成：

- 生产级多 Agent 平台
- 完整自治前端开发 Agent
- 大规模可恢复分布式 Agent 系统

因为这些描述会把面试官的追问拉到你还没完全实现的层面。

更稳妥的表达是：

- 工具调用型 Agent
- 事件驱动 / 可观测的 Agent 原型
- 面向页面生成与修改的 Agent 系统
