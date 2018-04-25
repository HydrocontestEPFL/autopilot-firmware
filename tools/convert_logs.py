#!/usr/bin/env python3
"""
Convert a log file from protobuf binary format to text.
"""

import argparse
import messages
import progressbar
import os
from google.protobuf.message import DecodeError
from google.protobuf import text_format


def field_names(message):
    for descriptor, value in message.ListFields():
        import pdb
        pdb.set_trace()
        print(descriptor)


def read_messages(f):
    """
    Yields a series of (TopicHeader, Message) tuples, contained in the given
    log file.
    """
    len_size = len(messages.MessageSize(bytes=0).SerializeToString())

    data = True
    try:
        while data:
            hdr_len = messages.MessageSize()
            hdr_len.ParseFromString(f.read(len_size))
            hdr = messages.TopicHeader()
            hdr.ParseFromString(f.read(hdr_len.bytes))
            msg_len = messages.MessageSize()
            msg_len.ParseFromString(f.read(len_size))

            for m in messages.messages:
                if messages.msgid(m) == hdr.msgid:
                    Message = m
                    break

            data = f.read(msg_len.bytes)
            msg = Message()
            msg.ParseFromString(data)

            if data:
                yield hdr, msg

    except DecodeError:
        pass


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "logfile",
        type=argparse.FileType('rb'),
        help="Log file (protobuf format")
    parser.add_argument(
        "out_file",
        type=argparse.FileType('w'),
        help="File where the human readable log will go")

    return parser.parse_args()


def main():
    args = parse_args()

    size = args.logfile.seek(0, os.SEEK_END)
    args.logfile.seek(0, os.SEEK_SET)

    pbar = progressbar.ProgressBar(maxval=size).start()

    for header, msg in read_messages(args.logfile):
        args.out_file.write("===\n")
        args.out_file.write("topic: '{}'\n".format(header.name))
        args.out_file.write("type: {}\n".format(msg.DESCRIPTOR.name))
        args.out_file.write("data:\n")
        args.out_file.write(text_format.MessageToString(msg, indent=2))
        pbar.update(args.logfile.seek(0, os.SEEK_CUR))
    pbar.finish()



if __name__ == '__main__':
    main()
