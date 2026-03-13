# agentV3 深化路线图（面向真实项目）

目标：把 `component-app-webservice` 里的 `agentV3` 从“可运行的工具调用型 agent”逐步推进到“可评估、可观测、可恢复、可治理”的工程化项目。

这份路线图的重点不是增加功能数量，而是补齐工程深度。判断标准不是“会不会说”，而是“有没有做出来并留下证据”。

---

## 1. 当前项目处于什么阶段

基于当前实现，`agentV3` 已经具备一个合格的第一版骨架：

- 有清晰的执行链路：`createRun -> runAgent -> agentLoop -> tool execution -> event stream`
- 有基本的 ReAct 式循环与步数限制
- 有工具层与结构校验层
- 有 run 生命周期与流式事件

这说明项目已经不是玩具 demo，而是一个真实业务约束下的 Agent 原型。

但它距离“高质量面试项目”还差 4 层：

1. 评估（Eval）
2. 可观测性（Observability）
3. 持久化与恢复（Durability / Recovery）
4. 安全与治理（Security / Governance）

这 4 层补出来，项目的叙事会明显升级：
- 从“我做了一个 Agent”
- 变成“我做了一个生产化 Agent 系统，并知道如何验证、定位、恢复和治理它”

---

## 2. 优先级原则

优化顺序必须严格控制，不然很容易陷入“功能越来越多，但深度没有上来”。

建议顺序：

1. Eval
2. Observability
3. Durability / Recovery
4. Security / Governance
5. Reflection / Planner-Executor / Multi-Agent

原因：

- 没有 Eval，就无法证明任何优化有效。
- 没有 Observability，就无法知道系统为什么失败。
- 没有 Durability，就很难回答生产环境恢复问题。
- 没有 Security，就无法支撑高风险写操作。
- 更高级的 Agent 结构应该建立在前四层之上，而不是先堆复杂度。

---

## 3. 六周深化计划

## Week 1：补 Eval 基线

目标：让 `agentV3` 第一次具备“可度量改进”的能力。

要做的事：

- 建一个最小评估集，建议 30-50 条样本。
- 按场景分桶：
  - 新建页面
  - 修改已有页面
  - 多步 patch
  - 空 app 场景
  - 非法 patch
  - 工具失败
- 为每条样本定义：
  - 输入
  - 预期页面变化
  - 最低通过标准
  - 失败类型

建议产出：

- `topics/agent/examples/agentv3-eval-dataset/`
- `topics/agent/knowledge/03-agent-eval-basics.md`

你要留下的证据：

- 一次基线跑分结果
- 按 case 类型的通过率统计
- 失败样本列表

验收标准：

- 能说出当前成功率是多少
- 能说出失败主要集中在哪三类 case

---

## Week 2：补观测与失败分类

目标：把“事件流”提升为“诊断系统”。

要做的事：

- 为每个 run 补 `trace_id`
- 补 run 级指标：
  - step_count
  - terminate_reason
  - total_tool_calls
  - total_tool_failures
- 补 tool 级指标：
  - tool_name
  - latency_ms
  - status
- 建失败分类：
  - 模型错误
  - 工具错误
  - 校验错误
  - 编排错误
  - 数据问题

建议落点：

- `external/component-app-webservice/services/agentV3/runService.js`
- `external/component-app-webservice/services/agentV3/loop.js`

建议产出：

- `topics/system-design/knowledge/01-agentv3-observability.md`

你要留下的证据：

- 一次失败 run 的完整定位路径
- 一张简单的指标表或日志样例

验收标准：

- 能回答“一次失败是怎么定位到根因的”

---

## Week 3：补持久化与恢复

目标：解决“重启丢状态”和“断线不可恢复”的问题。

要做的事：

- 将 run 元数据和 events 从内存态迁移到持久化存储
- 明确 run 状态机
- 支持 `resume` / `reconnect`
- 设计幂等策略：
  - 重试请求不重复创建 run
  - 高风险写入避免重复执行

