#include <ch.h>
#include <hal.h>
#include <error/error.h>
#include <lwip_bindings/lwipthread.h>
#include "usbconf.h"
#include "cmd.h"
#include "main.h"
#include "sbus_thread.h"
#include "mpu9250_thread.h"
#include "input_mapper_thread.h"
#include "output_pwm_thread.h"
#include "telemetry/udp_topic_broadcaster.h"
#include "telemetry/udp_topic_injector.h"
#include "telemetry/rpc_server_thread.h"
#include "exti.h"
#include "state_estimation_thread.h"
#include "log.h"

parameter_namespace_t parameter_root;
messagebus_t bus;
static MUTEX_DECL(bus_lock);
static CONDVAR_DECL(bus_condvar);

static void blinker_start(void);

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
    udp_topic_injector_start();
    rpc_server_start();
    usb_start();
    log_start();
    blinker_start();
    exti_start();
    mpu9250_start();
    output_pwm_start();
    input_mapper_start();
    state_estimation_start();
    sbus_start(NULL); /* TODO: Use real stream */

    NOTICE("Boot complete");
    while (true) {
        chThdSleepMilliseconds(500);
    }
}

static void blinker_thread(void *p)
{
    (void)p;

    /* Those parameters are just smoke tests to check that the parameter
     * subsystem works. */
    parameter_t freq;
    parameter_namespace_t led_ns;
    parameter_namespace_declare(&led_ns, &parameter_root, "led");
    parameter_scalar_declare_with_default(&freq, &led_ns, "blink_rate", 5);
    while (true) {
        board_user_led_green_set(true);
        chThdSleepMilliseconds(500 / parameter_scalar_get(&freq));
        board_user_led_green_set(false);
        chThdSleepMilliseconds(500 / parameter_scalar_get(&freq));
    }
}

static void blinker_start(void)
{
    static THD_WORKING_AREA(wa, 256);
    chThdCreateStatic(wa, sizeof(wa), LOWPRIO, blinker_thread, NULL);
}
