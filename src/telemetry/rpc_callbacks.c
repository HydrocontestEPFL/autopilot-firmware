#include <ch.h>
#include <hal.h>

#include "rpc_callbacks.h"
#include "main.h"
#include "rpc_parameter_server.h"
#include "messages/SetLed.pb.h"

static void led_set(void *p, pb_istream_t *input, pb_ostream_t *output)
{
    (void)output;
    (void)p;
    SetLedRequest req;
    pb_decode(input, SetLedRequest_fields, &req);

    switch (req.led) {
        case SetLedRequest_Led_RED:
            board_user_led_red_set(req.status);
            break;

        case SetLedRequest_Led_BLUE:
            board_user_led_blue_set(req.status);
            break;

        case SetLedRequest_Led_GREEN:
            board_user_led_green_set(req.status);
            break;
    }
}

static struct parameter_server_data param_server_data = {.ns = &parameter_root,
                                                         .config_start = &_config_start,
                                                         .config_end = &_config_end};

rpc_callback_t rpc_callbacks[] = {
    {"led_set", led_set, NULL},
    {"parameter_server_enumerate", parameter_server_enumerate, &param_server_data},
    {"parameter_server_set", parameter_server_set, &param_server_data},
};

const size_t rpc_callbacks_len = sizeof(rpc_callbacks) / sizeof(rpc_callback_t);
