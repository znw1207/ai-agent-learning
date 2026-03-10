# External Repositories (Submodules)

本目录用于托管学习参考仓库（`git submodule`）。

## 已纳入

- `external/AgentGuide`
- `external/langgraph`
- `external/opencode`
- `external/agentic-design-patterns`

## 拉取方式

首次克隆：

```bash
git clone git@github.com:znw1207/ai-agent-learning.git
cd ai-agent-learning
git submodule update --init --recursive
```

已有仓库更新子模块：

```bash
git submodule update --init --recursive
git submodule update --remote --recursive
```

## 暂未纳入

- `component-app-webservice`
- 原因：当前远程地址权限不足（`Repository not found`），无法从该 URL 拉取。
- 处理方式：补充可访问的远程地址后，再执行：

```bash
git submodule add <accessible-repo-url> external/component-app-webservice
```
