#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include <cstring>

#include "telemetry/rpc.h"
#include "messages/RPCParameter.pb.h"
#include <parameter/parameter.h>
#include "telemetry/rpc_parameter_server.h"

TEST_GROUP (RPCParameterEnumerationTestGroup) {
    uint8_t msg[128];
    uint8_t reply[128];

    pb_ostream_t msg_ostream, reply_ostream;
    pb_istream_t msg_istream, reply_istream;

    parameter_namespace_t root;

    struct parameter_server_data data;

    void setup()
    {
        memset(msg, 0, sizeof(msg));
        memset(reply, 0, sizeof(reply));
        data = {&root, nullptr, 0};

        msg_ostream = pb_ostream_from_buffer(msg, sizeof(msg));
        reply_ostream = pb_ostream_from_buffer(reply, sizeof(reply));
        msg_istream = pb_istream_from_buffer(msg, sizeof(msg));
        reply_istream = pb_istream_from_buffer(reply, sizeof(reply));

        parameter_namespace_declare(&root, NULL, "");
    }

    void enumerate_param(int index, ParameterEnumerationReply *reply)
    {
        // Generate a request for parameter 1, which should be /sub/p
        ParameterEnumerationRequest req;
        req.index = index;
        pb_encode(&msg_ostream, ParameterEnumerationRequest_fields, &req);

        // Enumerate parameter 0
        parameter_server_enumerate(&data, &msg_istream, &reply_ostream);

        // Read the reply
        pb_decode(&reply_istream, ParameterEnumerationReply_fields, reply);
    }
};

TEST (RPCParameterEnumerationTestGroup, CanEnumerateNonNestedThings) {
    // Declare a few parametes
    parameter_t foo, bar;
    parameter_scalar_declare_with_default(&foo, &root, "foo", 13);
    parameter_scalar_declare_with_default(&bar, &root, "bar", 42);

    ParameterEnumerationReply reply;
    enumerate_param(0, &reply);

    // Check that all fields are correct
    CHECK_TRUE(reply.has_parameter);
    STRCMP_EQUAL("/bar", reply.parameter.name);
    CHECK_EQUAL(Parameter_scalar_value_tag, reply.parameter.which_value);
    CHECK_EQUAL(42.0, reply.parameter.value.scalar_value);
}

TEST (RPCParameterEnumerationTestGroup, EnumerateNameSpace) {
    parameter_namespace_t sub_ns;
    parameter_namespace_declare(&sub_ns, &root, "sub");
    parameter_t param1, param2;
    parameter_scalar_declare_with_default(&param1, &sub_ns, "p", 42);
    parameter_scalar_declare_with_default(&param2, &root, "q", 43);

    // Get parameter 1 (/sub/p)
    ParameterEnumerationReply reply;
    enumerate_param(1, &reply);

    // Check that all fields are correct
    CHECK_TRUE(reply.has_parameter);
    STRCMP_EQUAL("/sub/p", reply.parameter.name);
    CHECK_EQUAL(Parameter_scalar_value_tag, reply.parameter.which_value);
}

TEST (RPCParameterEnumerationTestGroup, AskPastTheLastParameter) {
    parameter_t param1, param2;
    parameter_scalar_declare(&param1, &root, "p");
    parameter_scalar_declare(&param2, &root, "q");

    // Get parameter 2 (after the last parameter)
    ParameterEnumerationReply reply;
    enumerate_param(2, &reply);

    CHECK_FALSE(reply.has_parameter);
}

TEST (RPCParameterEnumerationTestGroup, IntegerParameter) {
    parameter_t param;
    parameter_integer_declare_with_default(&param, &root, "p", 42);

    ParameterEnumerationReply reply;
    enumerate_param(0, &reply);
    CHECK_EQUAL(Parameter_int_value_tag, reply.parameter.which_value);
    CHECK_EQUAL(42, reply.parameter.value.int_value);
}

TEST (RPCParameterEnumerationTestGroup, ) {
    parameter_t param;
    parameter_boolean_declare_with_default(&param, &root, "p", true);

    ParameterEnumerationReply reply;
    enumerate_param(0, &reply);
    CHECK_EQUAL(Parameter_bool_value_tag, reply.parameter.which_value);
}

TEST_GROUP (ParameterSetRPCTestGroup) {
    parameter_namespace_t ns;
    parameter_t param;
    uint8_t msg[128];
    uint8_t reply[128];

    pb_ostream_t msg_ostream, reply_ostream;
    pb_istream_t msg_istream, reply_istream;

    struct parameter_server_data server_data = {&ns, nullptr, 0};

    void setup()
    {
        parameter_namespace_declare(&ns, nullptr, "");

        memset(msg, 0, sizeof(msg));
        memset(reply, 0, sizeof(reply));

        msg_ostream = pb_ostream_from_buffer(msg, sizeof(msg));
        reply_ostream = pb_ostream_from_buffer(reply, sizeof(reply));
        msg_istream = pb_istream_from_buffer(msg, sizeof(msg));
        reply_istream = pb_istream_from_buffer(reply, sizeof(reply));
    }
};

