#include "commander.h"
#include <string.h>
#include <error/error.h>

enum CommanderState {
    COMMANDER_STATE_RESET,
    COMMANDER_STATE_DISARMED,
    COMMANDER_STATE_ARMED,
};

enum ControlMode {
    CONTROL_MODE_MANUAL,
    CONTROL_MODE_STABILIZED,
    CONTROL_MODE_ALTITUDE_HOLD,
};

void commander_init(commander_t *commander)
{
    memset(commander, 0, sizeof(commander_t));
    commander->state = COMMANDER_STATE_RESET;
}

bool commander_is_armed(const commander_t *commander)
{
    return commander->state == COMMANDER_STATE_ARMED;
}

void commander_update(commander_t *commander, RemoteControlInput input)
{
    switch (commander->state) {
        case COMMANDER_STATE_RESET:
            if (!input.arming_request) {
                commander->state = COMMANDER_STATE_DISARMED;
            }
            break;

        case COMMANDER_STATE_DISARMED:
            if (input.arming_request) {
                if (input.throttle >= 0.01) {
                    WARNING("Throttle is not zero, not arming!");
                } else {
                    NOTICE("System armed!");
                    commander->state = COMMANDER_STATE_ARMED;
                }
            }
            break;

        case COMMANDER_STATE_ARMED:
            if (!input.arming_request) {
                NOTICE("System disarmed!");
                commander->state = COMMANDER_STATE_DISARMED;
            }
            break;
    }

    if (input.control_mode_switch < -0.5) {
        commander->control_mode = CONTROL_MODE_MANUAL;
    } else if (input.control_mode_switch > 0.5) {
        commander->control_mode = CONTROL_MODE_ALTITUDE_HOLD;
    } else {
        commander->control_mode = CONTROL_MODE_STABILIZED;
    }
}

bool commander_roll_is_stabilized(commander_t *commander)
{
    return commander->control_mode == CONTROL_MODE_STABILIZED
           || commander->control_mode == CONTROL_MODE_ALTITUDE_HOLD;
}

bool commander_altitude_is_stabilized(commander_t *commander)
{
    return commander->control_mode == CONTROL_MODE_ALTITUDE_HOLD;
}
