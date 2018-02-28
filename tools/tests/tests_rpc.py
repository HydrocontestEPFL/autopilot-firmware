from unittest import TestCase
from unittest.mock import Mock
import time
from messages import Timestamp, MessageSize, RPCRequestHeader
import rpc

MESSAGESIZE_SIZE = len(MessageSize(bytes=0).SerializeToString())

class RPCEncodingTest(TestCase):
    def test_encode_request(self):
        data = Timestamp(us=100)
        msg = rpc.encode_request("myrpc", data)
        # Parse header size
        to_decode, msg = msg[:MESSAGESIZE_SIZE], msg[MESSAGESIZE_SIZE:]
        header_size = MessageSize()
        header_size.ParseFromString(to_decode)

        # Parse header
        to_decode, msg = msg[:header_size.bytes], msg[header_size.bytes:]
        header = RPCRequestHeader()
        header.ParseFromString(to_decode)
        self.assertEqual("myrpc", header.name)

        # Parse request size
        to_decode, msg = msg[:MESSAGESIZE_SIZE], msg[MESSAGESIZE_SIZE:]
        request_size = MessageSize()
        request_size.ParseFromString(to_decode)

        # Parse request size
        request = Timestamp()
        request.ParseFromString(msg)
        self.assertEqual(100, request.us)

    def test_read_reply_in_one_go(self):
        """
        Checks that we can read a reply from a socket.
        """
        reply = Timestamp(us=100).SerializeToString()
        reply_len = MessageSize(bytes=len(reply)).SerializeToString()
        socket = Mock()
        socket.recv.side_effect = [reply_len, reply]

        reply = rpc.read_reply(socket, Timestamp)
