#include <ch.h>
#include <hal.h>
#include <error/error.h>
#include <netif/slipif.h>
#include <lwip/tcpip.h>
#include <lwip/sio.h>
#include "usbconf.h"

/**
 * Sends a single character to the serial device.
 *
 * @param c character to send
 * @param fd serial device handle
 *
 * @note This function will block until the character can be sent.
 * @bug On UNIX platform, writing to a broken FIFO pipe will cause a SIGPIPE
 * to be raised.
 */
void sio_send(u8_t c, sio_fd_t fd)
{
    BaseSequentialStream *stream = fd;
    streamPut(stream, c);
}

/**
 * Reads from the serial device.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 *
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
    BaseSequentialStream *stream = fd;
    for (u32_t i = 0; i < len; i++) {
        msg_t msg = streamGet(stream);

        if (msg == MSG_RESET) {
            /* Needed to avoid full CPU utilization in some cases, e.g. when
             * USB is not connected. */
            chThdSleepMilliseconds(1);
            return 0;
        }

        data[i] = msg;
    }
    return len;
}

/**
 * Opens a serial device for communication.
 *
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
    (void)devnum;
    return (sio_fd_t)&SDU1;
}

static struct netif slipf;
static ip_addr_t ipaddr, netmask, gw;

void ip_over_uart_start(void)
{
    IP4_ADDR(&gw, 10, 0, 0, 11);
    IP4_ADDR(&ipaddr, 10, 0, 0, 10);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    netif_add(&slipf, &ipaddr, &netmask, &gw, NULL, slipif_init, tcpip_input);
    netif_set_up(&slipf);
    netif_set_link_up(&slipf);
}
