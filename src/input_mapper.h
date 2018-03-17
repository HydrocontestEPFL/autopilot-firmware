#ifndef INPUT_MAPPER_H
#define INPUT_MAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <parameter/parameter.h>
#include <messages/SBUSPacket.pb.h>
#include <messages/RemoteControlInput.pb.h>

/** RC Input mapper
 *
 * This module is responsible for translating raw data coming from the SBUS
 * receiver into a more abstract range, mapping joystick input to the -1;1
 * range.
 */
typedef struct {
    struct {
        struct {
            parameter_t min;
            parameter_t max;
            parameter_namespace_t ns;
        } throttle;
        struct {
            parameter_t min;
            parameter_t max;
            parameter_namespace_t ns;
        } roll, rudder, z;
    } params;
} input_mapper_t;

/** Initializes the given input mapper, declaring its parameters in the given
 * namespace. */
void input_mapper_init(input_mapper_t *mapper, parameter_namespace_t *ns);

/** Maps raw SBUS data to our representation. */
void input_mapper_map(input_mapper_t *mapper, const SBUSPacket *input, RemoteControlInput *output);

#ifdef __cplusplus
}
#endif

#endif
