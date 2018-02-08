#!/usr/bin/env python3
"""
Send fake protocol traffic
"""

import argparse
import messages
import socket
import datetime

HOST, PORT = "localhost", 10000

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)

    return parser.parse_args()

def generate_packet(msg, topic_name):
    header = messages.TopicHeader()
    header.msgid = messages.msgid(msg)
    header.name = topic_name

    header = header.SerializeToString()
    msg = msg.SerializeToString()

    header_size = messages.MessageSize(bytes=len(header)).SerializeToString()
    msg_size = messages.MessageSize(bytes=len(msg)).SerializeToString()

    return header_size + header + msg_size + msg


def main():
    args = parse_args()
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    m = messages.IMUReading()
    m.timestamp.us = int(datetime.datetime.now().timestamp())
    m.acceleration.x = 1
    m.acceleration.y = 2
    m.acceleration.z = 3
    m.angular_speed.x = 11
    m.angular_speed.y = 12
    m.angular_speed.z = 13
    m.magnetic_field.x = 21
    m.magnetic_field.y = 22
    m.magnetic_field.z = 23

    sock.sendto(generate_packet(m, "time"), (HOST, PORT))

if __name__ == '__main__':
    main()
