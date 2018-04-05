#include <sstream>
#include <utility>
#include <vector>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include "output_mixer.h"
#include <parameter/parameter.h>

TEST_GROUP (OutputMixing) {
    output_mixer_t mixer;
    parameter_namespace_t ns;
    InputMessage msg;
    float out[4];

    void setup()
    {
        msg = {};
        for (auto i = 0; i < OUTPUTS_CHANNELS; i++) {
            out[i] = 0.;
        }
        parameter_namespace_declare(&ns, nullptr, "");
        output_mixer_init(&mixer, &ns);
    }

    void assert_param_exists(const char *name)
    {
        auto p = parameter_find(&ns, name);
        if (!p) {
            std::stringstream msg;
            msg << "Cannot find parameter \"" << name << "\".";
            FAIL(msg.str().c_str());
        }

        if (!p->defined) {
            std::stringstream msg;
            msg << "\"" << name << "\" does not have a value.";
            FAIL(msg.str().c_str());
        }
    }
};

TEST (OutputMixing, CreateParameters) {
    assert_param_exists("/outputs/1/max_ms");
    assert_param_exists("/outputs/1/min_ms");
    assert_param_exists("/outputs/1/center_ms");
    assert_param_exists("/outputs/1/gains/throttle");
    assert_param_exists("/outputs/1/gains/roll");
    assert_param_exists("/outputs/1/gains/lift");
    assert_param_exists("/outputs/1/gains/rudder");

    // Don't repeat all parameters for all outputs
    assert_param_exists("/outputs/2/max_ms");
    assert_param_exists("/outputs/3/max_ms");
    assert_param_exists("/outputs/4/max_ms");
}

TEST (OutputMixing, ThrottleMixing) {
    std::vector<std::pair<float, float>> test_cases{{1.1, 10},
                                                    {0.5, 7.5},
                                                    {0., 5},
                                                    {-0.5, 2.5},
                                                    {-1.1, 0.}};

    for (auto i = 0; i < OUTPUTS_CHANNELS; i++) {
        parameter_scalar_set(&mixer.params.outputs[i].gains.throttle, 1.);
        parameter_scalar_set(&mixer.params.outputs[i].max_ms, 10.);
        parameter_scalar_set(&mixer.params.outputs[i].center_ms, 5.);
        parameter_scalar_set(&mixer.params.outputs[i].min_ms, 0.);

        for (const auto &c : test_cases) {
            msg.throttle = c.first;
            output_mixer_mix(&mixer, &msg, out);
            DOUBLES_EQUAL(c.second, out[i], 0.01);
        }
    }
}

TEST (OutputMixing, RollMixing) {
    std::vector<std::pair<float, float>> test_cases{{1.1, 10},
                                                    {0.5, 7.5},
                                                    {0., 5},
                                                    {-0.5, 2.5},
                                                    {-1.1, 0.}};

    for (auto i = 0; i < OUTPUTS_CHANNELS; i++) {
        parameter_scalar_set(&mixer.params.outputs[i].gains.roll, 1.);
        parameter_scalar_set(&mixer.params.outputs[i].max_ms, 10.);
        parameter_scalar_set(&mixer.params.outputs[i].center_ms, 5.);
        parameter_scalar_set(&mixer.params.outputs[i].min_ms, 0.);

        for (const auto &c : test_cases) {
            msg.roll = c.first;
            output_mixer_mix(&mixer, &msg, out);
            DOUBLES_EQUAL(c.second, out[i], 0.01);
        }
    }
}

TEST (OutputMixing, Lift) {
    std::vector<std::pair<float, float>> test_cases{{1.1, 10},
                                                    {0.5, 7.5},
                                                    {0., 5},
                                                    {-0.5, 2.5},
                                                    {-1.1, 0.}};

    for (auto i = 0; i < OUTPUTS_CHANNELS; i++) {
        parameter_scalar_set(&mixer.params.outputs[i].gains.lift, 1.);
        parameter_scalar_set(&mixer.params.outputs[i].max_ms, 10.);
        parameter_scalar_set(&mixer.params.outputs[i].center_ms, 5.);
        parameter_scalar_set(&mixer.params.outputs[i].min_ms, 0.);

        for (const auto &c : test_cases) {
            msg.lift = c.first;
            output_mixer_mix(&mixer, &msg, out);
            DOUBLES_EQUAL(c.second, out[i], 0.01);
        }
    }
}

TEST (OutputMixing, Rudder) {
    std::vector<std::pair<float, float>> test_cases{{1.1, 10},
                                                    {0.5, 7.5},
                                                    {0., 5},
                                                    {-0.5, 2.5},
                                                    {-1.1, 0.}};

    for (auto i = 0; i < OUTPUTS_CHANNELS; i++) {
        parameter_scalar_set(&mixer.params.outputs[i].gains.rudder, 1.);
        parameter_scalar_set(&mixer.params.outputs[i].max_ms, 10.);
        parameter_scalar_set(&mixer.params.outputs[i].center_ms, 5.);
        parameter_scalar_set(&mixer.params.outputs[i].min_ms, 0.);

        for (const auto &c : test_cases) {
            msg.rudder = c.first;
            output_mixer_mix(&mixer, &msg, out);
            DOUBLES_EQUAL(c.second, out[i], 0.01);
        }
    }
}

TEST (OutputMixing, RespectsWeight) {
    parameter_scalar_set(&mixer.params.outputs[0].max_ms, 10.);
    parameter_scalar_set(&mixer.params.outputs[0].center_ms, 5.);

    parameter_scalar_set(&mixer.params.outputs[0].gains.rudder, 1.);
    parameter_scalar_set(&mixer.params.outputs[0].gains.roll, 4.);
    msg.rudder = 0.1;
    msg.roll = 0.1;

    output_mixer_mix(&mixer, &msg, out);

    DOUBLES_EQUAL(7.5, out[0], 0.01);
}
