#include "rpc.h"

#include "messages/RemoteProcedureCall.pb.h"
#include "messages/MessageSize.pb.h"

size_t rpc_process(rpc_callback_t *callbacks,
                   unsigned callbacks_len,
                   const uint8_t *input_buf,
                   size_t input_len,
                   uint8_t *output_buf,
                   size_t output_len)
{
    pb_istream_t istream;
    pb_ostream_t ostream;
    size_t offset;
    size_t max_size;
    unsigned i;

    // Unused for now
    (void)input_len;

    MessageSize header_size, msg_size;
    RPCRequestHeader header;

    /* Decode header size */
    offset = 0;
    max_size = MessageSize_size;
    istream = pb_istream_from_buffer(&input_buf[offset], max_size);
    pb_decode(&istream, MessageSize_fields, &header_size);

    /* Decode header */
    offset = MessageSize_size;
    max_size = header_size.bytes;
    istream = pb_istream_from_buffer(&input_buf[offset], max_size);
    pb_decode(&istream, RPCRequestHeader_fields, &header);

    /* Decode data size */
    offset = MessageSize_size + header_size.bytes;
    max_size = MessageSize_size;
    istream = pb_istream_from_buffer(&input_buf[offset], max_size);
    pb_decode(&istream, MessageSize_fields, &msg_size);

    /* Prepare data reader */
    offset = 2 * MessageSize_size + header_size.bytes;
    max_size = msg_size.bytes;
    istream = pb_istream_from_buffer(&input_buf[offset], max_size);

    /* Prepare output */
    ostream = pb_ostream_from_buffer(&output_buf[MessageSize_size], output_len - MessageSize_size);

    /* Looks for the correct callbacks and passes it the message data. */
    for (i = 0; i < callbacks_len; i++) {
        if (!strcmp(callbacks[i].name, header.name)) {
            callbacks[i].cb(callbacks[i].cb_arg, &istream, &ostream);
        }
    }

    /* Write answer. */
    MessageSize reply_size = MessageSize_init_default;
    reply_size.bytes = ostream.bytes_written;
    ostream = pb_ostream_from_buffer(output_buf, MessageSize_size);
    pb_encode(&ostream, MessageSize_fields, &reply_size);

    return ostream.bytes_written + reply_size.bytes;
}

bool rpc_buffer_is_complete(const uint8_t *buffer, size_t buffer_len)
{
    size_t offset;
    size_t max_size;
    pb_istream_t istream;

    MessageSize header_size, msg_size;

    if (buffer_len < 2 * MessageSize_size) {
        return false;
    }

    /* Read header size */
    offset = 0;
    max_size = MessageSize_size;
    istream = pb_istream_from_buffer(&buffer[offset], max_size);
    pb_decode(&istream, MessageSize_fields, &header_size);

    if (buffer_len < 2 * MessageSize_size + header_size.bytes) {
        return false;
    }

    offset = MessageSize_size + header_size.bytes;
    max_size = MessageSize_size;
    istream = pb_istream_from_buffer(&buffer[offset], max_size);
    pb_decode(&istream, MessageSize_fields, &msg_size);

    if (buffer_len < 2 * MessageSize_size + header_size.bytes + msg_size.bytes) {
        return false;
    }

    return true;
}
