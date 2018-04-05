#!/usr/bin/env python3
"""
Download all parameters from the boat.
"""

import argparse
import messages
import socket
import rpc
from collections import defaultdict
from functools import reduce
import json
import yaml
from yaml.representer import Representer


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
        "--json", help="Dump as JSON (default: YAML)", action="store_true")

    return parser.parse_args()


def main():
    args = parse_args()
    conn = socket.create_connection((args.host, args.port))

    index = 0

    # Create a dict of dict data structures that automatically adds new dicts
    # on query.
    def nested_dict():
        return defaultdict(nested_dict)

    parameters = nested_dict()

    while True:
        req = messages.ParameterEnumerationRequest(index=index)
        conn.send(rpc.encode_request("parameter_server_enumerate", req))
        reply = rpc.read_reply(conn, messages.ParameterEnumerationReply)

        # If the optional parameter is not here, it means we reached the last
        # parameter
        if not reply.HasField('parameter'):
            break

        # Convert the parameter path into a dict.
        # E.g.: /bar/foo -> parameters['bar']['foo']
        path = reply.parameter.name.split("/")
        path, name = path[1:-1], path[-1]
        ns = reduce(lambda tree, key: tree[key], path, parameters)

        # Put the value in it
        if reply.parameter.HasField('int_value'):
            ns[name] = reply.parameter.int_value
        elif reply.parameter.HasField('scalar_value'):
            ns[name] = reply.parameter.scalar_value
        elif reply.parameter.HasField('bool_value'):
            ns[name] = reply.parameter.bool_value
        else:
            raise TypeError("Unknown parameter type")

        index += 1

    # Finally shows the values to the user
    if args.json:
        print(json.dumps(parameters, indent=2, sort_keys=True))
    else:
        # Needed so that default dict appears as dict and not !!defaultdict
        yaml.add_representer(defaultdict, Representer.represent_dict)
        print(yaml.dump(parameters, default_flow_style=False))


if __name__ == '__main__':
    main()
