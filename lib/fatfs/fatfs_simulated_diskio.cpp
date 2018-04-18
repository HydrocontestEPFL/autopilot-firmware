#include "fatfs/ff.h"
#include "fatfs/diskio.h"
#include <cstdint>
#include <cstring>

#define SECTOR_COUNT 1024
#define SECTOR_SIZE 1024

static uint8_t disk[SECTOR_COUNT * SECTOR_SIZE];

int ff_cre_syncobj(BYTE vol, _SYNC_t *sobj)
{
    (void)sobj;
    (void)vol;
    return true;
}

int ff_del_syncobj(_SYNC_t sobj)
{
    (void)sobj;
    return true;
}

int ff_req_grant(_SYNC_t sobj)
{
    (void)sobj;
    return true;
}

void ff_rel_grant(_SYNC_t sobj)
{
    (void)sobj;
}

BYTE disk_initialize(BYTE drive_number)
{
    (void)drive_number;
    return 0;
}

DRESULT disk_ioctl(BYTE drive, BYTE cmd, void *buff)
{
    (void)drive;
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *((DWORD *)buff) = SECTOR_COUNT;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((WORD *)buff) = SECTOR_COUNT;
            return RES_OK;
    }
    return RES_PARERR;
}

DRESULT disk_write(BYTE drive, const BYTE *buff, DWORD sector, UINT sector_count)
{
    (void)drive;
    for (auto i = 0u; i < sector_count; i++) {
        memcpy(&disk[sector * SECTOR_SIZE], &buff[i * SECTOR_SIZE], SECTOR_SIZE);
    }
    return RES_OK;
}

DRESULT disk_read(BYTE drive, BYTE *output, DWORD sector, UINT sector_count)
{
    (void)drive;
    for (auto i = 0u; i < sector_count; i++) {
        memcpy(&output[i * SECTOR_SIZE], &disk[sector * SECTOR_SIZE], SECTOR_SIZE);
    }
    return RES_OK;
}

DSTATUS disk_status(BYTE drive)
{
    (void)drive;
    return 0;
}

DWORD get_fattime(void)
{
    return ((uint32_t)0 | (1 << 16)) | (1 << 21); /* wrong but valid time */
}
