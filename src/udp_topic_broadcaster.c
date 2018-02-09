#include <ch.h>
#include <lwip/api.h>
#include "main.h"
#include "msgbus_protobuf.h"
#include "udp_topic_broadcaster.h"

static void udp_topic_broadcast_thd(void *p)
{
    (void) p;

    messagebus_topic_t *topic;

    uint8_t object_buf[512];
    uint8_t msg_buf[256];
    size_t encoded_size;

    topic = messagebus_find_topic_blocking(&bus, "time");

    while (true) {
        struct netconn *conn;
        struct netbuf *buf;

        // TODO wait for a group instead
        messagebus_topic_wait(topic, object_buf, sizeof(object_buf));

        encoded_size = messagebus_encode_topic_message(topic,
                                                       msg_buf, sizeof(msg_buf),
                                                       object_buf, sizeof(object_buf));

        if (encoded_size == 0) {
            continue;
        }

        conn = netconn_new(NETCONN_UDP);

        if (conn == NULL) {
            continue;
        }

        buf = netbuf_new();

        if (buf == NULL) {
            netconn_delete(conn);
            continue;
        }

        netbuf_ref(buf, msg_buf, encoded_size);

        // TODO: take those from parameter tree
        ip_addr_t addr;
        LWIP_GATEWAY(&addr);
        const int port = 10000;

        netconn_sendto(conn, buf, &addr, port);

        netbuf_delete(buf);
        netconn_delete(conn);
    }
}

void udp_topic_broadcast_start(void)
{
    static THD_WORKING_AREA(wa, 2048);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, udp_topic_broadcast_thd, NULL);
}
