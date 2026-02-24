#include "platform_linux.h"
#include "protocol_stack.h"
#include "routing.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static int server_fd = -1;
static sf_request_stats_t g_stats = {0};

double sf_platform_now_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec * 1000.0 + (double)tv.tv_usec / 1000.0;
}

int sf_platform_init(void) {
    sf_routing_init();
    memset(&g_stats, 0, sizeof(g_stats));
    return 0;
}

int sf_platform_listen(const char *bind_addr, uint16_t port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(bind_addr);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }
    if (listen(server_fd, 16) < 0) {
        perror("listen");
        return -1;
    }

    printf("SentryFlow firmware listening on %s:%u\n", bind_addr, port);
    return 0;
}

static void handle_client(int client_fd, struct sockaddr_in *client_addr) {
    char buf[1024];
    ssize_t n;

    double start_ms = sf_platform_now_ms();
    n = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) {
        close(client_fd);
        return;
    }
    buf[n] = '\0';

    char addr_str[64];
    inet_ntop(AF_INET, &client_addr->sin_addr, addr_str, sizeof(addr_str));

    sf_route_decision_t decision = sf_routing_decide(addr_str);

    double end_ms = sf_platform_now_ms();
    double latency = end_ms - start_ms;

    g_stats.total_requests++;
    g_stats.last_latency_ms = latency;
    g_stats.avg_latency_ms =
        g_stats.avg_latency_ms +
        (latency - g_stats.avg_latency_ms) / (double)g_stats.total_requests;

    char response[512];
    int len = snprintf(
        response,
        sizeof(response),
        "OK hops=%u latency_ms=%.3f avg_ms=%.3f msg=%s",
        decision.hops,
        latency,
        g_stats.avg_latency_ms,
        buf
    );
    send(client_fd, response, (size_t)len, 0);
    close(client_fd);
}

int sf_platform_accept_loop(void) {
    if (server_fd < 0) {
        return -1;
    }

    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            return -1;
        }
        handle_client(client_fd, &client_addr);
    }
}

void sf_stack_get_stats(sf_request_stats_t *out) {
    if (!out) return;
    *out = g_stats;
}

