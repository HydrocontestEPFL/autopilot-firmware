import unittest
import messages

class MessageIdAreUnique(unittest.TestCase):
    def test_message_ids_are_unique_per_message_type(self):
        seen = dict()

        for m in messages.messages:
            msgid = messages.msgid(m)
            if not msgid: # optional
                continue
            if msgid in seen:
                msg = "ID {} is not unique ({} & {} have it)"
                self.fail(msg.format(msgid, m, seen[msgid]))
            seen[msgid] = m


