# 面试口述稿 02：基于 agentV3 的生产化追问（开发岗）

用途：针对你已经做过的 `agentV3`（coding/page 修改方向），把“会做”转成“会讲”，重点覆盖面试高频追问。

---

## 1. 你这个 Agent 的核心链路是什么？

标准回答：
- 我把链路拆成 6 步：`createRun -> runAgent -> buildSystemPrompt -> agentLoop -> tool execution -> event stream`。
- API 层只负责收参和 run 管理，执行逻辑在 `runService + loop + tools`。
- 这样做的好处是边界清晰，便于后续替换 provider、扩展工具和做可观测性。

项目证据：
- `external/component-app-webservice/services/agentV3/agent_v3.md`
- `external/component-app-webservice/apis/agentRunsApi.js`
- `external/component-app-webservice/services/agentV3/runService.js`

---

## 2. 你是怎么防止 Agent 无限循环或胡乱调用工具的？

标准回答：
- 我做了双保险：`MAX_STEPS` 限制总步数，`doom loop` 检测同工具同输入的重复调用。
- 命中后返回 `tool_result is_error=true`，并通过事件流打点，避免系统卡死。

项目证据：
- `external/component-app-webservice/services/agentV3/loop.js`

可补强点：
- 增加“跨工具循环模式”检测，而不仅是同工具同输入。

---

## 3. Tool Calling 的契约和失败处理你怎么设计？

标准回答：
- 每个工具声明 `name/description/input_schema`，执行时统一回 `tool_result`。
- 失败不直接吞掉，会透出 `tool_failed` 事件并回灌模型，让模型决定重试、换路径或收敛回答。
- 关键写操作工具（如 `write_page`）前后都要做结构校验，防止脏 patch。

项目证据：
- `external/component-app-webservice/services/agentV3/tools/index.js`
- `external/component-app-webservice/services/agentV3/tools/writePage.js`
- `external/component-app-webservice/services/agentV3/validators.js`

---

## 4. 你的状态管理和记忆是怎么做的？

标准回答：
- 当前 run 状态是内存态，适合快速迭代和调试。
- 事件使用 seq 递增，支持按 `sinceSeq` 增量拉取。
- 这在单实例可用，但在生产多实例场景要升级成持久化 run store + durable queue。

项目证据：
- `external/component-app-webservice/services/agentV3/runService.js`
- `external/component-app-webservice/apis/agentRunsApi.js`

可补强点：
- 引入 Redis/Postgres 持久化，支持重启恢复和横向扩容。

---

## 5. 你如何保证页面 patch 不会破坏 schema？

标准回答：
- 我把 page 校验分成 full definition 和 patch 两类。
- patch 场景允许 partial，但保留底线约束：`children` 必须是数组、元素不能是 `null`、节点必须合法。
- 这个策略在“灵活修改”和“结构安全”之间做了平衡。

项目证据：
- `external/component-app-webservice/services/agentV3/validators.js`

---

## 6. 可观测性怎么做？你如何定位一次失败？

标准回答：
- 目前有 run 级事件流：`run.started/run.token/run.tool_*/run.completed/run.failed`。
- 排障时先看 run 生命周期，再看具体 tool 事件和报错 payload。
- 下一步是把事件映射到统一指标（成功率、工具错误率、P95 时延）并接入 tracing。

项目证据：
- `external/component-app-webservice/services/agentV3/runService.js`

可补强点：
- 增加 `trace_id` 贯穿 API -> run -> tool 调用。

---

## 7. 评估体系（Evals）你怎么回答？

标准回答：
- 线上质量不能靠感觉，要有离线评估集 + 线上指标双轨。
- 离线集覆盖：新增页面、复杂 patch、异常输入、工具失败。
- 线上看四个指标：任务成功率、P95 时延、单任务成本、人工接管率。

当前状态：
- `agentV3` 已有运行链路与事件基础，但系统化 eval 集还需要补。

面试表达建议：
- 不要说“还没做”；说“已有事件基础，下一步 2 周内补离线集与看板”。

---

## 8. 安全和权限边界你怎么设计？

标准回答：
- 工具层要做最小权限，不让模型直接拥有无限写入能力。
- 高风险写操作可加 policy check 和 HITL 审批。
- 密钥必须走环境变量和密钥管理系统，禁止硬编码到仓库。

项目提醒：
- 仓库中出现过敏感信息硬编码风险，需要立即做 key 轮换与清理历史提交。

---

## 9. 如果线上出故障，你的降级策略是什么？

标准回答：
- 第一层：限制步骤、超时返回、快速失败。
- 第二层：关闭高风险工具，仅保留只读工具。
- 第三层：切换到模板化 workflow 或人工接管。
- 目标不是“永不失败”，而是“失败可控、可恢复”。

---

## 10. 为什么你现在不先上多 Agent？

标准回答：
- 多 Agent 会放大通信和一致性成本。
- 在单 Agent 的稳定性、可观测性、评估体系没打牢前，上多 Agent 只会增加排障复杂度。
- 我的路线是先把单 Agent 生产闭环打透，再按场景拆角色。

---

## 快速复习卡（30 秒）

- 我的亮点：链路清晰、工具化落地、事件流可观测、基础防循环。
- 我的短板：eval 体系、持久化恢复、统一 tracing、权限治理闭环。
- 我的计划：先补评估与可观测，再补持久化与安全策略，最后再考虑多 Agent。
