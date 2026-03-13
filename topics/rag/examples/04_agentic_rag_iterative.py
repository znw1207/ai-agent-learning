import json
import math
import re
from collections import Counter, defaultdict
from pathlib import Path

from openai import OpenAI


def tokenize(text):
    return re.findall(r"[a-zA-Z]+", text.lower())


def load_corpus():
    data_path = Path(__file__).parent / "data" / "mini_corpus.json"
    return json.loads(data_path.read_text(encoding="utf-8"))


def build_tfidf(texts):
    doc_tokens = [tokenize(t) for t in texts]
    n_docs = len(doc_tokens)
    df = defaultdict(int)
    for tokens in doc_tokens:
        for term in set(tokens):
            df[term] += 1
    idf = {term: math.log((1 + n_docs) / (1 + freq)) + 1 for term, freq in df.items()}

    vectors = []
    for tokens in doc_tokens:
        tf = Counter(tokens)
        length = sum(tf.values()) or 1
        vectors.append({term: (freq / length) * idf.get(term, 0.0) for term, freq in tf.items()})
    return vectors, idf


def cosine_sparse(a, b):
    common = set(a) & set(b)
    dot = sum(a[t] * b[t] for t in common)
    na = math.sqrt(sum(v * v for v in a.values()))
    nb = math.sqrt(sum(v * v for v in b.values()))
    if na == 0.0 or nb == 0.0:
        return 0.0
    return dot / (na * nb)


def retrieve(query, docs, doc_vecs, idf, top_k=2):
    tf = Counter(tokenize(query))
    length = sum(tf.values()) or 1
    qvec = {term: (freq / length) * idf.get(term, 0.0) for term, freq in tf.items()}

    scored = []
    for doc, dvec in zip(docs, doc_vecs):
        scored.append((cosine_sparse(qvec, dvec), doc))
    scored.sort(key=lambda x: x[0], reverse=True)
    return [doc for score, doc in scored[:top_k] if score > 0]


def call_model(prompt, model="gpt-5.4"):
    client = OpenAI()
    response = client.responses.create(
        model=model,
        input=prompt,
    )
    return response.output_text.strip()


def rewrite_query_with_llm(original_query, step, retrieved_docs, model="gpt-5.4"):
    history = "\n".join(f"- {d['title']}: {d['text']}" for d in retrieved_docs) or "(no docs yet)"
    prompt = (
        "You are an agentic retrieval planner.\n"
        f"Original question: {original_query}\n"
        f"Current step: {step}\n"
        f"Retrieved evidence so far:\n{history}\n\n"
        "Rewrite a better retrieval query in one sentence only."
    )
    return call_model(prompt, model=model)


def enough_evidence_with_llm(original_query, retrieved_docs, model="gpt-5.4"):
    evidence = "\n".join(f"- {d['title']}: {d['text']}" for d in retrieved_docs) or "(no docs yet)"
    prompt = (
        "You are a retrieval judge.\n"
        f"Question: {original_query}\n"
        f"Evidence:\n{evidence}\n\n"
        "Answer with only YES or NO: Is the evidence sufficient to answer accurately?"
    )
    verdict = call_model(prompt, model=model).upper()
    return "YES" in verdict


def summarize_with_llm(query, retrieved_docs, model="gpt-5.4"):
    if not retrieved_docs:
        raise ValueError("No retrieved docs to summarize.")
    evidence = "\n".join(f"- {d['title']}: {d['text']}" for d in retrieved_docs)
    prompt = (
        "You are a grounded RAG assistant. Use only the evidence below.\n"
        f"Question: {query}\n"
        f"Evidence:\n{evidence}\n\n"
        "Write a concise answer and explicitly mention traditional RAG vs Agentic RAG difference."
    )
    return call_model(prompt, model=model)


def main():
    docs = load_corpus()
    texts = [d["title"] + " " + d["text"] for d in docs]
    doc_vecs, idf = build_tfidf(texts)

    original_query = "difference between rag and agentic rag"
    max_steps = 4
    all_retrieved = []
    model = "gpt-5.4"

    print("=== Step 4: Agentic RAG (Iterative) ===")
    print(f"Original query: {original_query}\n")

    for step in range(max_steps):
        query = rewrite_query_with_llm(original_query, step + 1, all_retrieved, model=model)
        docs_step = retrieve(query, docs, doc_vecs, idf, top_k=2)

        print(f"Step {step + 1} query: {query}")
        for d in docs_step:
            print(f"  - {d['id']} {d['title']}")

        seen_ids = {d["id"] for d in all_retrieved}
        for d in docs_step:
            if d["id"] not in seen_ids:
                all_retrieved.append(d)

        if enough_evidence_with_llm(original_query, all_retrieved, model=model):
            print("Evidence judged as sufficient, stop retrieving.\n")
            break
        print("Evidence not sufficient, continue.\n")

    print("Final synthesized answer:\n")
    print(summarize_with_llm(original_query, all_retrieved, model=model))


if __name__ == "__main__":
    main()
