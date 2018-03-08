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
} commander_t;

/** Initialize the commander module. */
void commander_init(commander_t *commander);

/** Returns true if the actuators are allowed to move. */
bool commander_is_armed(const commander_t *commander);

/** Feeds the remote control input to the commander, which will use it to
 * update the arming status. */
void commander_update(commander_t *commander, RemoteControlInput input);

#ifdef __cplusplus
}
#endif

#endif
