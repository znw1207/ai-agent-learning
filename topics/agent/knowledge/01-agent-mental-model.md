# Agent 核心心智模型（Phase 1）

本文是 `ai-agent-learning` 的第一份基础文档，目标是建立 Agent 开发工程师视角下的统一心智模型。

## 1. 先回答一个问题：Agent 到底是什么

从工程角度看，Agent 不是“会聊天的模型”，而是一个可执行任务的系统。

更准确的定义（参考 `agentic-design-patterns` 术语表）：
- Agent 是“能够感知环境并为实现目标采取自主行动的系统”。

最小能力闭环：
- 有目标（Goal）
- 能感知上下文（Context）
- 能做决策（Policy / Planning）
- 能调用外部能力（Tools）
- 能基于反馈迭代（Feedback Loop）

一句话版本（定义与实现分开）：
- Agent = `Goal + State + Policy + Action Interface + Feedback + Governance`

注意区分：
- Agent 是系统能力定义，不等于某一种固定架构。
- ReAct 是决策/执行范式之一（Thought -> Action -> Observation 循环）。
- Workflow（如 LangGraph）是另一类显式编排范式。
- Multi-Agent 是组织形态，不是 Agent 定义本身。

## 2. Agent 的 6 层系统结构（开发岗版）

建议以后都按这 6 层去分析任何 Agent 项目：

1. 入口层（Interface Layer）
- 对接 Web/API/消息队列
- 处理用户请求、鉴权、限流、幂等键

2. 编排层（Orchestration Layer）
- 负责任务拆解、流程控制、状态机推进
- 单 Agent ReAct 或多 Agent 协作都属于这一层

3. 模型层（Model Layer）
- 负责模型路由、提示词注入、参数控制
- 典型问题：质量/速度/成本 trade-off

4. 工具层（Tool Layer）
- 对接内部 API、数据库、搜索、浏览器自动化等
- 关键是工具契约、超时、重试、错误处理

5. 状态与记忆层（State & Memory Layer）
- 短期状态：本次任务上下文
- 长期记忆：跨会话偏好、历史事实、知识召回

6. 可观测与治理层（Observability & Governance）
- trace/log/metrics/alert
- 权限控制、审计、敏感信息保护

## 3. 一次典型 Agent 请求的数据流

```text
User Request
  -> 入参校验 / 鉴权
  -> 编排层生成计划（Plan）
  -> 决策是否调用工具（Tool Decision）
  -> 调用工具并拿回 Observation
  -> 更新状态/记忆
  -> 继续推理或结束
  -> 输出结果 + 执行轨迹
```

开发岗必须重点关注：
- 失败路径怎么走（超时、空返回、权限不足）
- 状态是否可恢复（checkpoint / retry）
- 请求是否可追踪（trace_id）

## 4. ReAct、Workflow、Multi-Agent 的关系

1. ReAct
- 思路：Thought -> Action -> Observation 循环
- 适合动态任务，灵活但不易约束

2. Workflow（如 LangGraph 风格）
- 思路：显式状态机 + 节点图
- 适合复杂流程，可观测性和稳定性更好

3. Multi-Agent
- 思路：角色分工 + 协同协议
- 优点：专业化与并行
- 风险：冲突、一致性、成本上涨

选择建议：
- 先单 Agent 闭环，再拆多 Agent
- 先能跑通，再追求角色精细化

## 5. 面试中最常见的 5 个追问（开发岗）

1. 为什么需要 Agent，而不是普通工作流？
- 回答方向：不确定性任务下，Agent 具备动态决策与工具选择能力。

2. 你怎么保证工具调用稳定？
- 回答方向：超时、重试、熔断、幂等、降级、错误分类。

3. 状态和记忆怎么设计？
- 回答方向：短期状态放执行链路，长期记忆做检索式存储并控制写入质量。

4. 多 Agent 如何避免冲突？
- 回答方向：Supervisor 仲裁、任务边界、冲突优先级、回退策略。

5. 怎么评估 Agent 真有价值？
- 回答方向：任务成功率、端到端时延、单任务成本、人工替代率、用户满意度。

## 6. 本阶段你要达到的能力标准

- 能用 3 分钟讲清 Agent 的系统分层。
- 能画出一次请求的数据流和失败路径。
- 能说明 ReAct 与 Workflow 的取舍依据。
- 能给出至少 3 个稳定性措施并解释为什么。

## 7. 下一步关联实践

建议与以下示例联动阅读和运行：
- `ai-agent-learning/agent-demo/03_command_api.py`
- `ai-agent-learning/agent-demo/04_interrupt_human_in_loop.py`
- `ai-agent-learning/agent-demo/08_multi_agent_system.py`

完成后补一条复盘：
- 我最容易混淆的概念是什么？
- 我的系统图是否覆盖异常链路？
- 我的指标是否可量化？
