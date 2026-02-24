#ifndef SENTRYFLOW_COMMANDS_H
#define SENTRYFLOW_COMMANDS_H

#include <stdint.h>

typedef enum {
    SF_MSG_PING = 1,
    SF_MSG_PONG = 2,
    SF_MSG_ECHO = 3,
    SF_MSG_ECHO_REPLY = 4,
    SF_MSG_GET_STATS = 5,
    SF_MSG_STATS_REPLY = 6,
    SF_MSG_ROUTE_UPDATE = 7,
    SF_MSG_ROUTE_ACK = 8,
    SF_MSG_ROUTE_LOOKUP = 9,
    SF_MSG_ROUTE_REPLY = 10,
    SF_MSG_ERROR = 255
} sf_msg_type_t;

typedef enum {
    SF_FLAG_NONE = 0,
    SF_FLAG_ACK_REQUIRED = 1 << 0
} sf_msg_flags_t;

const char *sf_msg_type_name(uint8_t type);

#endif /* SENTRYFLOW_COMMANDS_H */

