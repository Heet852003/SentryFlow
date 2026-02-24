#include "protocol_stack.h"
#include "routing.h"
#include "routing_table.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_u16(const char *s, uint16_t *out) {
    if (!s || !out) return -1;
    char *end = NULL;
    long v = strtol(s, &end, 10);
    if (!end || *end != '\0') return -1;
    if (v <= 0 || v > 65535) return -1;
    *out = (uint16_t)v;
    return 0;
}

static int parse_u16_metric(const char *s, uint16_t *out) {
    if (!s || !out) return -1;
    char *end = NULL;
    long v = strtol(s, &end, 10);
    if (!end || *end != '\0') return -1;
    if (v < 0 || v > 65535) return -1;
    *out = (uint16_t)v;
    return 0;
}

int main(int argc, char **argv) {
    int self_test = 0;
    const char *bind = "0.0.0.0";
    uint16_t port = 9000;
    sf_route_strategy_t strategy = SF_ROUTE_DIRECT;

    sf_routing_init();

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--self-test") == 0) {
            self_test = 1;
        } else if (strcmp(argv[i], "--bind") == 0 && i + 1 < argc) {
            bind = argv[++i];
        } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            if (parse_u16(argv[++i], &port) != 0) {
                fprintf(stderr, "invalid --port\n");
                return 2;
            }
        } else if (strcmp(argv[i], "--strategy") == 0 && i + 1 < argc) {
            const char *v = argv[++i];
            if (strcmp(v, "direct") == 0) strategy = SF_ROUTE_DIRECT;
            else if (strcmp(v, "hop") == 0) strategy = SF_ROUTE_SIMULATED_HOP;
            else {
                fprintf(stderr, "invalid --strategy (direct|hop)\n");
                return 2;
            }
        } else if (strcmp(argv[i], "--route") == 0 && i + 4 < argc) {
            /* --route <prefix> <maskBits> <nextHop> <metric> */
            const char *prefix_s = argv[++i];
            const char *mask_s = argv[++i];
            const char *nh_s = argv[++i];
            const char *metric_s = argv[++i];

            struct in_addr prefix, nh;
            if (inet_pton(AF_INET, prefix_s, &prefix) != 1 || inet_pton(AF_INET, nh_s, &nh) != 1) {
                fprintf(stderr, "invalid --route ip\n");
                return 2;
            }
            uint16_t metric = 0;
            if (parse_u16_metric(metric_s, &metric) != 0) {
                fprintf(stderr, "invalid --route metric\n");
                return 2;
            }
            int mask_bits = atoi(mask_s);
            if (mask_bits < 0 || mask_bits > 32) {
                fprintf(stderr, "invalid --route mask\n");
                return 2;
            }

            sf_route_entry_t e = {0};
            e.prefix_be = prefix.s_addr;
            e.mask_bits = (uint8_t)mask_bits;
            e.metric = metric;
            e.next_hop_be = nh.s_addr;
            sf_route_table_upsert(sf_routing_table(), &e);
        }
    }

    if (self_test) {
        return sf_stack_self_test();
    }

    sf_routing_set_strategy(strategy);

    if (sf_stack_init(bind, port) != 0) {
        return 1;
    }
    printf("SentryFlow firmware starting main loop (%s:%u)\n", bind, port);
    return sf_stack_run();
}

