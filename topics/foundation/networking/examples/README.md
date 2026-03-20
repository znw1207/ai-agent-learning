# Networking Examples

这些例子不追求生产可用，而是帮助把知识文档里的核心概念和最小代码路径对应起来。

## 1. `01_basic_tcp_server.c`

演示最小服务端流程：

- `socket()`
- `bind()`
- `listen()`
- `accept()`
- `read()` / `write()`

编译：

```bash
gcc 01_basic_tcp_server.c -o basic_tcp_server
```

运行：

```bash
./basic_tcp_server
```

另开一个终端测试：

```bash
curl http://127.0.0.1:8080/
```

## 2. `02_dns_lookup.c`

演示最小 DNS 解析流程：

- `getaddrinfo()`
- 遍历返回地址
- 打印 IPv4 / IPv6 结果

编译：

```bash
gcc 02_dns_lookup.c -o dns_lookup
```

运行：

```bash
./dns_lookup example.com
```

## 3. `03_select_poll_epoll_echo_server.c`

演示同一个 echo server 如何分别用：

- `select`
- `poll`
- `epoll`

运行时通过参数选择模式。

编译：

```bash
gcc 03_select_poll_epoll_echo_server.c -o multiplex_server
```

运行：

```bash
./multiplex_server select
./multiplex_server poll
./multiplex_server epoll
```

测试：

```bash
nc 127.0.0.1 9090
```

输入内容后，服务端会原样回显。

## 4. 学习建议

- 先结合 [01-tcp-server-connection-lifecycle.md](/home/wzn/codes/learn_agent/ai-agent-learning/topics/foundation/networking/knowledge/01-tcp-server-connection-lifecycle.md) 看 `01_basic_tcp_server.c`。
- 再结合 [08-dns-resolution-and-troubleshooting.md](/home/wzn/codes/learn_agent/ai-agent-learning/topics/foundation/networking/knowledge/08-dns-resolution-and-troubleshooting.md) 看 `02_dns_lookup.c`。
- 再结合 [14-select-poll-and-epoll.md](/home/wzn/codes/learn_agent/ai-agent-learning/topics/foundation/networking/knowledge/14-select-poll-and-epoll.md) 看 `03_select_poll_epoll_echo_server.c`。
- 这些例子只覆盖最小主干，不包含生产级错误处理、超时、连接池、线程池、复杂背压治理等能力。