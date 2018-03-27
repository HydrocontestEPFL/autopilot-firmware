#include <ch.h>
#include <hal.h>

#include "exti.h"

EVENTSOURCE_DECL(exti_mpu9250_event);

static void exti_cb(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;

    if (channel == GPIOF_MPU_INT) {
        chSysLockFromISR();
        chEvtBroadcastI(&exti_mpu9250_event);
        chSysUnlockFromISR();
    }
}

static const EXTConfig extcfg = {
    {{EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     /* MPU9250 IRQ */
     {EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOF, exti_cb},
     {EXT_CH_MODE_DISABLED, NULL},

     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     /* DWM 1000 IRQ. */
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},

     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL},
     {EXT_CH_MODE_DISABLED, NULL}}};

void exti_start(void)
{
    extStart(&EXTD1, &extcfg);
}
