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

    # TODO: Handle other parameter types
    parser.add_argument("val", type=float)

    return parser.parse_args()


def main():
    args = parse_args()
    conn = socket.create_connection((args.host, args.port))

    param = messages.Parameter(name=args.name, scalar_value=args.val)
    req = messages.ParameterSetRequest(parameter=param)
    conn.send(rpc.encode_request("parameter_server_set", req))
    reply = rpc.read_reply(conn, messages.ParameterSetReply)
    print(reply)


if __name__ == '__main__':
    main()
