#ifndef OUTPUT_MIXER_H
#define OUTPUT_MIXER_H

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct {
    float throttle;
    float roll;
    float lift;
    float rudder;
} InputMessage;

typedef struct {
    float channel[OUTPUTS_CHANNELS];
} OutputMessage;

/** Initializes an output mixer and declares its parameters in the given namespace. */
void output_mixer_init(output_mixer_t *mixer, parameter_namespace_t *ns);

/** Mixes the inputs and puts them in the output. */
void output_mixer_mix(output_mixer_t *mixer, const InputMessage *in, OutputMessage *out);

#ifdef __cplusplus
}
#endif
#endif
