#include "sf_commands.h"

const char *sf_msg_type_name(uint8_t type) {
    switch (type) {
        case SF_MSG_PING: return "PING";
        case SF_MSG_PONG: return "PONG";
        case SF_MSG_ECHO: return "ECHO";
        case SF_MSG_ECHO_REPLY: return "ECHO_REPLY";
        case SF_MSG_GET_STATS: return "GET_STATS";
        case SF_MSG_STATS_REPLY: return "STATS_REPLY";
        case SF_MSG_ROUTE_UPDATE: return "ROUTE_UPDATE";
        case SF_MSG_ROUTE_ACK: return "ROUTE_ACK";
        case SF_MSG_ROUTE_LOOKUP: return "ROUTE_LOOKUP";
        case SF_MSG_ROUTE_REPLY: return "ROUTE_REPLY";
        case SF_MSG_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

