#include "protocol_stack.h"

#include <iostream>

extern "C" void sf_stack_get_stats(sf_request_stats_t *out);

static void print_stats() {
    sf_request_stats_t stats{};
    sf_stack_get_stats(&stats);
    std::cout << "[router-metrics] total=" << stats.total_requests
              << " last_ms=" << stats.last_latency_ms
              << " avg_ms=" << stats.avg_latency_ms
              << std::endl;
}

void sf_router_metrics_tick() {
    print_stats();
}

