#include <ch.h>
#include <hal.h>
#include "usbconf.h"
#include "cmd.h"
#include "main.h"
#include "udp_topic_broadcaster.h"
#include <lwip_bindings/lwipthread.h>


parameter_namespace_t parameter_root;
messagebus_t bus;
static MUTEX_DECL(bus_lock);
static CONDVAR_DECL(bus_condvar);

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

static void parameter_start(void)
{
    parameter_namespace_declare(&parameter_root, NULL, "");
}

static void messagebus_start(void)
{
    messagebus_init(&bus, &bus_lock, &bus_condvar);
}

int main(void)
{
    halInit();
    chSysInit();

    parameter_start();
    messagebus_start();
    ip_start();
    udp_topic_broadcast_start();

    usb_start();

    while (true) {
        chThdSleepMilliseconds(500);
    }
}
