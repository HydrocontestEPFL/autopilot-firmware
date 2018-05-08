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

TEST_GROUP (ModeTestGroup) {
    commander_t commander;
    RemoteControlInput input;
    void setup()
    {
        input = RemoteControlInput_init_default;
        commander_init(&commander);
    }
};

TEST(ModeTestGroup, FirstSwitchPositionIsManual)
{
    input.control_mode_switch = -0.99;
    commander_update(&commander, input);
    CHECK_FALSE(commander_roll_is_stabilized(&commander));
    CHECK_FALSE(commander_altitude_is_stabilized(&commander));
}

TEST(ModeTestGroup, SecondSwitchPositionIsRollControlledButManualLift)
{
    input.control_mode_switch = 0.;
    commander_update(&commander, input);
    CHECK_TRUE(commander_roll_is_stabilized(&commander));
    CHECK_FALSE(commander_altitude_is_stabilized(&commander));
}

TEST(ModeTestGroup, ThirdSwitchPositionIsFullyAutomatic)
{
    input.control_mode_switch = 0.99;
    commander_update(&commander, input);
    CHECK_TRUE(commander_roll_is_stabilized(&commander));
    CHECK_TRUE(commander_altitude_is_stabilized(&commander));
}
