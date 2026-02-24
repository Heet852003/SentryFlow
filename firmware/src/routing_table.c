#include "routing_table.h"

#include <string.h>
#include <arpa/inet.h>

static uint32_t mask_from_bits(uint8_t bits) {
    if (bits == 0) return 0u;
    if (bits >= 32) return 0xFFFFFFFFu;
    return 0xFFFFFFFFu << (32 - bits);
}

void sf_route_table_init(sf_route_table_t *rt) {
    if (!rt) return;
    memset(rt, 0, sizeof(*rt));
}

size_t sf_route_table_count(const sf_route_table_t *rt) {
    return rt ? rt->count : 0;
}

int sf_route_table_upsert(sf_route_table_t *rt, const sf_route_entry_t *e) {
    if (!rt || !e) return -1;
    if (e->mask_bits > 32) return -1;

    for (size_t i = 0; i < rt->count; ++i) {
        if (rt->entries[i].prefix_be == e->prefix_be && rt->entries[i].mask_bits == e->mask_bits) {
            rt->entries[i] = *e;
            return 0;
        }
    }

    if (rt->count >= SF_ROUTE_TABLE_MAX) return -1;
    rt->entries[rt->count++] = *e;
    return 0;
}

int sf_route_table_remove(sf_route_table_t *rt, uint32_t prefix_be, uint8_t mask_bits) {
    if (!rt) return -1;
    for (size_t i = 0; i < rt->count; ++i) {
        if (rt->entries[i].prefix_be == prefix_be && rt->entries[i].mask_bits == mask_bits) {
            rt->entries[i] = rt->entries[rt->count - 1];
            rt->count--;
            return 0;
        }
    }
    return -1;
}

int sf_route_table_lookup(const sf_route_table_t *rt, uint32_t ip_be, sf_route_entry_t *out_best) {
    if (!rt || !out_best) return -1;
    if (rt->count == 0) return -1;

    uint32_t ip = ip_be;
    int found = 0;
    sf_route_entry_t best = {0};

    for (size_t i = 0; i < rt->count; ++i) {
        sf_route_entry_t e = rt->entries[i];
        uint32_t mask = htonl(mask_from_bits(e.mask_bits));
        if ((ip & mask) == (e.prefix_be & mask)) {
            if (!found) {
                best = e;
                found = 1;
            } else {
                if (e.mask_bits > best.mask_bits) {
                    best = e;
                } else if (e.mask_bits == best.mask_bits && e.metric < best.metric) {
                    best = e;
                }
            }
        }
    }

    if (!found) return -1;
    *out_best = best;
    return 0;
}

int sf_route_table_self_test(void) {
    sf_route_table_t rt;
    sf_route_table_init(&rt);

    sf_route_entry_t e1 = {0};
    e1.prefix_be = htonl(0x0A000000u); /* 10.0.0.0 */
    e1.mask_bits = 8;
    e1.metric = 10;
    e1.next_hop_be = htonl(0x0A000001u);
    if (sf_route_table_upsert(&rt, &e1) != 0) return -1;

    sf_route_entry_t e2 = {0};
    e2.prefix_be = htonl(0x0A010000u); /* 10.1.0.0 */
    e2.mask_bits = 16;
    e2.metric = 5;
    e2.next_hop_be = htonl(0x0A010001u);
    if (sf_route_table_upsert(&rt, &e2) != 0) return -1;

    sf_route_entry_t best = {0};
    if (sf_route_table_lookup(&rt, htonl(0x0A010203u), &best) != 0) return -1; /* 10.1.2.3 */
    if (best.mask_bits != 16) return -1;
    if (best.next_hop_be != e2.next_hop_be) return -1;

    if (sf_route_table_lookup(&rt, htonl(0x0A020203u), &best) != 0) return -1; /* 10.2.2.3 */
    if (best.mask_bits != 8) return -1;
    if (best.next_hop_be != e1.next_hop_be) return -1;

    return 0;
}

