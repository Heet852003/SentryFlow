#include "routing.h"

void sf_routing_init(void) {
    sf_routing_set_strategy(SF_ROUTE_DIRECT);
}

static sf_route_strategy_t current_strategy = SF_ROUTE_DIRECT;

void sf_routing_set_strategy(sf_route_strategy_t strategy) {
    current_strategy = strategy;
}

sf_route_decision_t sf_routing_decide(const char *remote_addr) {
    (void)remote_addr;
    sf_route_decision_t d;
    d.strategy = current_strategy;
    d.hops = (current_strategy == SF_ROUTE_DIRECT) ? 1 : 3;
    return d;
}

