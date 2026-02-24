#include "platform_linux.h"
#include "protocol_stack.h"
#include "routing.h"
#include "routing_table.h"
#include "sf_commands.h"
#include "sf_protocol.h"
#include "hal.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SF_EP_SERVER ((void*)1)

static int server_fd = -1;
static sf_request_stats_t g_stats = {0};

static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

double sf_platform_now_ms(void) {
    return now_ms();
}

static uint64_t now_u64_ms(void) {
    double t = now_ms();
    if (t < 0) return 0;
    return (uint64_t)t;
}

static uint64_t htonll_u64(uint64_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return ((uint64_t)htonl((uint32_t)(x & 0xFFFFFFFFull)) << 32) | htonl((uint32_t)(x >> 32));
#else
    return x;
#endif
}

typedef struct sf_conn {
    int       fd;
    sf_rxbuf_t rx;
    uint8_t   tx[8192];
    size_t    tx_len;
    size_t    tx_off;
    char      remote_addr[64];
} sf_conn_t;

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) return -1;
    return 0;
}

int sf_platform_init(void) {
    sf_hal_init();
    memset(&g_stats, 0, sizeof(g_stats));
    return 0;
}

int sf_platform_listen(const char *bind_addr, uint16_t port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    if (set_nonblocking(server_fd) != 0) {
        perror("fcntl");
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
    if (listen(server_fd, 128) < 0) {
        perror("listen");
        return -1;
    }

    printf("SentryFlow firmware (epoll) listening on %s:%u\n", bind_addr, port);
    return 0;
}

static void close_conn(int epfd, sf_conn_t *c) {
    if (!c) return;
    epoll_ctl(epfd, EPOLL_CTL_DEL, c->fd, NULL);
    close(c->fd);
    free(c);
}

static int queue_response(sf_conn_t *c, uint8_t type, uint32_t seq, const uint8_t *payload, size_t payload_len) {
    if (!c) return -1;
    if (c->tx_len != 0) return -1;

    sf_frame_t rf;
    memset(&rf, 0, sizeof(rf));
    rf.version = SF_PROTO_VERSION;
    rf.type = type;
    rf.flags = 0;
    rf.seq = seq;

    size_t out_len = 0;
    if (sf_proto_encode(c->tx, sizeof(c->tx), &rf, payload, payload_len, &out_len) != 0) {
        return -1;
    }
    c->tx_len = out_len;
    c->tx_off = 0;
    return 0;
}

static int handle_frame(sf_conn_t *c, const sf_frame_t *f, const uint8_t *payload, size_t payload_len) {
    uint8_t out_payload[2048];
    size_t out_len = 0;
    uint8_t out_type = SF_MSG_ERROR;

    if (f->type == SF_MSG_PING) {
        out_type = SF_MSG_PONG;
        out_len = payload_len;
        if (out_len > sizeof(out_payload)) out_len = sizeof(out_payload);
        if (out_len) memcpy(out_payload, payload, out_len);
    } else if (f->type == SF_MSG_ECHO) {
        out_type = SF_MSG_ECHO_REPLY;
        out_len = payload_len;
        if (out_len > sizeof(out_payload)) out_len = sizeof(out_payload);
        if (out_len) memcpy(out_payload, payload, out_len);
    } else if (f->type == SF_MSG_GET_STATS) {
        out_type = SF_MSG_STATS_REPLY;

        sf_hal_telemetry_t tel;
        sf_hal_get_telemetry(&tel);

        /* Binary reply:
           total_requests(u64), bad_frames(u64), routes_installed(u64), uptime_ms(u64),
           last_latency_us(u32), avg_latency_us(u32)
         */
        uint64_t tr = htonll_u64(g_stats.total_requests);
        uint64_t bf = htonll_u64(g_stats.bad_frames);
        uint64_t ri = htonll_u64(g_stats.routes_installed);
        uint64_t up = htonll_u64(tel.uptime_ms);

        uint32_t last_us = htonl((uint32_t)(g_stats.last_latency_ms * 1000.0));
        uint32_t avg_us = htonl((uint32_t)(g_stats.avg_latency_ms * 1000.0));

        memcpy(out_payload + 0, &tr, 8);
        memcpy(out_payload + 8, &bf, 8);
        memcpy(out_payload + 16, &ri, 8);
        memcpy(out_payload + 24, &up, 8);
        memcpy(out_payload + 32, &last_us, 4);
        memcpy(out_payload + 36, &avg_us, 4);
        out_len = 40;
    } else if (f->type == SF_MSG_ROUTE_UPDATE) {
        out_type = SF_MSG_ROUTE_ACK;

        size_t applied = 0;
        size_t off = 0;
        uint32_t now_ms_u32 = (uint32_t)now_u64_ms();
        while (off + 16 <= payload_len) {
            sf_route_entry_t e;
            memset(&e, 0, sizeof(e));
            memcpy(&e.prefix_be, payload + off + 0, 4);
            e.mask_bits = payload[off + 4];
            uint16_t metric_be;
            memcpy(&metric_be, payload + off + 6, 2);
            e.metric = ntohs(metric_be);
            memcpy(&e.next_hop_be, payload + off + 8, 4);
            e.last_updated_ms = now_ms_u32;

            if (sf_route_table_upsert(sf_routing_table(), &e) == 0) {
                applied++;
                g_stats.routes_installed++;
            }
            off += 16;
        }

        uint32_t applied_be = htonl((uint32_t)applied);
        memcpy(out_payload, &applied_be, 4);
        out_len = 4;
    } else if (f->type == SF_MSG_ROUTE_LOOKUP) {
        out_type = SF_MSG_ROUTE_REPLY;
        if (payload_len < 4) {
            const char *msg = "bad payload";
            return queue_response(c, SF_MSG_ERROR, f->seq, (const uint8_t *)msg, strlen(msg));
        }
        uint32_t ip_be;
        memcpy(&ip_be, payload, 4);
        sf_route_entry_t best;
        if (sf_route_table_lookup(sf_routing_table(), ip_be, &best) != 0) {
            uint32_t zero = 0;
            uint16_t metric = htons(0xFFFFu);
            out_payload[0] = 0;
            out_payload[1] = 0;
            memcpy(out_payload + 2, &metric, 2);
            memcpy(out_payload + 4, &zero, 4);
            out_len = 8;
        } else {
            out_payload[0] = best.mask_bits;
            out_payload[1] = 0;
            uint16_t metric_be = htons(best.metric);
            memcpy(out_payload + 2, &metric_be, 2);
            memcpy(out_payload + 4, &best.next_hop_be, 4);
            out_len = 8;
        }
    } else {
        const char *msg = "unknown message type";
        out_type = SF_MSG_ERROR;
        out_len = strlen(msg);
        memcpy(out_payload, msg, out_len);
    }

    return queue_response(c, out_type, f->seq, out_payload, out_len);
}

static int update_epoll_interest(int epfd, sf_conn_t *c) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = c;
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
    if (c->tx_len != 0) ev.events |= EPOLLOUT;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, c->fd, &ev);
}

