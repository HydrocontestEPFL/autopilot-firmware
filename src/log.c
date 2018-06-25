#include <hal.h>
#include <chprintf.h>
#include <stdio.h>
#include <string.h>
#include <error/error.h>
#include <parameter/parameter.h>

#include "log.h"
#include "usbconf.h"

#define OUTPUT_STREAM ((BaseSequentialStream *)&SD7)

MUTEX_DECL(log_lock);

static void vuart_log_message(struct error *e, va_list args);
static void vpanic_message(struct error *e, va_list args);

static void log_message(struct error *e, ...)
{
    va_list va;

    if (e->severity == ERROR_SEVERITY_ERROR) {
        va_start(va, e);
        vpanic_message(e, va);
        va_end(va);
    }

    chMtxLock(&log_lock);

    va_start(va, e);
    vuart_log_message(e, va);
    va_end(va);

    chMtxUnlock(&log_lock);
}

static const char *get_thread_name(void)
{
    const char *thread_name;

    thread_name = chRegGetThreadNameX(chThdGetSelfX());
    if (thread_name == NULL) {
        thread_name = "unknown";
    }

    return thread_name;
}

static void vuart_log_message(struct error *e, va_list args)
{
    /* Print time */
    uint32_t ts = chVTGetSystemTime() * (1000000 / CH_CFG_ST_FREQUENCY);
    uint32_t s = ts / 1000000;
    uint32_t us = ts - s * 1000000;
    chprintf(OUTPUT_STREAM, "[%4d.%06d]\t", s, us);

    /* Print location. */
    chprintf(OUTPUT_STREAM, "%s:%d\t", strrchr(e->file, '/') + 1, e->line);

    /* Print current thread */
    chprintf(OUTPUT_STREAM, "%s\t", get_thread_name());

    /* Print severity message */
    chprintf(OUTPUT_STREAM, "%s\t", error_severity_get_name(e->severity));

    /* Print message */
    chvprintf(OUTPUT_STREAM, e->text, args);

    chprintf(OUTPUT_STREAM, "\n");
}

static void vpanic_message(struct error *e, va_list args)
{
    static char buffer[256];
    vsnprintf(buffer, sizeof(buffer), e->text, args);
    chSysHalt(buffer);
}

void log_start(void)
{
    error_register_error(log_message);
    error_register_warning(log_message);
    error_register_notice(log_message);
    // error_register_debug(log_message);
}
