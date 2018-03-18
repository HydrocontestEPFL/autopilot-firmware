import unittest
import rpc
import messages

MESSAGESIZE_SIZE = len(messages.MessageSize(bytes=0).SerializeToString())

class UDPInjectionTestCase(unittest.TestCase):
    def test_generate(self):
        data = messages.Timestamp()
        data.us = 42
        topic = "/imu"
        msg = rpc.generate_topic_injection_packet(topic, data)

        to_decode, msg = msg[:MESSAGESIZE_SIZE], msg[MESSAGESIZE_SIZE:]
        header_size = messages.MessageSize()
        header_size.ParseFromString(to_decode)

        to_decode, msg = msg[:header_size.bytes], msg[header_size.bytes:]
        header = messages.TopicHeader()
        header.ParseFromString(to_decode)

        self.assertEqual(header.name, "/imu")
        self.assertEqual(header.msgid, 1)

        to_decode, msg = msg[:MESSAGESIZE_SIZE], msg[MESSAGESIZE_SIZE:]
        body_size = messages.MessageSize()
        body_size.ParseFromString(to_decode)

        to_decode, msg = msg[:header_size.bytes], msg[header_size.bytes:]
        body = messages.Timestamp()
        body.ParseFromString(to_decode)
        self.assertEqual(42, body.us)

