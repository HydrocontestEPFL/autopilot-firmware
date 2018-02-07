#ifndef MSGBUS_PROTOBUF_H
#define MSGBUS_PROTOBUF_H

#include <pb.h>
#include <msgbus/messagebus.h>

typedef struct {
    const pb_field_t *fields;
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
        }, \
}

#define _MESSAGEBUS_TOPIC_DATA(topic, lock, condvar, buffer, buffer_size, metadata) { \
    buffer, buffer_size, \
    &lock, \
    &condvar, \
    "", \
    false, \
    NULL, \
    NULL, \
    &metadata,  \
}


#endif
