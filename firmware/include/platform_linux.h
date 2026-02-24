#ifndef SENTRYFLOW_PLATFORM_LINUX_H
#define SENTRYFLOW_PLATFORM_LINUX_H

#include <stdint.h>

int   sf_platform_init(void);
int   sf_platform_listen(const char *bind_addr, uint16_t port);
int   sf_platform_accept_loop(void);
double sf_platform_now_ms(void);

#endif /* SENTRYFLOW_PLATFORM_LINUX_H */

