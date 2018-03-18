from messages import MessageSize, RPCRequestHeader, TopicHeader, msgid
MESSAGESIZE_SIZE = len(MessageSize(bytes=0).SerializeToString())

def encode_request(method_name, data):
    """
    Encodes a request to be sent over TCP to the board.

    :param str method_name: The method name as defined by the RPC server.
    :param google.protobuf.message.Message data: The request parameters.
    :return: A bytes object containing the encoded message
    """
    request = data.SerializeToString()
    request_size = MessageSize(bytes=len(request)).SerializeToString()

    header = RPCRequestHeader(name=method_name).SerializeToString()
    header_size = MessageSize(bytes=len(header)).SerializeToString()

    return header_size + header + request_size + request

def read_reply(socket, reply_type):
    """
    Reads a reply from the given socket.
    """
    data = socket.recv(MESSAGESIZE_SIZE)
    reply_size = MessageSize()
    reply_size.ParseFromString(data)

    data = socket.recv(reply_size.bytes)
    reply = reply_type()
    reply.ParseFromString(data)

    return reply

def generate_topic_injection_packet(topic, data):
    """
    Creates a data message than can be injected on the internal msgbus through
    UDP.
    """
    header = TopicHeader(name=topic, msgid=msgid(data)).SerializeToString()
    data = data.SerializeToString()
    header_size = MessageSize(bytes=len(header)).SerializeToString()
    data_size = MessageSize(bytes=len(data)).SerializeToString()
    return header_size + header + data_size + data
