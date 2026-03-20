#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void die(const char *message) {
    perror(message);
    exit(1);
}

int main(void) {
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
    server_addr.sin_port = htons(8080);

    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        die("bind");
    }

    if (listen(listen_fd, 128) < 0) {
        die("listen");
    }

    printf("listening on 0.0.0.0:8080\n");

    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (conn_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            die("accept");
        }

        char client_ip[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip)) != NULL) {
            printf("accepted connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
        }

        char buffer[1024];
        ssize_t bytes_read = read(conn_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            perror("read");
            close(conn_fd);
            continue;
        }

        buffer[bytes_read] = '\0';
        printf("received request:\n%s\n", buffer);

        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 18\r\n"
            "Connection: close\r\n"
            "\r\n"
            "hello from server\n";

        if (write(conn_fd, response, strlen(response)) < 0) {
            perror("write");
        }

        close(conn_fd);
    }

    close(listen_fd);
    return 0;
}