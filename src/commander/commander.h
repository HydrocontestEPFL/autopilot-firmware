/** This modules is responsible for handling the overall state of the boat. It
 * was named "commander", in reference to the subsystem of the same name in the
 * pixracer.
 *
 * Its first and most important responsibility is safety: it is responsible for
 * arming and disarming the actuators on the boat.
 */
#ifndef COMMANDER_H
#define COMMANDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "messages/RemoteControlInput.pb.h"

typedef struct {
    int state;
    int control_mode;
} commander_t;

/** Initialize the commander module. */
void commander_init(commander_t *commander);

/** Returns true if the actuators are allowed to move. */
bool commander_is_armed(const commander_t *commander);

/** Feeds the remote control input to the commander, which will use it to
 * update the arming status. */
void commander_update(commander_t *commander, RemoteControlInput input);

/** Checks if the boat must be automatically stabilized in roll in this mode. */
bool commander_roll_is_stabilized(commander_t *commander);

/** Checks if the boat must be automatically stabilized in altitude in this
 * mode. */
bool commander_altitude_is_stabilized(commander_t *commander);

#ifdef __cplusplus
}
#endif

#endif
