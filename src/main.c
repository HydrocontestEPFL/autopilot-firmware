#include <ch.h>
#include <hal.h>
#include "usbconf.h"
#include "cmd.h"

static void usb_start(void)
{
    /* Initializes a serial-over-USB CDC driver.  */
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    shell_start((BaseSequentialStream *)&SDU1);
}

int main(void)
{
    halInit();
    chSysInit();

    usb_start();

    while (true) {
        chThdSleepMilliseconds(500);
    }
}
