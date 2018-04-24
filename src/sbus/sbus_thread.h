#ifndef SBUS_THREAD_H
#define SBUS_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hal.h>

/** Starts the SBUS input thread.
 *
 * @param [in] stream The stream to which the SBUS receiver is plugged to.
 * Usually a UART. */
void sbus_start(BaseSequentialStream *stream);

#ifdef __cplusplus
}
#endif

#endif
