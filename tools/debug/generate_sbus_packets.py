#!/usr/bin/env python3
"""
"""

import argparse

CHANNEL_CNT = 16
BITS_PER_CHANNEL = 11

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("channel", type=int)
    parser.add_argument("value", type=int)

    return parser.parse_args()

def bytes_from_channel(channel, value):
    # create an 11 bin value
    bin_value = bin(value)[2:]
    bin_value = "0" * (BITS_PER_CHANNEL - len(bin_value)) + bin_value
    bin_value = bin_value[::-1]

    channels_before = "0" * (BITS_PER_CHANNEL*channel)
    channels_after = "0" * (BITS_PER_CHANNEL*(CHANNEL_CNT-(channel + 1)))

    bitpattern = channels_before + bin_value + channels_after

    data = [bitpattern[i:i+8] for i in range(0, len(bitpattern), 8)]
    data = [d[::-1] for d in data]
    data = [int(d, 2) for d in data]

    return data

def main():
    args = parse_args()

    data = bytes_from_channel(args.channel, args.value)
    for i, val in zip(range(1, 25), data):
        if val != 0:
            print("bytes[{}] = 0x{:X};".format(i, val))

if __name__ == '__main__':
    main()
