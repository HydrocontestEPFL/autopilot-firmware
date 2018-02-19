#include <ch.h>
#include <hal.h>
#include <lwip/api.h>
#include "rpc.h"
#include "rpc_server_thread.h"
#include "rpc_callbacks.h"

static void rpc_server_thread(void *p)
{
    (void) p;
    struct netconn *conn, *client_conn;
    err_t err;

    static uint8_t input_buffer[1024];
    static uint8_t output_buffer[1024];
    size_t input_len;
    size_t output_len;

    chRegSetThreadName("rpc_server");

    /* Creates a TCP server */
    conn = netconn_new(NETCONN_TCP);
    LWIP_ASSERT("Could not open RPC server connection.", conn != NULL);

    /* Bind it to port 10000 */
    netconn_bind(conn, IP_ADDR_ANY, 10000);
    netconn_listen(conn);


    while (true) {
        /* Reset length */
        input_len = 0;

        err = netconn_accept(conn, &client_conn);

        if (err != ERR_OK) {
            continue;
        }

        struct netbuf *buf;

        err = netconn_recv(client_conn, &buf);

        /* If connection was closed on client side */
        if (err != ERR_OK) {
            netconn_close(client_conn);
            netconn_delete(client_conn);
        }

        /* Append the data to the buffer */
        uint16_t len;
        void *data;
        do {
            netbuf_data(buf, &data, &len);
            memcpy(&input_buffer[input_len], data, len);
            input_len += len;
        } while (netbuf_next(buf) >= 0);

        netbuf_delete(buf);

        /* If the buffer is complete, then runs the command it contains. */
        if (rpc_buffer_is_complete(input_buffer, input_len)) {
            output_len = rpc_process(rpc_callbacks, rpc_callbacks_len,
                                     input_buffer, sizeof(input_buffer),
                                     output_buffer, sizeof(output_buffer));

            netconn_write(client_conn, output_buffer, output_len, NETCONN_COPY);

            /* Erase buffer */
            input_len = 0;
        }

    }
}

void rpc_server_start(void)
{
    static THD_WORKING_AREA(wa, 2048);
    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, rpc_server_thread, NULL);
}
