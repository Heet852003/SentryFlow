#ifndef SENTRYFLOW_HAL_H
#define SENTRYFLOW_HAL_H

#include <stdint.h>

typedef struct sf_hal_telemetry {
    uint64_t uptime_ms;
    uint64_t monotonic_ms;
    uint32_t pid;
} sf_hal_telemetry_t;

int sf_hal_init(void);
void sf_hal_get_telemetry(sf_hal_telemetry_t *out);

#endif /* SENTRYFLOW_HAL_H */

