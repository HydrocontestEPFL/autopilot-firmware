#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <cstdio>

// Mock types, must be before msgbus_protobuf.h
typedef int mutex_t;
typedef int condition_variable_t;
#define _MUTEX_DATA(name) 0
#define _CONDVAR_DATA(name) 0

#include "msgbus_protobuf.h"

// Message types
#include "messages/Timestamp.pb.h"

TEST_GROUP(MessagebusProtobufIntegration)
{
    messagebus_t bus;
    int bus_lock;
    int bus_condvar;

    void setup()
    {
        messagebus_init(&bus, &bus_lock, &bus_condvar);
    }
};

TEST(MessagebusProtobufIntegration, CanCreateTopic)
{
    TOPIC_DECL(mytopic, Timestamp);

    POINTERS_EQUAL(&mytopic.lock, mytopic.topic.lock);
    POINTERS_EQUAL(&mytopic.condvar, mytopic.topic.condvar);
    POINTERS_EQUAL(&mytopic.value, mytopic.topic.buffer);
    CHECK_EQUAL(sizeof(Timestamp), mytopic.topic.buffer_len);

    POINTERS_EQUAL(&mytopic.metadata, mytopic.topic.metadata);
    POINTERS_EQUAL(Timestamp_fields, mytopic.metadata.fields);
}

TEST(MessagebusProtobufIntegration, CanPublishThenEncodeData)
{
    TOPIC_DECL(mytopic, Timestamp);
    messagebus_advertise_topic(&bus, &mytopic.topic, "mytopic");

    Timestamp foo;
    foo.us = 1000;

    messagebus_topic_publish(&mytopic.topic, &foo, sizeof(foo));

    messagebus_topic_t *topic = messagebus_find_topic(&bus, "mytopic");

    // Encode the data using introspection
    uint8_t encoded_buffer[128];
    {
        pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer,
                                                     sizeof(encoded_buffer));

        uint8_t msg_buffer[128];
        CHECK_TRUE(sizeof(msg_buffer) >= topic->buffer_len); // assert
        messagebus_topic_read(topic, msg_buffer, topic->buffer_len);
        topic_metadata_t *metadata = (topic_metadata_t *)topic->metadata;
        auto res = pb_encode(&stream, metadata->fields, msg_buffer);

        CHECK_TRUE(res);

#if 0
        // Write the message in a file to be decoded via protoc --decode
        FILE *out = fopen("example_timestamp.bin", "wb");
        fwrite(encoded_buffer, stream.bytes_written, 1, out);
        fclose(out);
#endif
    }

    // Now decode the stream. We assume the message type was transmitted out of
    // band
    {
        Timestamp message;
        pb_istream_t stream = pb_istream_from_buffer(encoded_buffer,
                                                     Timestamp_size);
        auto status = pb_decode(&stream, Timestamp_fields, &message);

        if (!status) {
            FAIL(PB_GET_ERROR(&stream));
        }

        CHECK_EQUAL(1000, message.us);
    }
}
