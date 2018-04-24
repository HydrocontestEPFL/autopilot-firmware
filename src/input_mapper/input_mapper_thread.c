#include <ch.h>
#include <hal.h>
#include "main.h"
#include "msgbus_protobuf.h"
#include "input_mapper_thread.h"
#include "input_mapper.h"
#include "messages/SBUSPacket.pb.h"
#include "messages/RemoteControlInput.pb.h"

static void input_mapper_thread(void *p)
{
    (void)p;
    messagebus_topic_t *sbus_topic;
    static TOPIC_DECL(rcinput, RemoteControlInput);
    static parameter_namespace_t mapper_ns;
    static input_mapper_t mapper;

    messagebus_advertise_topic(&bus, &rcinput.topic, "rc_input");
    parameter_namespace_declare(&mapper_ns, &parameter_root, "input_map");
    input_mapper_init(&mapper, &mapper_ns);

    sbus_topic = messagebus_find_topic_blocking(&bus, "sbus");

    while (1) {
        SBUSPacket packet;
        RemoteControlInput result;
        messagebus_topic_wait(sbus_topic, &packet, sizeof(SBUSPacket));
        input_mapper_map(&mapper, &packet, &result);
        messagebus_topic_publish(&rcinput.topic, &result, sizeof(result));
    }
}

void input_mapper_start(void)
{
    static THD_WORKING_AREA(wa, 1024);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, input_mapper_thread, NULL);
}
