#ifndef SENTRYFLOW_ROUTING_TABLE_H
#define SENTRYFLOW_ROUTING_TABLE_H

#include <stdint.h>
#include <stddef.h>

#define SF_ROUTE_TABLE_MAX 256

typedef struct sf_route_entry {
    uint32_t prefix_be;     /* IPv4 prefix in network byte order */
    uint8_t  mask_bits;     /* 0..32 */
    uint16_t metric;        /* lower is better */
    uint32_t next_hop_be;   /* next hop IPv4 in network byte order */
    uint32_t last_updated_ms;
} sf_route_entry_t;

typedef struct sf_route_table {
    sf_route_entry_t entries[SF_ROUTE_TABLE_MAX];
    size_t           count;
} sf_route_table_t;

void   sf_route_table_init(sf_route_table_t *rt);
size_t sf_route_table_count(const sf_route_table_t *rt);
int    sf_route_table_upsert(sf_route_table_t *rt, const sf_route_entry_t *e);
int    sf_route_table_remove(sf_route_table_t *rt, uint32_t prefix_be, uint8_t mask_bits);
int    sf_route_table_lookup(const sf_route_table_t *rt, uint32_t ip_be, sf_route_entry_t *out_best);

int sf_route_table_self_test(void);

#endif /* SENTRYFLOW_ROUTING_TABLE_H */

