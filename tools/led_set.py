#!/usr/bin/env python3
"""
Turns off or on LEDs on the board. Useful to test the RPC framework.
"""

import argparse
import messages
import socket

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("host", help="IP address of the board")
    parser.add_argument("--port", "-p", type=int, default=10000,
                        help="Port to connect to (default 10000)")

    parser.add_argument("led", choices=['green', 'red', 'blue'],
                        help="LED to set")
    parser.add_argument("status", choices=['on', 'off'],
                        help="LED status")

    return parser.parse_args()

def main():
    args = parse_args()

    conn = socket.create_connection((args.host, args.port))

    header = messages.RPCRequestHeader(name="led_set").SerializeToString()
    header_size = messages.MessageSize(bytes=len(header)).SerializeToString()

    led = messages.SetLedRequest.Led.Value(args.led.upper())
    status = (args.status == 'on')

    msg = messages.SetLedRequest(led=led, status=status).SerializeToString()

    msg_size = messages.MessageSize(bytes=len(msg)).SerializeToString()

    conn.send(header_size + header + msg_size + msg)

    print(conn.recv(1024))

if __name__ == '__main__':
    main()
