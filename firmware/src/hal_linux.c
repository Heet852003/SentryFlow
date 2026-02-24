#include "hal.h"

#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#include <time.h>

static uint64_t start_ms = 0;

static uint64_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ull + (uint64_t)ts.tv_nsec / 1000000ull;
}

int sf_hal_init(void) {
    start_ms = now_ms();
    return 0;
}

void sf_hal_get_telemetry(sf_hal_telemetry_t *out) {
    if (!out) return;
    memset(out, 0, sizeof(*out));

    uint64_t t = now_ms();
    out->monotonic_ms = t;
    out->uptime_ms = (t >= start_ms) ? (t - start_ms) : 0;
    out->pid = (uint32_t)getpid();
}