TEST (ParameterSetRPCTestGroup, SetInteger) {
    parameter_integer_declare_with_default(&param, &ns, "param", 12);

    /* Craft the request */
    ParameterSetRequest request = ParameterSetRequest_init_default;
    strcpy(request.parameter.name, "/param");
    request.parameter.which_value = Parameter_int_value_tag;
    request.parameter.value.int_value = 42;
    pb_encode(&msg_ostream, ParameterSetRequest_fields, &request);

    /* Execute it */
    parameter_server_set(&server_data, &msg_istream, &reply_ostream);

    CHECK_EQUAL(42, parameter_integer_get(&param));
}

TEST (ParameterSetRPCTestGroup, SetScalar) {
    parameter_scalar_declare_with_default(&param, &ns, "param", 12);

    /* Craft the request */
    ParameterSetRequest request = ParameterSetRequest_init_default;
    strcpy(request.parameter.name, "/param");
    request.parameter.which_value = Parameter_scalar_value_tag;
    request.parameter.value.scalar_value = 42;
    pb_encode(&msg_ostream, ParameterSetRequest_fields, &request);

    /* Execute it */
    parameter_server_set(&server_data, &msg_istream, &reply_ostream);

    CHECK_EQUAL(42, parameter_scalar_get(&param));
}

TEST (ParameterSetRPCTestGroup, SetBoolean) {
    parameter_boolean_declare_with_default(&param, &ns, "param", false);

    /* Craft the request */
    ParameterSetRequest request = ParameterSetRequest_init_default;
    strcpy(request.parameter.name, "/param");
    request.parameter.which_value = Parameter_bool_value_tag;
    request.parameter.value.bool_value = true;
    pb_encode(&msg_ostream, ParameterSetRequest_fields, &request);

    /* Execute it */
    parameter_server_set(&server_data, &msg_istream, &reply_ostream);

    /* Check that the parameter changed */
    CHECK_TRUE(parameter_boolean_get(&param));
}

TEST (ParameterSetRPCTestGroup, CanSave) {
    /* Setup flash for testing */
    mock("flash").expectOneCall("unlock");
    mock("flash").ignoreOtherCalls();

    uint8_t flash[128] = {0};
    server_data.config_start = flash;
    server_data.config_end = &flash[127];

    parameter_boolean_declare_with_default(&param, &ns, "param", false);

    /* Craft the request */
    ParameterSetRequest request = ParameterSetRequest_init_default;
    strcpy(request.parameter.name, "/param");
    request.parameter.which_value = Parameter_bool_value_tag;
    request.parameter.value.bool_value = true;
    request.save_to_flash = true;

    pb_encode(&msg_ostream, ParameterSetRequest_fields, &request);

    /* Execute it */
    parameter_server_set(&server_data, &msg_istream, &reply_ostream);

    /* Check that the parameter changed */
    CHECK_TRUE(parameter_boolean_get(&param));

    /* Check that the flash was changed as well */
    CHECK_TRUE(flash[0] != 0);
}

TEST (ParameterSetRPCTestGroup, NoError) {
    parameter_boolean_declare_with_default(&param, &ns, "param", false);

    /* Craft the request */
    ParameterSetRequest request = ParameterSetRequest_init_default;
    strcpy(request.parameter.name, "/param");
    request.parameter.which_value = Parameter_bool_value_tag;
    request.parameter.value.bool_value = true;
    pb_encode(&msg_ostream, ParameterSetRequest_fields, &request);

    /* Execute it */
    parameter_server_set(&server_data, &msg_istream, &reply_ostream);

    /* Decode the answer */
    ParameterSetReply reply;
    auto status = pb_decode(&reply_istream, ParameterSetReply_fields, &reply);

    CHECK_TRUE(status);
    CHECK_FALSE(reply.has_error);
}

TEST (ParameterSetRPCTestGroup, UnknownParameter) {
    parameter_boolean_declare_with_default(&param, &ns, "param", false);

    /* Craft the request for a non existent parameter */
    ParameterSetRequest request = ParameterSetRequest_init_default;
    strcpy(request.parameter.name, "/bad_param");
    request.parameter.which_value = Parameter_bool_value_tag;
    request.parameter.value.bool_value = true;
    pb_encode(&msg_ostream, ParameterSetRequest_fields, &request);

    /* Execute it */
    parameter_server_set(&server_data, &msg_istream, &reply_ostream);

    /* Decode the answer */
    ParameterSetReply reply;
    auto status = pb_decode(&reply_istream, ParameterSetReply_fields, &reply);

    CHECK_TRUE(status);
    CHECK_TRUE(reply.has_error);
    CHECK_EQUAL(ParameterSetReply_Error_NOT_FOUND, reply.error);
}

TEST (ParameterSetRPCTestGroup, WrongTypeForBoolean) {
    parameter_boolean_declare_with_default(&param, &ns, "param", false);

    /* Craft the request for a non existent parameter */
    ParameterSetRequest request = ParameterSetRequest_init_default;
    strcpy(request.parameter.name, "/param");
    request.parameter.which_value = Parameter_int_value_tag;
    request.parameter.value.int_value = 42;
    pb_encode(&msg_ostream, ParameterSetRequest_fields, &request);

    /* Execute it */
    parameter_server_set(&server_data, &msg_istream, &reply_ostream);

    /* Decode the answer */
    ParameterSetReply reply;
    auto status = pb_decode(&reply_istream, ParameterSetReply_fields, &reply);

    CHECK_TRUE(status);
    CHECK_TRUE(reply.has_error);
    CHECK_EQUAL(ParameterSetReply_Error_WRONG_TYPE, reply.error);
}
