#ifndef OUTPUT_MIXER_H
#define OUTPUT_MIXER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <parameter/parameter.h>
#include "messages/FlightControlInput.pb.h"

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
        } outputs[OUTPUTS_CHANNELS];
    } params;
} output_mixer_t;

/** Initializes an output mixer and declares its parameters in the given namespace. */
void output_mixer_init(output_mixer_t *mixer, parameter_namespace_t *ns);

/** Mixes the inputs and puts them in the output. */
void output_mixer_mix(output_mixer_t *mixer,
                      const FlightControlInput *in,
                      float output[OUTPUTS_CHANNELS]);

#ifdef __cplusplus
}
#endif
#endif
