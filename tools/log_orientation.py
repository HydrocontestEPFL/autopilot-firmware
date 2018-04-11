#!/usr/bin/env python3
"""
Logs traffic coming on protobuf from the various topics.
"""

import sys
import time
import os.path
import argparse
import socketserver
import re
import math

from google.protobuf import text_format

import messages


def parse_packet(data):
    messagesize_size = len(messages.MessageSize(bytes=0).SerializeToString())

    header_size = messages.MessageSize()
    header_size.ParseFromString(data[0:messagesize_size])

    cut_point = messagesize_size + header_size.bytes
    header_data, msg_data = data[:cut_point], data[cut_point:]

    header = messages.TopicHeader()
    header.ParseFromString(header_data[messagesize_size:])

    for m in messages.messages:
        if messages.msgid(m) == header.msgid:
            msg = m()
            msg.ParseFromString(msg_data)
            break
    else:
        msg = None

    return header, msg


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--port", "-p", default=10000, help="Port to listen on (10000)")
    parser.add_argument(
        "--filter", "-f", help="Filter regexp to apply on the topic name")

    return parser.parse_args()


def quaternion_to_euler_angle(w, x, y, z):
    ysqr = y * y

    t0 = +2.0 * (w * x + y * z)
    t1 = +1.0 - 2.0 * (x * x + ysqr)
    X = math.atan2(t0, t1)

    t2 = +2.0 * (w * y - z * x)
    t2 = +1.0 if t2 > +1.0 else t2
    t2 = -1.0 if t2 < -1.0 else t2
    Y = math.asin(t2)

    t3 = +2.0 * (w * z + x * y)
    t4 = +1.0 - 2.0 * (ysqr + z * z)
    Z = math.atan2(t3, t4)

    return X, Y, Z


class Handler(socketserver.BaseRequestHandler):
    def handle(self):
        data = self.request[0]
        header, msg = parse_packet(data)

        if header.name != "state":
            return

        x, y, z = quaternion_to_euler_angle(
            msg.orientation.w, msg.orientation.x, msg.orientation.y,
            msg.orientation.z)

        x = math.degrees(x)
        y = math.degrees(y)
        z = math.degrees(z)

        print("{:.0f}° {:.0f}°".format(x, y))


def main():
    args = parse_args()

    if args.filter:
        topic_filter = re.compile(args.filter, flags=re.IGNORECASE)
    else:
        topic_filter = re.compile(".*")

    with socketserver.UDPServer(("0.0.0.0", args.port), Handler) as server:
        server.serve_forever()


if __name__ == '__main__':
    main()
