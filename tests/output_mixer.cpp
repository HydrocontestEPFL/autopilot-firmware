#include <sstream>
#include <utility>
#include <vector>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include <parameter/parameter.h>

#define OUTPUTS_CHANNELS 4

typedef struct {
    struct {
        parameter_namespace_t outputs_ns;
        struct {
            parameter_namespace_t ns;
            char name[2];
            parameter_t max_ms;    ///< Max output in milisecond (float)
            parameter_t min_ms;    ///< min output in milisecond (float)
            parameter_t center_ms; ///< Output at the neutral position (ms)
            struct {
                parameter_namespace_t ns;
                parameter_t throttle;
                parameter_t roll;
                parameter_t lift;
                parameter_t rudder;
            } gains;
        } outputs[4];
    } params;
} output_mixer_t;

struct InputMessage {
    float throttle;
    float roll;
    float lift;
    float rudder;
};

struct OutputMessage {
    float channel[OUTPUTS_CHANNELS];
};

void output_mixer_init(output_mixer_t *mixer, parameter_namespace_t *ns)
{
    parameter_namespace_declare(&mixer->params.outputs_ns, ns, "outputs");

    for (int i = 0; i < OUTPUTS_CHANNELS; i++) {
        char *name = mixer->params.outputs[i].name;
        name[0] = '1' + i;
        name[1] = '\0';
        parameter_namespace_declare(&mixer->params.outputs[i].ns, &mixer->params.outputs_ns, name);
        parameter_scalar_declare_with_default(&mixer->params.outputs[i].max_ms,
                                              &mixer->params.outputs[i].ns,
                                              "max_ms",
                                              2.);
        parameter_scalar_declare_with_default(&mixer->params.outputs[i].min_ms,
                                              &mixer->params.outputs[i].ns,
                                              "min_ms",
                                              0.);
        parameter_scalar_declare_with_default(&mixer->params.outputs[i].center_ms,
                                              &mixer->params.outputs[i].ns,
                                              "center_ms",
                                              1.);

        parameter_namespace_declare(&mixer->params.outputs[i].gains.ns,
                                    &mixer->params.outputs[i].ns,
                                    "gains");
        parameter_scalar_declare_with_default(&mixer->params.outputs[i].gains.throttle,
                                              &mixer->params.outputs[i].gains.ns,
                                              "throttle",
                                              0.);
        parameter_scalar_declare_with_default(&mixer->params.outputs[i].gains.roll,
                                              &mixer->params.outputs[i].gains.ns,
                                              "roll",
                                              0.);
        parameter_scalar_declare_with_default(&mixer->params.outputs[i].gains.lift,
                                              &mixer->params.outputs[i].gains.ns,
                                              "lift",
                                              0.);
        parameter_scalar_declare_with_default(&mixer->params.outputs[i].gains.rudder,
                                              &mixer->params.outputs[i].gains.ns,
                                              "rudder",
                                              0.);
    }
}

void output_mixer_mix(output_mixer_t *mixer, const InputMessage *in, OutputMessage *out)
{
    for (int i = 0; i < OUTPUTS_CHANNELS; i++) {
        float cmax = parameter_scalar_get(&mixer->params.outputs[i].max_ms);
        float cmin = parameter_scalar_get(&mixer->params.outputs[i].min_ms);
        float ccenter = parameter_scalar_get(&mixer->params.outputs[i].center_ms);

        float val = 0.;
        val += in->throttle * parameter_scalar_get(&mixer->params.outputs[i].gains.throttle);
        val += in->roll * parameter_scalar_get(&mixer->params.outputs[i].gains.roll);
        val += in->lift * parameter_scalar_get(&mixer->params.outputs[i].gains.lift);
        val += in->rudder * parameter_scalar_get(&mixer->params.outputs[i].gains.rudder);

        if (val > 1.) {
            val = 1;
        } else if (val < -1.) {
            val = -1;
        }

        if (val > 0.) {
            out->channel[i] = val * (cmax - ccenter) + ccenter;
        } else {
            out->channel[i] = (1 + val) * (ccenter - cmin) + cmin;
        }
    }
}

TEST_GROUP (OutputMixing) {
    output_mixer_t mixer;
    parameter_namespace_t ns;

    void setup()
    {
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
    InputMessage msg = {};
    OutputMessage out;

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
            output_mixer_mix(&mixer, &msg, &out);
            DOUBLES_EQUAL(c.second, out.channel[i], 0.01);
        }
    }
}

TEST (OutputMixing, RollMixing) {
    InputMessage msg = {};
    OutputMessage out;

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
            output_mixer_mix(&mixer, &msg, &out);
            DOUBLES_EQUAL(c.second, out.channel[i], 0.01);
        }
    }
}

TEST (OutputMixing, Lift) {
    InputMessage msg = {};
    OutputMessage out;

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
            output_mixer_mix(&mixer, &msg, &out);
            DOUBLES_EQUAL(c.second, out.channel[i], 0.01);
        }
    }
}

TEST (OutputMixing, Rudder) {
    InputMessage msg = {};
    OutputMessage out;

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
            output_mixer_mix(&mixer, &msg, &out);
            DOUBLES_EQUAL(c.second, out.channel[i], 0.01);
        }
    }
}

TEST (OutputMixing, RespectsWeight) {
    parameter_scalar_set(&mixer.params.outputs[0].max_ms, 10.);
    parameter_scalar_set(&mixer.params.outputs[0].center_ms, 5.);

    InputMessage msg = {};
    OutputMessage out;

    parameter_scalar_set(&mixer.params.outputs[0].gains.rudder, 1.);
    parameter_scalar_set(&mixer.params.outputs[0].gains.roll,   4.);
    msg.rudder = 0.1;
    msg.roll = 0.1;

    output_mixer_mix(&mixer, &msg, &out);

    DOUBLES_EQUAL(7.5, out.channel[0], 0.01);
}
