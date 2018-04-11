#include <ch.h>
#include <hal.h>
#include "main.h"
#include "state_estimation_thread.h"
#include "madgwick/madgwick.h"
#include "messages/IMUReading.pb.h"

// TODO: Check the frequency
#define FREQUENCY 250

void state_estimation_thd(void *p)
{
    (void)p;
    madgwick_filter_t filter;
    madgwick_filter_init(&filter);

    messagebus_topic_t *topic;

    topic = messagebus_find_topic_blocking(&bus, "imu0");

    madgwick_filter_set_sample_frequency(&filter, FREQUENCY);
    // TODO: Take those from parameter tree
    madgwick_filter_set_gain(0.1);

    while (1) {
        IMUReading msg;
        messagebus_topic_wait(topic, &msg, sizeof(msg));

        madgwick_filter_updateIMU(&filter,
                                  msg.angular_speed.x, msg.angular_speed.y, msg.angular_speed.z
    }
}

void state_estimation_start(void)
{
    static THD_WORKING_AREA(wa, 1024);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, state_estimation_thd, NULL);
}
