#!/usr/bin/env python3
"""
Turns off or on LEDs on the board. Useful to test the RPC framework.
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
        "led", choices=['green', 'red', 'blue'], help="LED to set")
    parser.add_argument("status", choices=['on', 'off'], help="LED status")

    return parser.parse_args()


def led_set(conn, led, value):
    led = messages.SetLedRequest.Led.Value(led.upper())
    msg = messages.SetLedRequest(led=led, status=value)
    conn.send(rpc.encode_request('led_set', msg))


def main():
    args = parse_args()
    conn = socket.create_connection((args.host, args.port))
    led_set(conn, args.led, args.status == 'on')


if __name__ == '__main__':
    main()
