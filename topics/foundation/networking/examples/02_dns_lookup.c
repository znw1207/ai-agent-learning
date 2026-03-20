#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_address(struct addrinfo *result) {
    char host[NI_MAXHOST];
    int status = getnameinfo(result->ai_addr, result->ai_addrlen, host, sizeof(host), NULL, 0,
                             NI_NUMERICHOST);
    if (status != 0) {
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(status));
        return;
    }

    const char *family = result->ai_family == AF_INET6 ? "IPv6" : "IPv4";
    printf("%s %s\n", family, host);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *results = NULL;
    int status = getaddrinfo(argv[1], NULL, &hints, &results);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    printf("results for %s:\n", argv[1]);
    for (struct addrinfo *current = results; current != NULL; current = current->ai_next) {
        if (current->ai_family == AF_INET || current->ai_family == AF_INET6) {
            print_address(current);
        }
    }

    freeaddrinfo(results);
    return 0;
}