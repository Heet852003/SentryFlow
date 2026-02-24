#ifndef SENTRYFLOW_PROTOCOL_STACK_H
#define SENTRYFLOW_PROTOCOL_STACK_H

#include <stdint.h>

typedef struct sf_request_stats {
    uint64_t total_requests;
    double   last_latency_ms;
    double   avg_latency_ms;
} sf_request_stats_t;

int  sf_stack_init(const char *bind_addr, uint16_t port);
int  sf_stack_run(void);
int  sf_stack_self_test(void);
void sf_stack_get_stats(sf_request_stats_t *out);

#endif /* SENTRYFLOW_PROTOCOL_STACK_H */

