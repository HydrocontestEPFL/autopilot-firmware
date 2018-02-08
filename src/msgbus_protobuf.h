#ifndef MSGBUS_PROTOBUF_H
#define MSGBUS_PROTOBUF_H

#define PB_MSGID 1

#include <pb.h>
#include <msgbus/messagebus.h>

#include <unistd.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const pb_field_t *fields;
    uint32_t msgid;
} topic_metadata_t;

#define TOPIC_DECL(name, type) struct { \
        messagebus_topic_t topic; \
        mutex_t lock; \
        condition_variable_t condvar; \
        type value; \
        topic_metadata_t metadata; \
} name = { \
        .lock = _MUTEX_DATA(name.lock), \
        .condvar = _CONDVAR_DATA(name.condvar), \
        .topic = \
            _MESSAGEBUS_TOPIC_DATA(name.topic, name.lock, name.condvar, &name.value, sizeof(type), name.metadata), \
        .metadata = { \
            .fields = type##_fields, \
            .msgid = type##_msgid, \
        }, \
}

#define _MESSAGEBUS_TOPIC_DATA(topic, lock, condvar, buffer, buffer_size, metadata) { \
    buffer, buffer_size, \
    &lock, \
    &condvar, \
    "", \
    0, \
    NULL, \
    NULL, \
    &metadata,  \
}

/* Wraps the topic information in a header (in protobuf format) to be sent over
 * UDP or logged to disk or whatever. */
bool messagebus_encode_topic_message(messagebus_topic_t *topic,
                                     uint8_t *buf, size_t buf_len,
                                     uint8_t *obj_buf, size_t obj_buf_len);

#ifdef __cplusplus
}
#endif
#endif
