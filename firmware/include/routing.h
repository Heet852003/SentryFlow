#ifndef SENTRYFLOW_ROUTING_H
#define SENTRYFLOW_ROUTING_H

#include <stdint.h>

typedef enum {
    SF_ROUTE_DIRECT = 0,
    SF_ROUTE_SIMULATED_HOP = 1
} sf_route_strategy_t;

typedef struct sf_route_decision {
    sf_route_strategy_t strategy;
    uint8_t             hops;
} sf_route_decision_t;

void sf_routing_init(void);
void sf_routing_set_strategy(sf_route_strategy_t strategy);
sf_route_decision_t sf_routing_decide(const char *remote_addr);

#endif /* SENTRYFLOW_ROUTING_H */

