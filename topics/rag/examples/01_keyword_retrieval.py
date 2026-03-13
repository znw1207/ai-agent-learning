import json
from pathlib import Path


def load_corpus():
    data_path = Path(__file__).parent / "data" / "mini_corpus.json"
    return json.loads(data_path.read_text(encoding="utf-8"))


def keyword_search(query, docs, top_k=3):
    q_terms = set(query.lower().split())
    scored = []
    for doc in docs:
        text_terms = set((doc["title"] + " " + doc["text"]).lower().split())
        score = len(q_terms & text_terms)
        scored.append((score, doc))
    scored.sort(key=lambda x: x[0], reverse=True)
    return [item for item in scored if item[0] > 0][:top_k]


def main():
    docs = load_corpus()
    query = "what is rag retrieval and generation"
    hits = keyword_search(query, docs)

    print("=== Step 1: Keyword Retrieval ===")
    print(f"Query: {query}\n")
    for rank, (score, doc) in enumerate(hits, start=1):
        print(f"{rank}. score={score} id={doc['id']} title={doc['title']}")
        print(f"   {doc['text']}")


if __name__ == "__main__":
    main()
