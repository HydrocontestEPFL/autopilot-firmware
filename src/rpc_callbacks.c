#include <ch.h>
#include <hal.h>

#include "rpc_callbacks.h"
#include "messages/SetLed.pb.h"

static void led_set(pb_istream_t *input, pb_ostream_t *output)
{
    (void) output;
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


rpc_callback_t rpc_callbacks[] = {
    {"led_set", led_set},
};

const size_t rpc_callbacks_len = sizeof(rpc_callbacks) / sizeof(rpc_callback_t);
