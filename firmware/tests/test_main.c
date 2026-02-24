#include "sf_protocol.h"
#include "routing_table.h"

#include <stdio.h>

int main(void) {
    int ok = 1;
    if (sf_proto_self_test() != 0) {
        fprintf(stderr, "FAIL: protocol framing\n");
        ok = 0;
    }
    if (sf_route_table_self_test() != 0) {
        fprintf(stderr, "FAIL: routing table\n");
        ok = 0;
    }
    if (ok) {
        printf("OK: firmware unit tests\n");
        return 0;
    }
    return 1;
}

