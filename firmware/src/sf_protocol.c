#include "sf_protocol.h"
#include "sf_crc32.h"

#include <string.h>

#include <arpa/inet.h>

void sf_rxbuf_init(sf_rxbuf_t *rb) {
    if (!rb) return;
    rb->len = 0;
}

int sf_rxbuf_append(sf_rxbuf_t *rb, const uint8_t *data, size_t len) {
    if (!rb || (!data && len)) return -1;
    if (len == 0) return 0;
    if (rb->len + len > sizeof(rb->data)) return -1;
    memcpy(rb->data + rb->len, data, len);
    rb->len += len;
    return 0;
}

static void rb_consume(sf_rxbuf_t *rb, size_t n) {
    if (n >= rb->len) {
        rb->len = 0;
        return;
    }
    memmove(rb->data, rb->data + n, rb->len - n);
    rb->len -= n;
}

int sf_proto_encode(
    uint8_t *out,
    size_t out_cap,
    const sf_frame_t *frame,
    const uint8_t *payload,
    size_t payload_len,
    size_t *out_len
) {
    if (!out || !frame || !out_len) return -1;
    if (payload_len != 0 && !payload) return -1;
    if (payload_len > 1024 * 1024) return -1;

    size_t total = SF_PROTO_HEADER_LEN + payload_len;
    if (out_cap < total) return -1;

    uint32_t magic_be = htonl(SF_PROTO_MAGIC);
    memcpy(out + 0, &magic_be, 4);
    out[4] = frame->version;
    out[5] = frame->type;

    uint16_t flags_be = htons(frame->flags);
    memcpy(out + 6, &flags_be, 2);

    uint32_t seq_be = htonl(frame->seq);
    memcpy(out + 8, &seq_be, 4);

    uint32_t plen_be = htonl((uint32_t)payload_len);
    memcpy(out + 12, &plen_be, 4);

    uint32_t crc = sf_crc32(payload, payload_len);
    uint32_t crc_be = htonl(crc);
    memcpy(out + 16, &crc_be, 4);

    if (payload_len) {
        memcpy(out + SF_PROTO_HEADER_LEN, payload, payload_len);
    }

    *out_len = total;
    return 0;
}

int sf_proto_try_decode(
    sf_rxbuf_t *rb,
    sf_frame_t *out_frame,
    uint8_t *payload_out,
    size_t payload_cap,
    size_t *payload_len
) {
    if (!rb || !out_frame || !payload_len) return -1;
    if (rb->len < SF_PROTO_HEADER_LEN) return 0;

    uint32_t magic_be;
    memcpy(&magic_be, rb->data + 0, 4);
    uint32_t magic = ntohl(magic_be);
    if (magic != SF_PROTO_MAGIC) {
        return -1;
    }

    out_frame->version = rb->data[4];
    out_frame->type = rb->data[5];

    uint16_t flags_be;
    memcpy(&flags_be, rb->data + 6, 2);
    out_frame->flags = ntohs(flags_be);

    uint32_t seq_be;
    memcpy(&seq_be, rb->data + 8, 4);
    out_frame->seq = ntohl(seq_be);

    uint32_t plen_be;
    memcpy(&plen_be, rb->data + 12, 4);
    out_frame->payload_len = ntohl(plen_be);

    uint32_t crc_be;
    memcpy(&crc_be, rb->data + 16, 4);
    out_frame->payload_crc32 = ntohl(crc_be);

    if (out_frame->version != SF_PROTO_VERSION) return -1;
    if (out_frame->payload_len > sizeof(rb->data) - SF_PROTO_HEADER_LEN) return -1;

    size_t total = SF_PROTO_HEADER_LEN + (size_t)out_frame->payload_len;
    if (rb->len < total) return 0;

    if (out_frame->payload_len > payload_cap) return -1;
    if (out_frame->payload_len && payload_out) {
        memcpy(payload_out, rb->data + SF_PROTO_HEADER_LEN, out_frame->payload_len);
    }
    *payload_len = (size_t)out_frame->payload_len;

    uint32_t computed = sf_crc32(rb->data + SF_PROTO_HEADER_LEN, *payload_len);
    if (computed != out_frame->payload_crc32) return -1;

    rb_consume(rb, total);
    return 1;
}

int sf_proto_self_test(void) {
    uint8_t buf[256];
    uint8_t payload[32];
    for (int i = 0; i < (int)sizeof(payload); ++i) payload[i] = (uint8_t)i;

    sf_frame_t f;
    memset(&f, 0, sizeof(f));
    f.version = SF_PROTO_VERSION;
    f.type = 1;
    f.flags = 0x1234;
    f.seq = 42;

    size_t out_len = 0;
    if (sf_proto_encode(buf, sizeof(buf), &f, payload, sizeof(payload), &out_len) != 0) return -1;

    sf_rxbuf_t rb;
    sf_rxbuf_init(&rb);
    if (sf_rxbuf_append(&rb, buf, out_len) != 0) return -1;

    sf_frame_t decoded;
    uint8_t decoded_payload[64];
    size_t decoded_len = 0;
    int r = sf_proto_try_decode(&rb, &decoded, decoded_payload, sizeof(decoded_payload), &decoded_len);
    if (r != 1) return -1;
    if (decoded.seq != 42 || decoded.flags != 0x1234 || decoded_len != sizeof(payload)) return -1;
    if (memcmp(decoded_payload, payload, sizeof(payload)) != 0) return -1;
    if (rb.len != 0) return -1;

    return 0;
}

