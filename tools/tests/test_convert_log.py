from convert_logs import *
from unittest import TestCase
import messages
import io


class ConvertToCSVTestCase(TestCase):
    def test_read_messages(self):
        msg = messages.Timestamp(us=42)
        hdr = messages.TopicHeader(msgid=messages.msgid(msg), name="ts")
        def encoded_length(m):
            encoded = m.SerializeToString()
            res = messages.MessageSize(bytes=len(encoded))
            return res.SerializeToString()

        data = encoded_length(hdr) + hdr.SerializeToString()
        data = data + encoded_length(msg) + msg.SerializeToString()


        # Duplicate data so that it is twice here
        data = io.BytesIO(data + data)

        msg_list = list(read_messages(data))

        self.assertEqual(len(msg_list), 2, "There should be only two messages.")

        self.assertEqual(hdr, msg_list[0][0])
        self.assertEqual(msg, msg_list[0][1])
        self.assertEqual(hdr, msg_list[1][0])
        self.assertEqual(msg, msg_list[1][1])

