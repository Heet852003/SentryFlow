#include "routing.h"

#include <arpa/inet.h>
#include <string.h>

static sf_route_strategy_t current_strategy = SF_ROUTE_DIRECT;
static sf_route_table_t g_table;

void sf_routing_init(void) {
    sf_routing_set_strategy(SF_ROUTE_DIRECT);
    sf_route_table_init(&g_table);
}

void sf_routing_set_strategy(sf_route_strategy_t strategy) {
    current_strategy = strategy;
}

sf_route_table_t *sf_routing_table(void) {
    return &g_table;
}

sf_route_decision_t sf_routing_decide(const char *remote_addr) {
    sf_route_decision_t d;
    memset(&d, 0, sizeof(d));
    d.strategy = current_strategy;

    struct in_addr addr;
    if (remote_addr && inet_pton(AF_INET, remote_addr, &addr) == 1) {
        sf_route_entry_t best;
        if (sf_route_table_lookup(&g_table, addr.s_addr, &best) == 0) {
            d.matched_prefix_bits = best.mask_bits;
            d.metric = best.metric;
            d.next_hop_be = best.next_hop_be;
            d.hops = (current_strategy == SF_ROUTE_DIRECT) ? 1 : (uint8_t)(1 + (best.metric / 5u));
            if (d.hops < 1) d.hops = 1;
            return d;
        }
    }

    d.hops = (current_strategy == SF_ROUTE_DIRECT) ? 1 : 3;
    d.metric = 0xFFFFu;
    d.next_hop_be = 0;
    return d;
}

