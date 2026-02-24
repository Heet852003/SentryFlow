import pytest

from sentryflow_protocol import decode_frame, encode_frame


def test_roundtrip() -> None:
    data = encode_frame(3, b"hello", seq=123, flags=0x10)
    f = decode_frame(data)
    assert f.msg_type == 3
    assert f.seq == 123
    assert f.flags == 0x10
    assert f.payload == b"hello"


def test_crc_reject() -> None:
    data = bytearray(encode_frame(1, b"abc", seq=1))
    data[-1] ^= 0xFF
    with pytest.raises(ValueError):
        decode_frame(bytes(data))

