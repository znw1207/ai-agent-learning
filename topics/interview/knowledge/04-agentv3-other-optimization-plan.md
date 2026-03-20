# agentV3 后续可深化的其它方向

目标：除了 RAG 之外，把这个项目继续提升到更有面试深度的方向。

---

## 1. Eval

这是最高优先级。

要补：

- 30-50 条评估样本
- case 分类
- 通过标准
- 基线成绩
- 优化前后对比

没有这层，后续所有优化都不容易证明有效。

---

## 2. Memory

当前项目的 memory 还比较弱。

可以分成两层：

### 2.1 短期执行状态

- run 上下文
- 当前步骤结果
- tool 历史
- 中断后恢复所需状态

### 2.2 长期知识 / 偏好

- 设计规范库
- 页面案例库
- 团队约定
- 用户或租户级偏好

注意：

- 长期 memory 不应等于“把所有历史塞进 prompt”
- 更适合走检索式加载

---

## 3. Context Compression

随着 run 变长，上下文一定会越来越重。

可以补：

- 长历史压缩摘要
- 工具结果提炼
- 保留关键决策，不保留全部 token
- 失败 run 的摘要化回放

这点和 `learn-claude-code` 里强调的 context compact 思路高度一致。

---

## 4. Concurrency / Background Tasks

如果后续工具越来越多，有些操作会明显变慢。

可以考虑：

- 后台执行慢工具
- 让 Agent 不阻塞等待长耗时任务
- 异步通知工具完成结果

这会让系统更像一个成熟 harness，而不是一次性串行脚本。

---

## 5. Error Handling

当前还需要进一步显式化失败分类和恢复策略。

建议至少补：

- 可重试错误
- 不可重试错误
- 工具错误
- 校验错误
- 模型错误
- 数据错误

然后为每类错误补：

- 日志
- terminate_reason
- 恢复策略
- 人工介入策略

---

## 6. Recovery / Durability

要让项目从原型升级到更像生产系统，这层很关键。

建议补：

- 持久化 run state
- 持久化 event log
- reconnect / resume
- 幂等 key
- 服务重启后恢复

---

## 7. Security / Governance

这是另一个明显能拉开项目深度的方向。

建议补：

- 工具权限分级
- 高风险写入审批
- dry-run 模式
- patch scope 限制
- 审计日志

---

## 8. Planning / Reflection / Subagent

这类高级模式应该放在后面。

建议顺序：

1. Eval
2. Observability
3. Recovery
4. Security
5. RAG knowledge loading
6. Reflection / Planner-Executor
7. Subagent / Multi-Agent

因为前五层没打稳时，上更复杂的 Agent 结构只会放大排障成本。
