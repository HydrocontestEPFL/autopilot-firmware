#ifndef RPC_H
#define RPC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pb_encode.h>
#include <pb_decode.h>

typedef struct {
    const char *name;
    void (*cb)(void *, pb_istream_t *, pb_ostream_t*);
    void *cb_arg;
} rpc_callback_t;


/** Processes the message, calls the correct callback and writes output back.
 *
 * @param [in] callbacks The array of message handlers with their names.
 * @param [in] callbacks_len Number of items in callbacks.
 * @param [in] input_buf Buffer containing the received message.
 * @param [in] input_len Size of input_buf in bytes.
 * @param [in] output_buf Pointer to a memory region where the reply will be stored.
 * @param [in] output_len Size of output_buf in bytes.
 *
 * @returns Size of the reply in bytes.
 * @warning Do not call this function on incomplete message buffers.
 * rpc_buffer_is_complete can be used to check that the message is complete.
 * @todo TODO: No bound check is done on input_buf.
 */
size_t rpc_process(rpc_callback_t *callbacks,
                   unsigned callbacks_len,
                   const uint8_t *input_buf, size_t input_len,
                   uint8_t *output_buf, size_t output_len);

/** Returns if the given buffer contains a complete RPC message.
 *
 * This functions tells if the buffer contains a message ready to be parsed by
 * rpc_process. It can be used for example in a TCP hander to know if we need
 * to read more bytes before processing.
 */
bool rpc_buffer_is_complete(const uint8_t *buffer, size_t buffer_len);

#ifdef __cplusplus
}
#endif

#endif
