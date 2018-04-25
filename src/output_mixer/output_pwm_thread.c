#include <ch.h>
#include <hal.h>
#include "main.h"
#include "msgbus_protobuf.h"
#include "output_pwm_thread.h"
#include "output_mixer.h"

static void output_pwm_thread(void *p)
{
    (void)p;
    static TOPIC_DECL(flight_control, FlightControlInput);
    static output_mixer_t mixer;

    output_mixer_init(&mixer, &parameter_root);
    messagebus_advertise_topic(&bus, &flight_control.topic, "flight_control_inputs");

    while (1) {
        FlightControlInput msg;
        messagebus_topic_wait(&flight_control.topic, &msg, sizeof(msg));

        /* TODO: Actually send it on the timer */
    }
}

void output_pwm_start(void)
{
    static THD_WORKING_AREA(wa, 512);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, output_pwm_thread, NULL);
}