建议优先实现：

- 先落 Redis 或 Postgres，选一个即可
- 不追求分布式最优，先追求重启后可恢复

建议产出：

- `topics/system-design/knowledge/02-run-durability-and-recovery.md`

你要留下的证据：

- 服务重启后的恢复演示
- 断流后基于 `sinceSeq` 的重新拉流演示

验收标准：

- 能回答“服务重启后 run 怎么恢复”

---

## Week 4：补安全与治理

目标：把“能写页面”变成“受控地写页面”。

要做的事：

- 对工具做权限分级：
  - 只读工具
  - 低风险写工具
  - 高风险写工具
- 为写操作增加 policy check 或 dry-run 模式
- 限制 patch 作用范围
- 清理密钥管理问题，改为环境变量或密钥服务

建议落点：

- `external/component-app-webservice/services/agentV3/tools/`
- `external/component-app-webservice/config.js`

建议产出：

- `topics/system-design/knowledge/03-agent-security-and-governance.md`

你要留下的证据：

- 一次危险写入被拦截的案例
- 一次 dry-run 返回建议 patch 的案例

验收标准：

- 能回答“模型为什么不能直接拥有无限写权限”

---

## Week 5：补反思或 Planner-Executor

目标：在前四层稳定后，再提升推理质量。

二选一即可：

1. Reflection
- 让模型在写页面前后进行一次自检

2. Planner-Executor
- 先生成编辑计划，再逐步执行工具

注意：

- 这一步必须用 Week 1 的 Eval 数据集验证收益
- 不允许只靠主观感觉说“更好了”

建议产出：

- `topics/agent/knowledge/04-reflection-vs-planner-executor.md`

你要留下的证据：

- 优化前后通过率对比
- 成本与时延变化

验收标准：

- 能回答“为什么选择 Reflection 而不是直接上 Multi-Agent”

---

## Week 6：做一次项目化复盘

目标：把工程实践沉淀成高质量项目材料。

要做的事：

- 记录一次完整迭代：
  - 发现问题
  - 建评估集
  - 做优化
  - 指标变化
  - 剩余问题
- 输出一次故障复盘：
  - 问题现象
  - 定位路径
  - 根因
  - 修复方案
  - 如何避免复发

建议产出：

- `topics/interview/knowledge/01-agentv3-project-story.md`
- `topics/interview/knowledge/02-agentv3-postmortem.md`

你要留下的证据：

- 一次完整的项目故事线
- 一次故障案例闭环

验收标准：

- 能在 5-8 分钟内讲清楚项目背景、架构、问题、优化、结果、复盘

---

## 4. 每周都要沉淀什么

每周至少留下三类证据：

1. 文档
- 你做了什么
- 为什么这样做
- 还缺什么

2. 数据
- 成功率
- 时延
- 成本
- 错误率

3. 案例
- 一个成功案例
- 一个失败案例

这三类材料比任何“面试稿”都更有价值，因为它们是你后续表达的证据源。

---

## 5. 面试价值最高的优化项

如果时间有限，优先做这 5 件事：

1. Eval 数据集
2. terminate_reason + step_count + tool latency
3. run 持久化
4. 工具权限分级
5. Reflection 或 Planner-Executor 的一次收益验证

把这 5 件事做完，这个项目就已经能体现出明显的工程深度。

---

## 6. 判断自己有没有学深的标准

如果你能明确回答下面这些问题，说明这个项目开始有“深度”了：

- 我如何证明系统比上周更好？
- 我如何知道一次失败是模型问题、工具问题还是编排问题？
- 我如何在服务重启后恢复 run？
- 我如何限制模型的写权限？
- 我为什么在这个阶段不先做多 Agent？

如果这些问题还回答不实，就说明应该继续优先补工程层，而不是继续加新花样。