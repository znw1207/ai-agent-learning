# RAG From Scratch Examples

目标：用最小可运行示例，从检索基础走到 Agentic RAG。

学习顺序：

1. `01_keyword_retrieval.py`
- 先理解“检索”是什么，不引入向量和模型。

2. `02_vector_retrieval_tfidf.py`
- 用纯 Python 实现简化 TF-IDF + cosine，相当于向量检索的入门版本。

3. `03_basic_rag_pipeline.py`
- 搭出完整传统 RAG：`query -> retrieve -> augment -> answer`。
- 使用 `OpenAI().responses.create` 调用 LLM 生成答案（默认 `gpt-5.4`）。

4. `04_agentic_rag_iterative.py`
- 在传统 RAG 之上加“决策循环”：查询改写、多轮检索、结果充足性判断。
- 查询改写、充足性判断、最终回答都使用 `OpenAI().responses.create`。

运行方式：

```bash
cd topics/rag/examples
python 01_keyword_retrieval.py
python 02_vector_retrieval_tfidf.py
python 03_basic_rag_pipeline.py
python 04_agentic_rag_iterative.py
```

依赖与环境变量

```bash
pip install openai
export OPENAI_API_KEY="your_api_key"
python 03_basic_rag_pipeline.py
python 04_agentic_rag_iterative.py
```

说明：
- `01`、`02` 仅用标准库，`03`、`04` 依赖 `openai` SDK。
- 语料在 `data/mini_corpus.json`，你可以自行扩展。
