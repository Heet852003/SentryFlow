#include "protocol_stack.h"
#include "platform_linux.h"
#include "sf_protocol.h"
#include "routing_table.h"

#include <stdio.h>
#include <string.h>

static char g_bind_addr[32] = "0.0.0.0";
static unsigned short g_port = 9000;

int sf_stack_init(const char *bind_addr, unsigned short port) {
    if (bind_addr && *bind_addr) {
        strncpy(g_bind_addr, bind_addr, sizeof(g_bind_addr) - 1);
        g_bind_addr[sizeof(g_bind_addr) - 1] = '\0';
    }
    g_port = port ? port : 9000;

    if (sf_platform_init() != 0) {
        fprintf(stderr, "platform init failed\n");
        return -1;
    }
    if (sf_platform_listen(g_bind_addr, g_port) != 0) {
        fprintf(stderr, "listen failed\n");
        return -1;
    }
    return 0;
}

int sf_stack_run(void) {
    return sf_platform_accept_loop();
}

int sf_stack_self_test(void) {
    int ok = 1;
    if (sf_proto_self_test() != 0) {
        fprintf(stderr, "self-test failed: protocol framing\n");
        ok = 0;
    }
    if (sf_route_table_self_test() != 0) {
        fprintf(stderr, "self-test failed: routing table\n");
        ok = 0;
    }
    if (!ok) return 1;
    printf("SentryFlow firmware self-test: OK\n");
    return 0;
}

