#include <ch.h>
#include <hal.h>
#include "main.h"
#include "msgbus_protobuf.h"
#include "messages/SBUSPacket.pb.h"

static void sbus_thread(void *p)
{
    BaseSequentialStream *stream = (BaseSequentialStream *)p;

    static TOPIC_DECL(rcinput, SBUSPacket);
    messagebus_advertise_topic(&bus, &rcinput.topic, "sbus");

    while (true) {
        /* TODO: Actually parse the topic */
        (void)stream;

        chThdSleepMilliseconds(1000);
    }
}

void sbus_start(BaseSequentialStream *stream)
{
    static THD_WORKING_AREA(wa, 1024);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, sbus_thread, stream);
}
