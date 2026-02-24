#include "protocol_stack.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    int self_test = 0;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--self-test") == 0) {
            self_test = 1;
        }
    }

    if (self_test) {
        return sf_stack_self_test();
    }

    if (sf_stack_init("0.0.0.0", 9000) != 0) {
        return 1;
    }
    printf("SentryFlow firmware starting main loop (port 9000)\n");
    return sf_stack_run();
}

