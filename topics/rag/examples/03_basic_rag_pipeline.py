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
    q_tokens = tokenize(query)
    tf = Counter(q_tokens)
    q_len = sum(tf.values()) or 1
    qvec = {term: (freq / q_len) * idf.get(term, 0.0) for term, freq in tf.items()}

    scored = []
    for doc, dvec in zip(docs, doc_vecs):
        score = cosine_sparse(qvec, dvec)
        scored.append((score, doc))
    scored.sort(key=lambda x: x[0], reverse=True)
    return [doc for score, doc in scored[:top_k] if score > 0]


def generate_answer_with_openai(query, contexts, model="gpt-5.4"):
    if not contexts:
        raise ValueError("No contexts retrieved; cannot run LLM generation.")

    client = OpenAI()
    context_text = "\n".join(
        f"[{idx}] {doc['title']}: {doc['text']}" for idx, doc in enumerate(contexts, start=1)
    )
    prompt = (
        "You are a RAG assistant. Use only the provided context. "
        "If context is insufficient, say it clearly.\n\n"
        f"Question: {query}\n\n"
        f"Context:\n{context_text}\n\n"
        "Return a concise grounded answer."
    )

    response = client.responses.create(
        model=model,
        input=prompt,
    )
    return response.output_text.strip()


def main():
    docs = load_corpus()
    texts = [d["title"] + " " + d["text"] for d in docs]
    doc_vecs, idf = build_tfidf(texts)

    query = "what is the difference between rag and agentic rag"
    retrieved = retrieve(query, docs, doc_vecs, idf, top_k=2)
    answer = generate_answer_with_openai(query, retrieved, model="gpt-5.4")

    print("=== Step 3: Basic RAG Pipeline ===")
    print(f"Query: {query}\n")
    print("Retrieved context:")
    for i, doc in enumerate(retrieved, start=1):
        print(f"{i}. [{doc['id']}] {doc['title']}: {doc['text']}")
    print("\nGeneration mode: OpenAI Responses API (gpt-5.4)")
    print("\nGenerated answer:")
    print(answer)


if __name__ == "__main__":
    main()
