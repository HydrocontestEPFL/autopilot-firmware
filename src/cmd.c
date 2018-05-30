#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <shell.h>
#include <parameter_flash_storage/parameter_flash_storage.h>
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

    UINT written;
    FRESULT res;
    res = f_write(&file, "foobar", 6, &written);

    if (res != FR_OK || written != 6) {
        chprintf(chp, "Could not write, res = %d\n", (int)res);
    }
    f_sync(&file);
    f_close(&file);
}

static void tree_indent(BaseSequentialStream *out, int indent)
{
    int i;
    for (i = 0; i < indent; ++i) {
        chprintf(out, "  ");
    }
}

static void show_config_tree(BaseSequentialStream *out, parameter_namespace_t *ns, int indent)
{
    parameter_t *p;
    char string_buf[64];

    tree_indent(out, indent);
    chprintf(out, "%s:\r\n", ns->id);

    for (p = ns->parameter_list; p != NULL; p = p->next) {
        tree_indent(out, indent + 1);
        if (parameter_defined(p)) {
            switch (p->type) {
                case _PARAM_TYPE_SCALAR:
                    chprintf(out, "%s: %f\r\n", p->id, parameter_scalar_get(p));
                    break;

                case _PARAM_TYPE_INTEGER:
                    chprintf(out, "%s: %d\r\n", p->id, parameter_integer_get(p));
                    break;

                case _PARAM_TYPE_BOOLEAN:
                    chprintf(out, "%s: %s\r\n", p->id, parameter_boolean_get(p) ? "true" : "false");
                    break;

                case _PARAM_TYPE_STRING:
                    parameter_string_get(p, string_buf, sizeof(string_buf));
                    chprintf(out, "%s: %s\r\n", p->id, string_buf);
                    break;

                default:
                    chprintf(out, "%s: unknown type %d\r\n", p->id, p->type);
                    break;
            }
        } else {
            chprintf(out, "%s: [not set]\r\n", p->id);
        }
    }

    if (ns->subspaces) {
        show_config_tree(out, ns->subspaces, indent + 1);
    }

    if (ns->next) {
        show_config_tree(out, ns->next, indent);
    }
}

static void cmd_config_tree(BaseSequentialStream *chp, int argc, char **argv)
{
    parameter_namespace_t *ns;
    if (argc != 1) {
        ns = &parameter_root;
    } else {
        ns = parameter_namespace_find(&parameter_root, argv[0]);
        if (ns == NULL) {
            chprintf(chp, "Cannot find subtree.\r\n");
            return;
        }
    }

    show_config_tree(chp, ns, 0);
}

static void cmd_config_set(BaseSequentialStream *chp, int argc, char **argv)
{
    parameter_t *param;
    int value_i;
    float value_f;

    if (argc != 2) {
        chprintf(chp, "Usage: config_set /parameter/url value.\r\n");
        return;
    }

    param = parameter_find(&parameter_root, argv[0]);

    if (param == NULL) {
        chprintf(chp, "Could not find parameter \"%s\"\r\n", argv[0]);
        return;
    }

    switch (param->type) {
        case _PARAM_TYPE_INTEGER:
            if (sscanf(argv[1], "%d", &value_i) == 1) {
                parameter_integer_set(param, value_i);
            } else {
                chprintf(chp, "Invalid value for integer parameter.\r\n");
            }
            break;

        case _PARAM_TYPE_SCALAR:
            if (sscanf(argv[1], "%f", &value_f) == 1) {
                parameter_scalar_set(param, value_f);
            } else {
                chprintf(chp, "Invalid value for scalar parameter.\r\n");
            }
            break;

        case _PARAM_TYPE_BOOLEAN:
            if (!strcmp(argv[1], "true")) {
                parameter_boolean_set(param, true);
            } else if (!strcmp(argv[1], "false")) {
                parameter_boolean_set(param, false);
            } else {
                chprintf(chp, "Invalid value for boolean parameter, must be true or false.\r\n");
            }
            break;

        case _PARAM_TYPE_STRING:
            if (argc == 2) {
                parameter_string_set(param, argv[1]);
            } else {
                chprintf(chp, "Invalid value for string parameter, must not use spaces.\r\n");
            }
            break;

        default:
            chprintf(chp, "%s: unknown type %d\r\n", param->id, param->type);
            break;
    }
}

static void cmd_config_erase(BaseSequentialStream *chp, int argc, char **argv)
{
    (void)argc;
    (void)argv;
    (void)chp;
    parameter_flash_storage_erase(&_config_start);
}

static void cmd_config_save(BaseSequentialStream *chp, int argc, char **argv)
{
    (void)argc;
    (void)argv;
    size_t len = (size_t)(&_config_end - &_config_start);
    bool success;

    // First write the config to flash
    parameter_flash_storage_save(&_config_start, len, &parameter_root);

    // Second try to read it back, see if we failed
    success = parameter_flash_storage_load(&parameter_root, &_config_start);

    if (success) {
        chprintf(chp, "OK.\r\n");
    } else {
        chprintf(chp, "Save failed.\r\n");
    }
}

static void cmd_config_load(BaseSequentialStream *chp, int argc, char **argv)
{
    (void)argc;
    (void)argv;
    bool success;

    success = parameter_flash_storage_load(&parameter_root, &_config_start);

    if (success) {
        chprintf(chp, "OK.\r\n");
    } else {
        chprintf(chp, "Load failed.\r\n");
    }
}

static ShellConfig shell_cfg;
const ShellCommand shell_commands[] = {{"reboot", cmd_reboot},
                                       {"topics", cmd_topics},
                                       {"mount", cmd_mount},
                                       {"umount", cmd_umount},
                                       {"config_tree", cmd_config_tree},
                                       {"config_set", cmd_config_set},
                                       {"config_save", cmd_config_save},
                                       {"config_load", cmd_config_load},
                                       {"config_erase", cmd_config_erase},

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
