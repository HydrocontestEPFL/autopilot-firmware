#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include "commander.h"

TEST_GROUP (ArmingTestGroup) {
    commander_t commander;
    RemoteControlInput input;

    void setup()
    {
        input = RemoteControlInput_init_default;
        commander_init(&commander);
    }
};

TEST (ArmingTestGroup, DisarmedByDefault) {
    CHECK_FALSE(commander_is_armed(&commander));
}

TEST (ArmingTestGroup, CanArmCommanderOnRisingEdge) {
    commander_update(&commander, input);
    input.arming_request = true;
    commander_update(&commander, input);

    CHECK_TRUE(commander_is_armed(&commander));
}

TEST (ArmingTestGroup, ResettingWithArmingSwitchOnDoesNotArmTheSystem) {
    input.arming_request = true;
    commander_update(&commander, input);
    CHECK_FALSE(commander_is_armed(&commander));
}

TEST (ArmingTestGroup, CanDisarm) {
    commander_update(&commander, input);
    input.arming_request = true;
    commander_update(&commander, input);
    input.arming_request = false;
    commander_update(&commander, input);
    CHECK_FALSE(commander_is_armed(&commander));
}

TEST (ArmingTestGroup, RefuseArmingIfThrottleIsHigh) {
    commander_update(&commander, input);
    input.throttle = 0.5;
    input.arming_request = true;
    commander_update(&commander, input);
    CHECK_FALSE(commander_is_armed(&commander));
}
