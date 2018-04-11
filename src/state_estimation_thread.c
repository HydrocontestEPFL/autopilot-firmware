#include <ch.h>
#include <hal.h>
#include <error/error.h>
#include "main.h"
#include "state_estimation_thread.h"
#include "madgwick/madgwick.h"
#include "msgbus_protobuf.h"
#include "messages/IMUReading.pb.h"
#include "messages/BoatState.pb.h"

// TODO: Check the frequency
#define FREQUENCY 250

void state_estimation_thd(void *p)
{
    (void)p;

    static TOPIC_DECL(state, BoatState);

    static struct {
        parameter_namespace_t ns;
        parameter_t beta;
    } params;

    chRegSetThreadName(__FUNCTION__);
    madgwick_filter_t filter;
    madgwick_filter_init(&filter);

    parameter_namespace_declare(&params.ns, &parameter_root, "ahrs");
    parameter_scalar_declare_with_default(&params.beta, &params.ns, "beta", 0.5);

    messagebus_topic_t *imu_topic;

    imu_topic = messagebus_find_topic_blocking(&bus, "imu0");

    messagebus_advertise_topic(&bus, &state.topic, "state");

    madgwick_filter_set_sample_frequency(&filter, FREQUENCY);

    while (1) {
        IMUReading msg;
        messagebus_topic_wait(imu_topic, &msg, sizeof(msg));
        madgwick_filter_set_gain(&filter, parameter_scalar_get(&params.beta));

        DEBUG("Updating madgwick filter...");
        madgwick_filter_updateIMU(&filter,
                                  msg.angular_speed.x,
                                  msg.angular_speed.y,
                                  msg.angular_speed.z,
                                  msg.acceleration.x,
                                  msg.acceleration.y,
                                  msg.acceleration.z);

        BoatState new_state;
        new_state.orientation.w = filter.q[0];
        new_state.orientation.x = filter.q[1];
        new_state.orientation.y = filter.q[2];
        new_state.orientation.z = filter.q[3];

        messagebus_topic_publish(&state.topic, &new_state, sizeof(new_state));
    }
}

void state_estimation_start(void)
{
    static THD_WORKING_AREA(wa, 1024);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, state_estimation_thd, NULL);
}
