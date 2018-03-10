# Hydrocontest ligthweight boat firmware

## Tasks to be done

### Does not depend on hw
* Output mixer (handles arming as well)
* Input remapper
* State estimation
* Control loops

### Can be done with Nucleo
* IP over serial port (depends of HW but can be done on Nucleo)
* Save parameter tree to flash memory
* HTTP server to retrieve logs from SD card without having to open the boat
    That one is especially important if we go with Yaffs2, as we will not have a working Yaffs driver in most PCs.

### Does depend on hw
* SBUS Input thread
* Output module (generates servo PWM)
* Drivers for IMUs, GPS, and altitude sensor
* Filesystem on SD card (FatFS for first iteration, YafFS2 might be more resilient)
* Logging of bus messages to sd card
* Logging of error messages to sd card

### Long term
* Protobuf to csv converter to ease the life of experimenters
* Protobuf plotter
* Telemetry GUI (write requirements with the rest of the team)
