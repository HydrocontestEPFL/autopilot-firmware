# Hydrocontest ligthweight boat firmware

## Tasks to be done

### Does not depend on hw
* State estimation
* Control loops

### Can be done with Nucleo
* IP over serial port (depends of HW but can be done on Nucleo)
* Save parameter tree to flash memory
* SBUS Input thread (can be simulated over USB)

### Does depend on hw
* Output module (generates servo PWM)
* Drivers for IMUs, GPS, and altitude sensor
* Logging of lib/error messages to sd card

### Long term
* Protobuf to csv converter to ease the life of experimenters.
    -> Still need to do the csv part, but can read protobuf log
* Protobuf plotter
* Telemetry GUI (write requirements with the rest of the team)
