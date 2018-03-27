#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <shell.h>
#include "mpu9250.h"

static void cmd_mpu9250_test(BaseSequentialStream *chp, int argc, char **argv)
{
    static SPIConfig spi_cfg = {.end_cb = NULL,
                                .ssport = GPIOB,
                                .sspad = GPIOB_MPU_CSN,
                                .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA};
    spiStart(&SPID3, &spi_cfg);
    mpu9250_t mpu;
    mpu9250_init(&mpu, &SPID3);

    if (mpu9250_ping(&mpu)) {
        chprintf(chp, "OK\r\n");
    } else {
        chprintf(chp, "Did not answer\r\n");
    }
}

static void cmd_reboot(BaseSequentialStream *chp, int argc, char **argv)
{
    (void)chp;
    (void)argc;
    (void)argv;
    NVIC_SystemReset();
}

static ShellConfig shell_cfg;
const ShellCommand shell_commands[] = {{"reboot", cmd_reboot},
                                       {"mpu", cmd_mpu9250_test},
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
