#include <ch.h>
#include <hal.h>
#include <error/error.h>
#include "msgbus_protobuf.h"
#include "main.h"
#include "commander.h"
#include "commander_thread.h"

#include "messages/RemoteControlInput.pb.h"
#include "messages/ArmingStatus.pb.h"

static void commander_thd(void *p)
{
    (void)p;
    chRegSetThreadName("commander");

    commander_t commander;
    commander_init(&commander);

    static TOPIC_DECL(arming_status, ArmingStatus);
    messagebus_advertise_topic(&bus, &arming_status.topic, "arming_status");

    auto remote_topic = messagebus::find_topic_blocking<RemoteControlInput>(bus, "rc_input");
    messagebus::TopicWrapper<ArmingStatus> arming_topic(&arming_status.topic);
    while (true) {

        auto input = remote_topic.wait();
        commander_update(&commander, input);

        ArmingStatus status = ArmingStatus_init_default;
        status.is_armed = commander_is_armed(&commander);
        status.roll_is_stabilized = commander_roll_is_stabilized(&commander);
        status.altitude_is_stabilized = commander_altitude_is_stabilized(&commander);
        arming_topic.publish(status);

        if (status.is_armed) {
            board_rgb_led_set(0, 100, 0);
        } else {
            board_rgb_led_set(100, 0, 0);
        }
    }
}

void commander_start(void)
{
    static THD_WORKING_AREA(wa, 2048);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, commander_thd, NULL);
}
