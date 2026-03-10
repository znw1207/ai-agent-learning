# AI Agent Learning Roadmap

目标：从“会用框架”进阶到“能面试、能设计、能落地”的 Agent 开发工程师。

定位：本路线以工程实践为主，不走算法深挖路线。仓库组织按知识点（topic-first），学习节奏按阶段推进。每个阶段都包含：
- 文档学习（概念与方法）
- 可执行示例（代码验证）
- 问答训练（用于面试表达与方案阐述）

## 0. 学习规则（先统一）

- 每个主题遵循 3 步：先读文档 -> 跑代码 -> 写复盘。
- 每个主题至少产出 1 份笔记和 1 个可运行示例。
- 每周至少 1 次“面试口述练习”（录音或文字都可）。
- 不追求一次学全，优先建立系统心智模型（mental model）。

## 1. 总体阶段图

Phase 1: Agent 基础与工作流（2 周）
- 重点：ReAct、Tool Calling、状态流转、单 Agent 到多 Agent 的最小闭环

Phase 2: RAG 工程化（2 周）
- 重点：端到端 pipeline、检索与生成评估、性能与成本优化

Phase 3: 系统设计与稳定性（2 周）
- 重点：高并发、多租户、可观测性、容错、降级

Phase 4: 面试化表达与项目化沉淀（2 周）
- 重点：系统设计题模板、项目故事线、指标化表达

说明：8 周是建议节奏，可按你的工作节奏拉长到 10-12 周。

## 2. 分阶段执行计划（按知识点产出）

## Phase 1（第 1-2 周）：Agent 基础与工作流

学习目标：
- 解释清楚 Agent 是什么、核心模块是什么、为什么需要工作流引擎。
- 能实现一个最小可运行 Agent：任务分解 -> 工具调用 -> 结果汇总。

参考资料：
- `AgentGuide/docs/04-interview/03-agent-questions.md`
- `ai-agent-learning/agent-demo/*.py`（优先 01-08）

代码实践：
- 跑通 `agent-demo` 的 01-08 示例。
- 新增一个自己的最小 ReAct 示例（可先不接真实外部 API）。
- 新增一个最小多 Agent 协作示例（1 个 supervisor + 2 个 specialist）。

文档产出：
- `topics/agent/knowledge/01-agent-mental-model.md`
- `topics/agent/knowledge/02-workflow-patterns.md`
- `topics/agent/knowledge/03-react-vs-workflow.md`

示例与问答产出：
- `topics/agent/examples/`
- `topics/agent/qa/`

验收标准：
- 能回答“ReAct 和 workflow 的区别与适用场景”。
- 能现场讲清楚一次工具调用失败后的重试/降级策略。

## Phase 2（第 3-4 周）：RAG 工程化

学习目标：
- 掌握端到端 RAG pipeline：数据处理、索引、检索、重排、生成、缓存。
- 会做工程权衡：准确率、延迟、成本三者平衡。

参考资料：
- `AgentGuide/docs/04-interview/02-rag-questions.md`
- 可按需引入外部参考（如 RAG 框架文档、向量数据库最佳实践）

代码实践：
- 做一个最小 RAG demo：本地文档 -> 切块 -> 向量检索 -> 回答生成。
- 加入 1 个优化点（例如：semantic cache 或混合检索）。
- 记录优化前后指标（P95/P99、成本、命中率）。

文档产出：
- `topics/rag/knowledge/01-rag-pipeline.md`
- `topics/rag/knowledge/02-rag-evaluation.md`
- `topics/rag/knowledge/03-rag-optimization-notes.md`

示例与问答产出：
- `topics/rag/examples/`
- `topics/rag/qa/`

验收标准：
- 能讲出“RAG 和传统搜索系统”的差异与选型依据。
- 能展示至少一项可量化优化结果。

## Phase 3（第 5-6 周）：系统设计与稳定性