static void handle_readable(int epfd, sf_conn_t *c) {
    for (;;) {
        uint8_t tmp[2048];
        ssize_t n = recv(c->fd, tmp, sizeof(tmp), 0);
        if (n == 0) {
            close_conn(epfd, c);
            return;
        }
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            close_conn(epfd, c);
            return;
        }

        if (sf_rxbuf_append(&c->rx, tmp, (size_t)n) != 0) {
            close_conn(epfd, c);
            return;
        }

        for (;;) {
            sf_frame_t f;
            uint8_t payload[4096];
            size_t payload_len = 0;
            int r = sf_proto_try_decode(&c->rx, &f, payload, sizeof(payload), &payload_len);
            if (r == 0) break;
            if (r < 0) {
                g_stats.bad_frames++;
                close_conn(epfd, c);
                return;
            }

            double start = now_ms();
            if (handle_frame(c, &f, payload, payload_len) != 0) {
                close_conn(epfd, c);
                return;
            }
            double end = now_ms();
            double latency = end - start;

            g_stats.total_requests++;
            g_stats.last_latency_ms = latency;
            g_stats.avg_latency_ms =
                g_stats.avg_latency_ms +
                (latency - g_stats.avg_latency_ms) / (double)g_stats.total_requests;

            if (update_epoll_interest(epfd, c) != 0) {
                close_conn(epfd, c);
                return;
            }

            if (c->tx_len != 0) break;
        }
    }
}

static void handle_writable(int epfd, sf_conn_t *c) {
    while (c->tx_off < c->tx_len) {
        ssize_t n = send(c->fd, c->tx + c->tx_off, c->tx_len - c->tx_off, 0);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            close_conn(epfd, c);
            return;
        }
        c->tx_off += (size_t)n;
    }
    if (c->tx_off >= c->tx_len) {
        c->tx_len = 0;
        c->tx_off = 0;
        if (update_epoll_interest(epfd, c) != 0) {
            close_conn(epfd, c);
            return;
        }
    }
}

int sf_platform_accept_loop(void) {
    if (server_fd < 0) return -1;

    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        return -1;
    }

    struct epoll_event sev;
    memset(&sev, 0, sizeof(sev));
    sev.data.ptr = SF_EP_SERVER;
    sev.events = EPOLLIN;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &sev) != 0) {
        perror("epoll_ctl ADD server");
        close(epfd);
        return -1;
    }

    struct epoll_event events[64];
    for (;;) {
        int n = epoll_wait(epfd, events, (int)(sizeof(events) / sizeof(events[0])), 1000);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            close(epfd);
            return -1;
        }

        for (int i = 0; i < n; ++i) {
            if (events[i].data.ptr == SF_EP_SERVER) {
                for (;;) {
                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    int cfd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
                    if (cfd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        perror("accept");
                        break;
                    }
                    if (set_nonblocking(cfd) != 0) {
                        close(cfd);
                        continue;
                    }

                    sf_conn_t *c = (sf_conn_t *)calloc(1, sizeof(sf_conn_t));
                    if (!c) {
                        close(cfd);
                        continue;
                    }
                    c->fd = cfd;
                    sf_rxbuf_init(&c->rx);

                    inet_ntop(AF_INET, &client_addr.sin_addr, c->remote_addr, sizeof(c->remote_addr));

                    struct epoll_event ev;
                    memset(&ev, 0, sizeof(ev));
                    ev.data.ptr = c;
                    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev) != 0) {
                        close(cfd);
                        free(c);
                        continue;
                    }
                }
            } else {
                sf_conn_t *c = (sf_conn_t *)events[i].data.ptr;
                uint32_t ev = events[i].events;
                if (ev & (EPOLLHUP | EPOLLRDHUP)) {
                    close_conn(epfd, c);
                    continue;
                }
                if (ev & EPOLLIN) {
                    handle_readable(epfd, c);
                }
                if (ev & EPOLLOUT) {
                    handle_writable(epfd, c);
                }
            }
        }
    }
}

void sf_stack_get_stats(sf_request_stats_t *out) {
    if (!out) return;
    *out = g_stats;
}

