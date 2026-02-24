#ifndef SENTRYFLOW_PROTOCOL_H
#define SENTRYFLOW_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#define SF_PROTO_MAGIC 0x53464C57u /* 'SFLW' */
#define SF_PROTO_VERSION 1u
#define SF_PROTO_HEADER_LEN 20u

typedef struct sf_frame {
    uint8_t  version;
    uint8_t  type;
    uint16_t flags;
    uint32_t seq;
    uint32_t payload_len;
    uint32_t payload_crc32;
} sf_frame_t;

typedef struct sf_rxbuf {
    uint8_t data[8192];
    size_t  len;
} sf_rxbuf_t;

void sf_rxbuf_init(sf_rxbuf_t *rb);
int  sf_rxbuf_append(sf_rxbuf_t *rb, const uint8_t *data, size_t len);

int sf_proto_encode(
    uint8_t *out,
    size_t out_cap,
    const sf_frame_t *frame,
    const uint8_t *payload,
    size_t payload_len,
    size_t *out_len
);

/* Returns: 1 if a frame was decoded, 0 if more data is needed, -1 on parse error. */
int sf_proto_try_decode(
    sf_rxbuf_t *rb,
    sf_frame_t *out_frame,
    uint8_t *payload_out,
    size_t payload_cap,
    size_t *payload_len
);

int sf_proto_self_test(void);

#endif /* SENTRYFLOW_PROTOCOL_H */

