#!/usr/bin/env python3
"""
Sets one parameter on board of the boat
"""

import argparse
import messages
import socket
import rpc


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("host", help="IP address of the board")
    parser.add_argument(
        "--port",
        "-p",
        type=int,
        default=10000,
        help="Port to connect to (default 10000)")

    parser.add_argument(
        "name",
        help="Name of the parameter with full path, e.g. /led/blink_rate")
    parser.add_argument(
        "--save",
        action="store_true",
        help="Store parameter tree to flash after setting it.")

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--integer", "-i", type=int, help="Integer value")
    group.add_argument(
        "--scalar", "-s", type=float, help="Floating point value")
    group.add_argument(
        "--boolean",
        "-b",
        type=lambda s: bool(int(s)),
        help="Boolean value (as 0 or 1)")

    return parser.parse_args()


def main():
    args = parse_args()
    conn = socket.create_connection((args.host, args.port))

    param = messages.Parameter(name=args.name)

    if args.integer is not None:
        param.int_value = args.integer
    elif args.scalar is not None:
        param.scalar_value = args.scalar
    else:
        param.bool_value = args.boolean

    req = messages.ParameterSetRequest(
        parameter=param, save_to_flash=args.save)
    conn.send(rpc.encode_request("parameter_server_set", req))
    reply = rpc.read_reply(conn, messages.ParameterSetReply)
    print(reply)


if __name__ == '__main__':
    main()
