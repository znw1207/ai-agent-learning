#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 9090
#define MAX_CLIENTS 1024
#define MAX_EVENTS 64
#define BUFFER_SIZE 4096

static void die(const char *message) {
    perror(message);
    exit(1);
}

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int create_listener(void) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        die("socket");
    }

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        die("setsockopt");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        die("bind");
    }

    if (listen(listen_fd, 128) < 0) {
        die("listen");
    }

    if (set_nonblocking(listen_fd) < 0) {
        die("fcntl listen_fd");
    }

    return listen_fd;
}

static int echo_once(int fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == 0) {
        return -1;
    }
    if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            return 0;
        }
        return -1;
    }

    ssize_t total_written = 0;
    while (total_written < bytes_read) {
        ssize_t written = write(fd, buffer + total_written, (size_t)(bytes_read - total_written));
        if (written < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;
            }
            return -1;
        }
        total_written += written;
    }

    return 0;
}

static void close_fd(int fd) {
    if (fd >= 0) {
        close(fd);
    }
}

static void print_client(const struct sockaddr_in *client_addr, int fd) {
    char client_ip[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, sizeof(client_ip)) != NULL) {
        printf("accepted fd=%d from %s:%d\n", fd, client_ip, ntohs(client_addr->sin_port));
    }
}

static int accept_one(int listen_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
    if (conn_fd < 0) {
        return -1;
    }
    if (set_nonblocking(conn_fd) < 0) {
        close(conn_fd);
        return -1;
    }
    print_client(&client_addr, conn_fd);
    return conn_fd;
}

static void run_select_loop(int listen_fd) {
    int clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i] = -1;
    }

    printf("mode=select listening on 0.0.0.0:%d\n", PORT);

    for (;;) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);
        int maxfd = listen_fd;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i] >= 0) {
                FD_SET(clients[i], &readfds);
                if (clients[i] > maxfd) {
                    maxfd = clients[i];
                }
            }
        }

        int ready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            die("select");
        }

        if (FD_ISSET(listen_fd, &readfds)) {
            for (;;) {
                int conn_fd = accept_one(listen_fd);
                if (conn_fd < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        break;
                    }
                    break;
                }

                int placed = 0;
                for (int i = 0; i < MAX_CLIENTS; ++i) {
                    if (clients[i] < 0) {
                        clients[i] = conn_fd;
                        placed = 1;
                        break;
                    }
                }
                if (!placed) {
                    close_fd(conn_fd);
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int fd = clients[i];
            if (fd >= 0 && FD_ISSET(fd, &readfds)) {
                if (echo_once(fd) < 0) {
                    close_fd(fd);
                    clients[i] = -1;
                }
            }
        }
    }
}

static void run_poll_loop(int listen_fd) {
    struct pollfd pfds[MAX_CLIENTS + 1];
    for (int i = 0; i < MAX_CLIENTS + 1; ++i) {
        pfds[i].fd = -1;
        pfds[i].events = POLLIN;
        pfds[i].revents = 0;
    }
    pfds[0].fd = listen_fd;

    printf("mode=poll listening on 0.0.0.0:%d\n", PORT);

    for (;;) {
        int ready = poll(pfds, MAX_CLIENTS + 1, -1);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            die("poll");
        }

        if (pfds[0].revents & POLLIN) {
            for (;;) {
                int conn_fd = accept_one(listen_fd);
                if (conn_fd < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        break;
                    }
                    break;
                }

                int placed = 0;
                for (int i = 1; i < MAX_CLIENTS + 1; ++i) {
                    if (pfds[i].fd < 0) {
                        pfds[i].fd = conn_fd;
                        pfds[i].events = POLLIN;
                        pfds[i].revents = 0;
                        placed = 1;
                        break;
                    }
                }
                if (!placed) {
                    close_fd(conn_fd);
                }
            }
        }

        for (int i = 1; i < MAX_CLIENTS + 1; ++i) {
            if (pfds[i].fd >= 0 && (pfds[i].revents & (POLLIN | POLLERR | POLLHUP))) {
                if (echo_once(pfds[i].fd) < 0) {
                    close_fd(pfds[i].fd);
                    pfds[i].fd = -1;
                }
            }
            pfds[i].revents = 0;
        }
        pfds[0].revents = 0;
    }
}

static void run_epoll_loop(int listen_fd) {
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        die("epoll_create1");
    }

    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = listen_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &event) < 0) {
        die("epoll_ctl add listen_fd");
    }

    struct epoll_event events[MAX_EVENTS];
    printf("mode=epoll listening on 0.0.0.0:%d\n", PORT);

    for (;;) {
        int ready = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            die("epoll_wait");
        }

        for (int i = 0; i < ready; ++i) {
            int fd = events[i].data.fd;
            if (fd == listen_fd) {
                for (;;) {
                    int conn_fd = accept_one(listen_fd);
                    if (conn_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        }
                        break;
                    }

                    struct epoll_event client_event;
                    memset(&client_event, 0, sizeof(client_event));
                    client_event.events = EPOLLIN;
                    client_event.data.fd = conn_fd;
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &client_event) < 0) {
                        close_fd(conn_fd);
                    }
                }
            } else {
                if (echo_once(fd) < 0) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close_fd(fd);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <select|poll|epoll>\n", argv[0]);
        return 1;
    }

    int listen_fd = create_listener();

    if (strcmp(argv[1], "select") == 0) {
        run_select_loop(listen_fd);
    } else if (strcmp(argv[1], "poll") == 0) {
        run_poll_loop(listen_fd);
    } else if (strcmp(argv[1], "epoll") == 0) {
        run_epoll_loop(listen_fd);
    } else {
        fprintf(stderr, "unknown mode: %s\n", argv[1]);
        close_fd(listen_fd);
        return 1;
    }

    close_fd(listen_fd);
    return 0;
}