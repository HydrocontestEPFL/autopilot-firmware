#include <ch.h>
#include <lwip/api.h>
#include "main.h"
#include "msgbus_protobuf.h"
#include "udp_topic_broadcaster.h"

static messagebus_watchgroup_t watchgroup;
static MUTEX_DECL(watchgroup_lock);
static CONDVAR_DECL(watchgroup_condvar);

static void new_topic_cb(messagebus_t *bus, messagebus_topic_t *topic, void *arg)
{
    (void) bus;
    (void) arg;
    topic_metadata_t *metadata = (topic_metadata_t *)topic->metadata;
    messagebus_watchgroup_watch(&metadata->udp_watcher, &watchgroup, topic);
}

static void udp_topic_broadcast_thd(void *p)
{
    (void) p;

    messagebus_topic_t *topic;

    uint8_t object_buf[512];
    uint8_t msg_buf[256];
    size_t encoded_size;

    while (true) {
        struct netconn *conn;
        struct netbuf *buf;

        topic = messagebus_watchgroup_wait(&watchgroup);

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
    static messagebus_new_topic_cb_t cb;
    messagebus_watchgroup_init(&watchgroup, &watchgroup_lock, &watchgroup_condvar);
    messagebus_new_topic_callback_register(&bus, &cb, new_topic_cb, NULL);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, udp_topic_broadcast_thd, NULL);
}
