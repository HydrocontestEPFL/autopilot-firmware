#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <shell.h>
#include "mpu9250.h"
#include "main.h"
#include <fatfs/ff.h>
#include "sdcard.h"

static void cmd_topics(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    const char *usage
        = "usage:\r\n"
          "topics list -- Lists all available topics.\r\n"
          "topics hz topic_name -- Displays the rate of the"
          "topic over a 5 second window.";

    if (argc < 1) {
        chprintf(chp, "%s\r\n", usage);
        return;
    }

    if (!strcmp(argv[0], "list")) {
        chprintf(chp, "available topics:\r\n");

        MESSAGEBUS_TOPIC_FOREACH(&bus, topic)
        {
            chprintf(chp, "%s\r\n", topic->name);
        }
    } else if (!strcmp(argv[0], "hz")) {
        if (argc != 2) {
            chprintf(chp, "%s\r\n", usage);
            return;
        }

        messagebus_topic_t *topic = messagebus_find_topic(&bus, argv[1]);
        if (topic == NULL) {
            chprintf(chp, "Cannot find topic \"%s\".\r\n", argv[1]);
            return;
        }

        chprintf(chp, "Waiting for publish for 5 seconds...\r\n");

        systime_t start = chVTGetSystemTime();
        unsigned int message_counter = 0;

        while (chVTGetSystemTime() < start + MS2ST(5000)) {
            chMtxLock(topic->lock);
            if (chCondWaitTimeout(topic->condvar, MS2ST(10)) != MSG_TIMEOUT) {
                message_counter++;
                chMtxUnlock(topic->lock);
            }
        }

        if (message_counter == 0) {
            chprintf(chp, "No messages.\r\n");
        } else {
            chprintf(chp, "Average rate: %.2f Hz\r\n", message_counter / 5.);
        }
    } else {
        chprintf(chp, "%s\r\n", usage);
        return;
    }
}

static void cmd_reboot(BaseSequentialStream *chp, int argc, char **argv)
{
    (void)chp;
    (void)argc;
    (void)argv;
    NVIC_SystemReset();
}

static void cmd_mount(BaseSequentialStream *chp, int argc, char **argv)
{
    (void)chp;
    (void)argc;
    (void)argv;
    sdcard_automount();
}

static void cmd_umount(BaseSequentialStream *chp, int argc, char **argv)
{
    (void)chp;
    (void)argc;
    (void)argv;
    sdcard_unmount();
}


static void cmd_touch(BaseSequentialStream *chp, int argc, char **argv)
{
    if (argc != 1) {
        chprintf(chp, "Usage: touch file\r\n");
        return;
    }

    static FIL file;
    if (f_open(&file, argv[0], FA_WRITE | FA_CREATE_NEW) != FR_OK) {
        chprintf(chp, "Could not open %s\r\n", argv[0]);
        return;
    }

    BYTE written;
    FRESULT res;
    res = f_write(&file, "foobar", 6, &written);

    if (res != FR_OK || written != 6) {
        chprintf(chp, "Could not write, res = %d\n", (int)res);
    }
    f_sync(&file);
    f_close(&file);
}

static ShellConfig shell_cfg;
const ShellCommand shell_commands[] = {{"reboot", cmd_reboot},
                                       {"topics", cmd_topics},
                                       {"mount", cmd_mount},
                                       {"umount", cmd_umount},
                                       {"touch", cmd_touch},
                                       {NULL, NULL}};

#define SHELL_WA_SIZE 2048

static THD_FUNCTION(shell_spawn_thd, p)
{
    BaseSequentialStream *io = (BaseSequentialStream *)p;
    thread_t *shelltp = NULL;
    static THD_WORKING_AREA(shell_wa, 2048);

    shell_cfg.sc_channel = io;
    shell_cfg.sc_commands = shell_commands;

    shellInit();

    while (TRUE) {
        if (!shelltp) {
            shelltp = chThdCreateStatic(&shell_wa,
                                        sizeof(shell_wa),
                                        NORMALPRIO,
                                        shellThread,
                                        (void *)&shell_cfg);
            chRegSetThreadNameX(shelltp, "shell");
        } else {
            if (chThdTerminatedX(shelltp)) {
                chThdRelease(shelltp);
                shelltp = NULL;
            }
        }
        chThdSleepMilliseconds(500);
    }
}
void shell_start(BaseSequentialStream *io)
{
    static THD_WORKING_AREA(wa, SHELL_WA_SIZE);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, shell_spawn_thd, io);
}
