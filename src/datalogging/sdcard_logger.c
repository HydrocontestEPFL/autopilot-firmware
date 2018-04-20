#include <stdio.h>
#include <ch.h>
#include "main.h"
#include "error/error.h"
#include "msgbus_protobuf.h"
#include "sdcard_logger.h"
#include "fatfs/ff.h"

static messagebus_watchgroup_t watchgroup;
static MUTEX_DECL(watchgroup_lock);
static CONDVAR_DECL(watchgroup_condvar);

#define MSG_MAX_LENGTH 128
#define MSG_BUF_SIZE 16

struct encoded_message {
    uint16_t len;
    uint8_t buf[MSG_MAX_LENGTH];
};

static struct encoded_message msg_buffer[MSG_BUF_SIZE];
static char *msg_mailbox_buf[MSG_BUF_SIZE];
static MAILBOX_DECL(msg_mailbox, msg_mailbox_buf, MSG_BUF_SIZE);
static MEMORYPOOL_DECL(msg_pool, sizeof(struct encoded_message), NULL);

static void new_topic_cb(messagebus_t *bus, messagebus_topic_t *topic, void *arg)
{
    (void)bus;
    (void)arg;
    topic_metadata_t *metadata = (topic_metadata_t *)topic->metadata;
    messagebus_watchgroup_watch(&metadata->sdcard_watcher, &watchgroup, topic);
}

static void sdcard_topic_encode_thd(void *p)
{
    (void)p;

    messagebus_topic_t *topic;

    uint8_t object_buf[512];

    /* We need to have max priority to avoid skipping messages */
    chThdSetPriority(HIGHPRIO);
    chRegSetThreadName(__FUNCTION__);

    while (true) {
        topic = messagebus_watchgroup_wait(&watchgroup);

        struct encoded_message *msg = chPoolAlloc(&msg_pool);

        if (msg == NULL) {
            WARNING("Dropping a messsage.");
            continue;
        }

        msg->len = messagebus_encode_topic_message(topic,
                                                   msg->buf,
                                                   MSG_MAX_LENGTH,
                                                   object_buf,
                                                   sizeof(object_buf));

        if (msg->len > 0) {
            chMBPost(&msg_mailbox, (msg_t)msg, TIME_IMMEDIATE);
        } else {
            chPoolFree(&msg_pool, msg);
        }
    }
}

static void sdcard_topic_write_thd(void *p)
{
    chThdSetPriority(NORMALPRIO);
    chRegSetThreadName(__FUNCTION__);

    FIL *f = (FIL *)p;

    while (true) {
        struct encoded_message *msg;

        msg_t res = chMBFetch(&msg_mailbox, (msg_t *)&msg, TIME_INFINITE);

        if (res != MSG_OK) {
            continue;
        }

        UINT byte_written;

        f_write(f, msg->buf, msg->len, &byte_written);

        if (byte_written != msg->len) {
            WARNING("Could not write message to disk");
        }

        chPoolFree(&msg_pool, msg);
    }
}

void sdcard_logger_start(void)
{
    chPoolLoadArray(&msg_pool, msg_buffer, MSG_BUF_SIZE);

    static messagebus_new_topic_cb_t cb;
    messagebus_watchgroup_init(&watchgroup, &watchgroup_lock, &watchgroup_condvar);
    messagebus_new_topic_callback_register(&bus, &cb, new_topic_cb, NULL);

    /* Search for a usable logfile */
    FIL log_file;
    bool openened = false;
    for (int i = 0; i <= 9999; i++) {
        char name[10];
        sprintf(name, "%04d.bin", i);

        /* Fails if the file exists */
        if (f_open(&log_file, name, FA_CREATE_NEW | FA_WRITE) == FR_OK) {
            openened = true;
            break;
        }
    }

    if (openened) {
        static THD_WORKING_AREA(encode_wa, 2048);
        chThdCreateStatic(encode_wa, sizeof(encode_wa), HIGHPRIO, sdcard_topic_encode_thd, NULL);

        static THD_WORKING_AREA(write_wa, 2048);
        chThdCreateStatic(write_wa, sizeof(write_wa), NORMALPRIO, sdcard_topic_write_thd, NULL);
    }
}
