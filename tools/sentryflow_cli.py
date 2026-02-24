import argparse
import asyncio
import json

from sentryflow_client import (
    Msg,
    encode_route_entries,
    encode_route_lookup,
    parse_route_reply,
    parse_stats,
    request_once,
)


async def main_async() -> int:
    parser = argparse.ArgumentParser(description="SentryFlow protocol CLI client.")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9000)

    sub = parser.add_subparsers(dest="cmd", required=True)

    sub.add_parser("ping")

    echo_p = sub.add_parser("echo")
    echo_p.add_argument("text")

    sub.add_parser("stats")

    ru = sub.add_parser("route-update")
    ru.add_argument("--entry", action="append", required=True, help="prefix,mask,nextHop,metric (e.g. 10.0.0.0,8,10.0.0.1,10)")

    rl = sub.add_parser("route-lookup")
    rl.add_argument("ip")

    args = parser.parse_args()

    if args.cmd == "ping":
        t, p = await request_once(args.host, args.port, Msg.PING, b"ping", seq=1)
        print({"type": t, "payload": p.decode("utf-8", "replace")})
        return 0

    if args.cmd == "echo":
        t, p = await request_once(args.host, args.port, Msg.ECHO, args.text.encode("utf-8"), seq=1)
        print({"type": t, "payload": p.decode("utf-8", "replace")})
        return 0

    if args.cmd == "stats":
        t, p = await request_once(args.host, args.port, Msg.GET_STATS, b"", seq=1)
        if t != Msg.STATS_REPLY:
            print({"type": t, "payload": p.decode("utf-8", "replace")})
            return 1
        s = parse_stats(p)
        print(json.dumps(s.__dict__, indent=2))
        return 0

    if args.cmd == "route-update":
        entries = []
        for e in args.entry:
            prefix, mask, nh, metric = e.split(",")
            entries.append((prefix, int(mask), nh, int(metric)))
        payload = encode_route_entries(entries)
        t, p = await request_once(args.host, args.port, Msg.ROUTE_UPDATE, payload, seq=1)
        print({"type": t, "payload_len": len(p)})
        return 0

    if args.cmd == "route-lookup":
        payload = encode_route_lookup(args.ip)
        t, p = await request_once(args.host, args.port, Msg.ROUTE_LOOKUP, payload, seq=1)
        if t != Msg.ROUTE_REPLY:
            print({"type": t, "payload": p.decode("utf-8", "replace")})
            return 1
        r = parse_route_reply(p)
        print({"result": r})
        return 0

    return 2


def main() -> None:
    raise SystemExit(asyncio.run(main_async()))


if __name__ == "__main__":
    main()

