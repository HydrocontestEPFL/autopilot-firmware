#include <utility>
#include <vector>
#include <sstream>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include "input_mapper.h"

// TODO: Use real channels
#define CHANNEL_ROLL 0
#define CHANNEL_RUDDER 1
#define CHANNEL_Z 2
#define CHANNEL_THROTTLE 3

TEST_GROUP (InputMapperTestGroup) {
    parameter_namespace_t root;
    input_mapper_t mapper;

    void setup()
    {
        parameter_namespace_declare(&root, nullptr, "");
        input_mapper_init(&mapper, &root);
    }

    void check_parameter_exists(const std::string &name)
    {
        auto res = parameter_find(&root, name.c_str());

        if (!res) {
            std::ostringstream msg;
            msg << "Parameter \"" << name << "\" does not exist";
            FAIL(msg.str().c_str());
        }
    }

    // Hellper function to check that we can interpolate the given axis
    using TestCase = std::pair<int, float>;
    void check_interpolation(std::vector<TestCase> cases,
                             int channel_number,
                             std::function<float(RemoteControlInput)> getter)
    {
        for (const auto &p : cases) {
            SBUSPacket input_msg;
            RemoteControlInput output_msg;
            input_msg.channels[channel_number] = p.first;
            input_mapper_map(&mapper, &input_msg, &output_msg);

            DOUBLES_EQUAL(p.second, getter(output_msg), 0.01);
        }
    }
};

TEST (InputMapperTestGroup, RegisterParameters) {
    check_parameter_exists("/throttle/min");
    check_parameter_exists("/throttle/max");
    check_parameter_exists("/roll/min");
    check_parameter_exists("/roll/max");
    check_parameter_exists("/z/min");
    check_parameter_exists("/z/max");
    check_parameter_exists("/rudder/min");
    check_parameter_exists("/rudder/max");
}

TEST (InputMapperTestGroup, CanMapRollInput) {
    parameter_integer_set(&mapper.params.roll.min, 10);
    parameter_integer_set(&mapper.params.roll.max, 20);

    std::vector<TestCase> expectations({{0, -1}, {10, -1}, {15, 0.}, {20, 1.}, {30, 1.}});

    check_interpolation(expectations, CHANNEL_ROLL, [](auto p) { return p.roll; });
}

TEST (InputMapperTestGroup, CanMapZInput) {
    parameter_integer_set(&mapper.params.z.min, 10);
    parameter_integer_set(&mapper.params.z.max, 20);

    std::vector<TestCase> expectations({{0, -1}, {10, -1}, {15, 0.}, {20, 1.}, {30, 1.}});

    check_interpolation(expectations, CHANNEL_Z, [](auto p) { return p.z; });
}

TEST (InputMapperTestGroup, CanMapRudder) {
    parameter_integer_set(&mapper.params.rudder.min, 10);
    parameter_integer_set(&mapper.params.rudder.max, 20);

    std::vector<TestCase> expectations({{0, -1}, {10, -1}, {15, 0.}, {20, 1.}, {30, 1.}});

    check_interpolation(expectations, CHANNEL_RUDDER, [](auto p) { return p.rudder; });
}

TEST (InputMapperTestGroup, CanMapThrottle) {
    parameter_integer_set(&mapper.params.throttle.min, 10);
    parameter_integer_set(&mapper.params.throttle.max, 20);

    // Throttle is only mapped between 0 and 1
    std::vector<TestCase> expectations({{0, 0.}, {10, 0.}, {15, 0.5}, {20, 1.}, {30, 1.}});

    check_interpolation(expectations, CHANNEL_THROTTLE, [](auto p) { return p.throttle; });
}

TEST(InputMapperTestGroup, CanMapArmingRequest)
{
    parameter_integer_set(&mapper.params.arming.threshold, 20);
    SBUSPacket input;
    RemoteControlInput output;

    input.channels[5] = 10;
    input_mapper_map(&mapper, &input, &output);
    CHECK_FALSE(output.arming_request);
    input.channels[5] = 30;
    input_mapper_map(&mapper, &input, &output);
    CHECK_TRUE(output.arming_request);
}
