#include <cstring>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <pb_encode.h>
#include <pb_decode.h>

#include "messages/RemoteProcedureCall.pb.h"
#include "messages/MessageSize.pb.h"
#include "messages/Timestamp.pb.h"
#include "rpc.h"

TEST_GROUP(RemoteProcedureCallTestGroup)
{
    uint8_t input_buffer[128];
    uint8_t output_buffer[128];

    void setup()
    {
        memset(input_buffer, 0, sizeof(input_buffer));
        memset(output_buffer, 0, sizeof(output_buffer));
    }

    void prepare_buffer(const char *name, uint32_t us)
    {
        RPCRequestHeader header;
        MessageSize header_size;
        strcpy(header.name, name);
        size_t offset;

        pb_ostream_t ostream;
        // Encode message header
        offset = MessageSize_size;
        ostream = pb_ostream_from_buffer(&input_buffer[offset], sizeof(input_buffer) - offset);
        pb_encode(&ostream, RPCRequestHeader_fields, &header);

        offset = 0;
        header_size.bytes = ostream.bytes_written;
        ostream = pb_ostream_from_buffer(&input_buffer[offset], sizeof(input_buffer) - offset);
        pb_encode(&ostream, MessageSize_fields, &header_size);

        // Encode message data
        Timestamp data;
        data.us = us;
        offset = 2 * MessageSize_size + header_size.bytes;
        ostream = pb_ostream_from_buffer(&input_buffer[offset], sizeof(input_buffer) - offset);
        pb_encode(&ostream, Timestamp_fields, &data);

        // Encode message data size
        MessageSize msg_size;
        msg_size.bytes = ostream.bytes_written;
        offset = MessageSize_size + header_size.bytes;
        ostream = pb_ostream_from_buffer(&input_buffer[offset], sizeof(input_buffer) - offset);
        pb_encode(&ostream, MessageSize_fields, &msg_size);
    }

    size_t test_rpc_process(rpc_callback_t *callbacks, int callback_len)
    {
        return rpc_process(callbacks, callback_len,
                           input_buffer, sizeof(input_buffer),
                           output_buffer, sizeof(output_buffer));
    }
};

static void myrpc(void *p, pb_istream_t *input, pb_ostream_t *output)
{
    (void) output;

    Timestamp ts;
    pb_decode(input, Timestamp_fields, &ts);
    mock().actualCall("myrpc").withParameter("us", ts.us).withParameter("p", p);
}

TEST(RemoteProcedureCallTestGroup, CallsCorrectMethod)
{
    rpc_callback_t callbacks[] = {
        {"invalid_method", nullptr, nullptr},
        {"myrpc", myrpc, (void *)0x1234},
    };

    prepare_buffer("myrpc", 1000);
    mock().expectOneCall("myrpc").withParameter("us", 1000).withPointerParameter("p", (void *)0x1234);

    test_rpc_process(callbacks, 2);
}

static void increment(void *p, pb_istream_t *input, pb_ostream_t *output)
{
    (void) p;
    Timestamp ts;
    pb_decode(input, Timestamp_fields, &ts);

    ts.us += 1;

    pb_encode(output, Timestamp_fields, &ts);
}

TEST(RemoteProcedureCallTestGroup, CanParseAnswer)
{
    rpc_callback_t callbacks[] = {
        {"increment", increment, nullptr},
    };

    prepare_buffer("increment", 1000);
    test_rpc_process(callbacks, 1);

    pb_istream_t istream;
    size_t offset;
    size_t max_size;
    bool status;

    MessageSize reply_size;
    Timestamp reply;

    // Decode reply size
    offset = 0;
    max_size = MessageSize_size;
    istream = pb_istream_from_buffer(&output_buffer[offset], max_size);

    status = pb_decode(&istream, MessageSize_fields, &reply_size);
    CHECK_TRUE(status);

    // Decode reply itself
    offset += MessageSize_size;
    max_size = reply_size.bytes;
    istream = pb_istream_from_buffer(&output_buffer[offset], max_size);
    status = pb_decode(&istream, Timestamp_fields, &reply);
    CHECK_TRUE(status);
    CHECK_EQUAL(1001, reply.us);
}

TEST(RemoteProcedureCallTestGroup, ReturnsWrittenSize)
{
    rpc_callback_t callbacks[] = {
        {"increment", increment, nullptr},
    };

    prepare_buffer("increment", 1000);

    auto size = test_rpc_process(callbacks, 1);
    CHECK_EQUAL(8, size);
}

TEST(RemoteProcedureCallTestGroup, CallbackNotFound)
{
    rpc_callback_t callbacks[] = {
        {"increment", increment, nullptr},
    };

    prepare_buffer("invalid", 1000);

    auto size = test_rpc_process(callbacks, 1);
    CHECK_EQUAL(5, size); // means we only have the reply size
}

TEST(RemoteProcedureCallTestGroup, IncompleteWhenSizeIsNotThere)
{
    prepare_buffer("foo", 0);
    CHECK_FALSE(rpc_buffer_is_complete(input_buffer, 2));
}

TEST(RemoteProcedureCallTestGroup, IncompleteWhenOnlyMessageSizeAreThere)
{
    prepare_buffer("foo", 0);
    CHECK_FALSE(rpc_buffer_is_complete(input_buffer, 10));
}

TEST(RemoteProcedureCallTestGroup, IncompleteWhenMessageIsPartial)
{
    prepare_buffer("foo", 0);
    CHECK_FALSE(rpc_buffer_is_complete(input_buffer, 15));
}

TEST(RemoteProcedureCallTestGroup, Complete)
{
    prepare_buffer("foo", 0);
    CHECK_TRUE(rpc_buffer_is_complete(input_buffer, 100));
}
