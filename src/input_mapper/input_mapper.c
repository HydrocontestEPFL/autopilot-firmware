#include "input_mapper.h"

void input_mapper_init(input_mapper_t *mapper, parameter_namespace_t *ns)
{
    const int default_max = 1000;
    parameter_namespace_declare(&mapper->params.throttle.ns, ns, "throttle");
    parameter_integer_declare_with_default(&mapper->params.throttle.min,
                                           &mapper->params.throttle.ns,
                                           "min",
                                           0);
    parameter_integer_declare_with_default(&mapper->params.throttle.max,
                                           &mapper->params.throttle.ns,
                                           "max",
                                           default_max);
    parameter_namespace_declare(&mapper->params.roll.ns, ns, "roll");
    parameter_integer_declare_with_default(&mapper->params.roll.min, &mapper->params.roll.ns, "min", 0);
    parameter_integer_declare_with_default(&mapper->params.roll.max,
                                           &mapper->params.roll.ns,
                                           "max",
                                           default_max);
    parameter_namespace_declare(&mapper->params.rudder.ns, ns, "rudder");
    parameter_integer_declare_with_default(&mapper->params.rudder.min,
                                           &mapper->params.rudder.ns,
                                           "min",
                                           0);
    parameter_integer_declare_with_default(&mapper->params.rudder.max,
                                           &mapper->params.rudder.ns,
                                           "max",
                                           default_max);
    parameter_namespace_declare(&mapper->params.z.ns, ns, "z");
    parameter_integer_declare_with_default(&mapper->params.z.min, &mapper->params.z.ns, "min", 0);
    parameter_integer_declare_with_default(&mapper->params.z.max,
                                           &mapper->params.z.ns,
                                           "max",
                                           default_max);
    parameter_namespace_declare(&mapper->params.mode_selection.ns, ns, "mode_selection");
    parameter_integer_declare_with_default(&mapper->params.mode_selection.min,
                                           &mapper->params.mode_selection.ns,
                                           "min",
                                           0);
    parameter_integer_declare_with_default(&mapper->params.mode_selection.max,
                                           &mapper->params.mode_selection.ns,
                                           "max",
                                           default_max);

    parameter_namespace_declare(&mapper->params.arming.ns, ns, "arming");
    parameter_integer_declare_with_default(&mapper->params.arming.threshold,
                                           &mapper->params.arming.ns,
                                           "threshold",
                                           default_max / 2);
}

static float interpolate(int value, parameter_t *min_p, parameter_t *max_p)
{
    const float min = parameter_integer_get(min_p);
    const float max = parameter_integer_get(max_p);

    float ret = 2 * value / (max - min);
    ret += (max + min) / (min - max);

    if (ret < -1.) {
        ret = -1;
    } else if (ret > 1.) {
        ret = 1;
    }
    return ret;
}

void input_mapper_map(input_mapper_t *mapper, const SBUSPacket *input, RemoteControlInput *output)
{
    output->roll = interpolate(input->channels[0], &mapper->params.roll.min, &mapper->params.roll.max);
    output->z = interpolate(input->channels[2], &mapper->params.z.min, &mapper->params.z.max);
    output->rudder = interpolate(input->channels[1],
                                 &mapper->params.rudder.min,
                                 &mapper->params.rudder.max);
    output->throttle = interpolate(input->channels[3],
                                   &mapper->params.throttle.min,
                                   &mapper->params.throttle.max);
    output->control_mode_switch = interpolate(input->channels[4],
                                              &mapper->params.mode_selection.min,
                                              &mapper->params.mode_selection.max);

    /* Map the throttle back from -1;1 to 0;1 */
    output->throttle = (output->throttle + 1) / 2;

    output->arming_request = (input->channels[5]
                              > parameter_integer_get(&mapper->params.arming.threshold));
}
