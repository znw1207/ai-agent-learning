import json
import math
import re
from collections import Counter, defaultdict
from pathlib import Path


def tokenize(text):
    return re.findall(r"[a-zA-Z]+", text.lower())


def load_corpus():
    data_path = Path(__file__).parent / "data" / "mini_corpus.json"
    return json.loads(data_path.read_text(encoding="utf-8"))


def build_tfidf_vectors(texts):
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
        vec = {}
        for term, freq in tf.items():
            vec[term] = (freq / length) * idf.get(term, 0.0)
        vectors.append(vec)
    return vectors, idf


def cosine_sparse(vec_a, vec_b):
    if not vec_a or not vec_b:
        return 0.0
    common = set(vec_a.keys()) & set(vec_b.keys())
    dot = sum(vec_a[t] * vec_b[t] for t in common)
    norm_a = math.sqrt(sum(v * v for v in vec_a.values()))
    norm_b = math.sqrt(sum(v * v for v in vec_b.values()))
    if norm_a == 0.0 or norm_b == 0.0:
        return 0.0
    return dot / (norm_a * norm_b)


def query_vector(query, idf):
    tokens = tokenize(query)
    tf = Counter(tokens)
    length = sum(tf.values()) or 1
    qvec = {}
    for term, freq in tf.items():
        qvec[term] = (freq / length) * idf.get(term, 0.0)
    return qvec


def main():
    docs = load_corpus()
    texts = [d["title"] + " " + d["text"] for d in docs]
    doc_vecs, idf = build_tfidf_vectors(texts)

    query = "how does agentic rag differ from normal rag"
    qvec = query_vector(query, idf)

    scored = []
    for doc, dvec in zip(docs, doc_vecs):
        score = cosine_sparse(qvec, dvec)
        scored.append((score, doc))
    scored.sort(key=lambda x: x[0], reverse=True)

    print("=== Step 2: Simple Vector Retrieval (TF-IDF) ===")
    print(f"Query: {query}\n")
    for rank, (score, doc) in enumerate(scored[:3], start=1):
        print(f"{rank}. score={score:.4f} id={doc['id']} title={doc['title']}")
        print(f"   {doc['text']}")


if __name__ == "__main__":
    main()
