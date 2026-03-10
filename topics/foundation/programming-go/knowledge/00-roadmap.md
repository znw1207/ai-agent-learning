# Go 基础学习 Roadmap（Foundation）

定位：本主题是支线能力，用于支撑主线中的高并发服务与工程化实现。

## 1. 服务的主线场景

- `topics/system-design`：高并发 API、网关、任务调度服务。
- `topics/agent`：工具服务（Tool Service）和轻量执行器。
- `topics/rag`：检索中间层与缓存服务。

## 2. 学习范围

- 语言基础：结构体、接口、错误处理、包管理。
- 并发模型：goroutine、channel、context、worker pool。
- Web 开发：HTTP 服务、路由、中间件、配置管理。
- 工程实践：日志、监控、测试、性能分析（pprof）。

## 3. 产出要求

- `knowledge/`：并发模型与服务设计文档。
- `examples/`：最小 API 服务、并发任务池示例。
- `qa/`：Go 面试与系统设计问答。

## 4. 第一周任务

- [ ] 写一个最小 HTTP 服务（带健康检查）。
- [ ] 写一个并发任务池示例（带超时取消）。
- [ ] 总结“Go 为什么适合做 Agent 工具网关”。

## 5. 验收标准

- 能解释 Go 并发模型和 Python 异步模型差异。
- 能写出基础可观测（日志+指标）的服务。
- 能把 Go 技能映射到主线中的高并发场景。
