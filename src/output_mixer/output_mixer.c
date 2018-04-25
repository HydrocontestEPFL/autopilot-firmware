#include "output_mixer.h"

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

void output_mixer_mix(output_mixer_t *mixer,
                      const FlightControlInput *in,
                      float output[OUTPUTS_CHANNELS])
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
            output[i] = val * (cmax - ccenter) + ccenter;
        } else {
            output[i] = (1 + val) * (ccenter - cmin) + cmin;
        }
    }
}
