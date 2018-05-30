#include <ch.h>
#include <hal.h>
#include "msgbus/messagebus.h"
#include "main.h"
#include "msgbus_protobuf.h"
#include "input_mapper_thread.h"
#include "input_mapper.h"
#include "messages/SBUSPacket.pb.h"
#include "messages/RemoteControlInput.pb.h"

static void input_mapper_thread(void *p)
{
    (void)p;
    static TOPIC_DECL(rcinput, RemoteControlInput);
    static parameter_namespace_t mapper_ns;
    static input_mapper_t mapper;

    messagebus_advertise_topic(&bus, &rcinput.topic, "rc_input");
    parameter_namespace_declare(&mapper_ns, &parameter_root, "input_map");
    input_mapper_init(&mapper, &mapper_ns);

    messagebus::TopicWrapper<RemoteControlInput> rc_input_topic(&rcinput.topic);
    auto sbus_topic = messagebus::find_topic_blocking<SBUSPacket>(bus, "sbus");

    while (1) {
        RemoteControlInput result;
        auto packet = sbus_topic.wait();
        input_mapper_map(&mapper, &packet, &result);
        rc_input_topic.publish(result);
    }
}

void input_mapper_start(void)
{
    static THD_WORKING_AREA(wa, 1024);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, input_mapper_thread, NULL);
}
