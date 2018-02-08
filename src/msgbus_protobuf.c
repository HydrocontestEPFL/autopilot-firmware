#include "msgbus_protobuf.h"
#include "messages/MessageSize.pb.h"
#include "messages/TopicHeader.pb.h"
#include <pb_encode.h>

size_t messagebus_encode_topic_message(messagebus_topic_t *topic,
                                       uint8_t *buf, size_t buf_len,
                                       uint8_t *obj_buf, size_t obj_buf_len)
{
    size_t offset;
    int32_t max_len;

    topic_metadata_t *metadata = (topic_metadata_t *)topic->metadata;

    TopicHeader header;
    MessageSize header_size, msg_size;

    /* First populate information in the header */
    strncpy(header.name, topic->name, sizeof(header.name) - 1);
    header.msgid = metadata->msgid;

    pb_ostream_t stream;
    offset = MessageSize_size;
    max_len = buf_len - 2 * MessageSize_size;
    if (max_len <= 0) {
        return 0;
    }

    stream = pb_ostream_from_buffer(&buf[offset], max_len);
    if(!pb_encode(&stream, TopicHeader_fields, &header)) {
        return 0;
    }

    header_size.bytes = stream.bytes_written;

    offset = 0;
    max_len = MessageSize_size;
    stream = pb_ostream_from_buffer(&buf[offset], max_len);

    if(!pb_encode(&stream, MessageSize_fields, &header_size)) {
        return 0;
    }

    /* Read topic content */
    if (obj_buf_len < topic->buffer_len) {
        return 0;
    }
    messagebus_topic_read(topic, obj_buf, topic->buffer_len);

    offset = 2 * MessageSize_size + header_size.bytes;
    max_len = buf_len - (offset);

    if (max_len <= 0) {
        return 0;
    }

    stream = pb_ostream_from_buffer(&buf[offset], max_len);
    if(!pb_encode(&stream, metadata->fields, obj_buf)) {
        return 0;
    }

    msg_size.bytes = stream.bytes_written;
    offset = header_size.bytes + MessageSize_size;
    stream = pb_ostream_from_buffer(&buf[offset], MessageSize_size);
    if(!pb_encode(&stream, MessageSize_fields, &msg_size)) {
        return 0;
    }

    return msg_size.bytes + header_size.bytes + 2 * MessageSize_size;
}