学习目标：
- 能设计高并发 Agent/RAG 服务架构。
- 能明确稳定性机制：限流、熔断、超时、重试、幂等、降级。

参考资料：
- `AgentGuide/docs/04-interview/06-development-specialized.md`

代码实践：
- 基于已有 demo 增加：
- 统一请求入口（gateway 思路）
- 简单任务队列/并发控制
- 结构化日志 + 基础指标输出
- 错误分类与重试策略

文档产出：
- `topics/system-design/knowledge/01-system-design-blueprint.md`
- `topics/system-design/knowledge/02-reliability-playbook.md`
- `topics/system-design/knowledge/03-observability-checklist.md`

示例与问答产出：
- `topics/system-design/examples/`
- `topics/system-design/qa/`

验收标准：
- 能画出系统组件图并解释每层职责。
- 能回答“QPS 提升 10 倍如何扩展”的路线图。

## Phase 4（第 7-8 周）：面试表达与项目化沉淀

学习目标：
- 把技术经历表达成“可面试、可量化、可复盘”的项目故事。
- 形成自己的题库答案模板和故障案例库。

参考资料：
- `AgentGuide/docs/04-interview/07-career-transition.md`
- `AgentGuide/docs/04-interview/12-company-interview-cases.md`

代码实践：
- 整理一个端到端项目（可从 Phase 2/3 的 demo 升级）。
- 做 2 次“故障演练”：工具超时、检索质量下降，并记录修复过程。

文档产出：
- `topics/interview/knowledge/01-project-story-template.md`
- `topics/interview/knowledge/02-interview-qa-bank.md`
- `topics/interview/knowledge/03-failure-postmortems.md`

示例与问答产出：
- `topics/interview/examples/`
- `topics/interview/qa/`

验收标准：
- 能在 5-8 分钟讲完一个完整项目（背景-方案-优化-结果-复盘）。
- 每个项目至少有 3 个量化指标（性能/成本/业务价值）。

## 3. 推荐目录规划（按知识点）

```text
ai-agent-learning/
  roadmap.md
  topics/
    agent/
      knowledge/
      examples/
      qa/
    rag/
      knowledge/
      examples/
      qa/
    system-design/
      knowledge/
      examples/
      qa/
    interview/
      knowledge/
      examples/
      qa/
    foundation/
      programming-python/
        knowledge/
        examples/
        qa/
      programming-go/
        knowledge/
        examples/
        qa/
      programming-c/
        knowledge/
        examples/
        qa/
      networking/
        knowledge/
        examples/
        qa/
  external/
```

说明：统一按知识点建目录，避免把知识、示例、问答拆散在不同顶层目录。
说明补充：`foundation` 目录用于支线基础能力建设，默认服务主线主题学习。

## 4. 每周节奏模板（可复用）

- 周一：读 1-2 篇核心文档，整理问题清单。
- 周二-周三：写最小可运行示例。
- 周四：做性能/稳定性优化并记录指标。
- 周五：做 30-60 分钟面试口述与复盘。
- 周末：整理本周文档，更新下周计划。

## 5. 当前启动任务（本周）

- [ ] 跑通 `agent-demo` 的 01-04，并写运行记录。
- [ ] 输出第一份心智模型文档：Agent 核心模块与数据流。
- [ ] 输出第一份面试稿：ReAct、Tool Calling、Memory 的工程实现要点。

## 6. 后续扩展策略（需要时再加）

- 如果某主题深度不足，再引入对应外部仓库或官方文档。
- 优先引入原则：能直接提升当前阶段产出，不做“收藏式学习”。
- 每引入一个外部资料，都要落地为本仓库的一份笔记或一个可执行示例。

---

使用方式：每完成一个阶段，就在本文件追加“阶段复盘”小节，记录学到了什么、卡在哪里、下一阶段怎么改。

补充说明：本路线默认优先沉淀“知识点文档”和“可执行示例”，问答文档用于辅助表达训练。复杂主题要先补优质资料，再落地到对应 `topics/<topic>/`。
