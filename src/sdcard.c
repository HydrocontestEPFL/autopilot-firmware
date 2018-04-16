#include <ch.h>
#include <hal.h>
#include <error/error.h>
#include <stdbool.h>
#include <fatfs/ff.h>
#include "sdcard.h"

bool sdcard_mounted = false;
static FATFS SDC_FS;

void sdcard_start(void)
{
    static const SDCConfig sdc_config = {NULL, /* not needed for SD cards */
                                         SDC_MODE_4BIT};
    sdcStart(&SDCD1, &sdc_config);
}

void sdcard_mount(void)
{
    FRESULT err;
    sdcard_mounted = false;

    if (sdcConnect(&SDCD1) == HAL_FAILED) {
        WARNING("Could not connect to SDCD1");
        return;
    }

    const bool mount_immediately = true;
    err = f_mount(&SDC_FS, "", mount_immediately);
    if (err != FR_OK) {
        WARNING("Could not mount filesystem");
        sdcDisconnect(&SDCD1);
        return;
    }
    sdcard_mounted = true;
}

void sdcard_unmount(void)
{
    f_mount(NULL, "", 0); /* fatfs unmount */
    sdcDisconnect(&SDCD1);
    sdcard_mounted = false;
}

void sdcard_automount(void)
{
    if (sdcIsCardInserted(&SDCD1)) {
        if (!sdcard_mounted) {
            sdcard_mount();
        }
    } else {
        WARNING("No SD card present");
        if (sdcard_mounted) {
            WARNING("Unmounting");
            sdcard_unmount();
        }
    }
}

void sdcard_activity(void)
{
    //    palTogglePad(GPIOE, GPIOE_LED_SD);
}
