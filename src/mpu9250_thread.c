#include <ch.h>
#include <hal.h>

#include <msgbus/messagebus.h>

#include "mpu9250_thread.h"
#include "mpu9250.h"
#include "main.h"
#include "msgbus_protobuf.h"
#include "exti.h"
#include "messages/IMUReading.pb.h"

#define MPU_INTERRUPT_EVENT 0x01

static void mpu9250_init_hardware(mpu9250_t *mpu)
{
    // power up sensors
    palSetPadMode(GPIOE, GPIOE_VDD_3V3_SENSORS_EN, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOE, GPIOE_VDD_3V3_SENSORS_EN);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOE, GPIOE_VDD_3V3_SENSORS_EN);

    // init mosi miso sck cs, AF5
    palSetPadMode(GPIOA, GPIOA_MPU9250_SCK, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOA, GPIOA_MPU9250_MISO, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
    palSetPadMode(GPIOA, GPIOA_MPU9250_MOSI, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOC, GPIOC_MPU9250_CS, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOD, GPIOD_MPU9250_DRDY, PAL_MODE_INPUT_PULLUP);
    palSetPad(GPIOC, GPIOC_MPU9250_CS);

    palSetPadMode(GPIOC, GPIOC_20608_CS, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
    palSetPad(GPIOC, GPIOC_20608_CS);
    palSetPadMode(GPIOD, GPIOD_BARO_CS, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
    palSetPad(GPIOD, GPIOD_BARO_CS);
    palSetPadMode(GPIOD, GPIOD_FRAM_CS, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
    palSetPad(GPIOD, GPIOD_FRAM_CS);

    /*
     * SPI1 configuration structure for MPU9250.
     * SPI1 is on APB2 @ 84MHz / 128 = 656.25kHz
     * CPHA=1, CPOL=1, 8bits frames, MSb transmitted first.
     */
    static SPIConfig spi_cfg = {
        .end_cb = NULL,
        .ssport = GPIOC,
        .sspad = GPIOC_MPU9250_CS,
        .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA
    };
    spiStart(&SPID1, &spi_cfg);

    mpu9250_init(mpu, &SPID1);

    mpu9250_reset(mpu);

    do {
        chThdSleepMilliseconds(100);
    } while (!mpu9250_ping(mpu));

    mpu9250_configure(mpu);
    mpu9250_enable_magnetometer(mpu);
}

static void mpu9250_reader_thd(void *p)
{
    (void)p;
    mpu9250_t mpu;

    chRegSetThreadName("IMU");

    event_listener_t imu_int;
    chEvtRegisterMask(&exti_mpu9250_event, &imu_int, MPU_INTERRUPT_EVENT);

    mpu9250_init_hardware(&mpu);

    /* Creates the mpu9250 topic */
    static TOPIC_DECL(mpu9250_topic, IMUReading);

    messagebus_advertise_topic(&bus, &mpu9250_topic.topic, "imu0");

    while (1) {
        IMUReading msg;

        /* Wait for an interrupt from the MPU */
        chEvtWaitAny(MPU_INTERRUPT_EVENT);

        /* Read data from the IMU. */
#if 0
        /* TODO: For some reason the macro ST2US creates an overflow. */
        msg.timestamp.us = ST2US(chVTGetSystemTime());
#else
        msg.timestamp.us = chVTGetSystemTime() * (1000000 / CH_CFG_ST_FREQUENCY);
#endif
        mpu9250_gyro_read(&mpu, &msg.angular_speed.x, &msg.angular_speed.y, &msg.angular_speed.z);
        mpu9250_acc_read(&mpu, &msg.acceleration.x, &msg.acceleration.y, &msg.acceleration.z);
        mpu9250_mag_read(&mpu, &msg.magnetic_field.x, &msg.magnetic_field.y, &msg.magnetic_field.z);

        /* TODO: Read temperature */

        /* Publish the data. */
        messagebus_topic_publish(&mpu9250_topic.topic, &msg, sizeof(msg));

        /* Signals the MPU that we are ready for another interrupt. */
        mpu9250_interrupt_read_and_clear(&mpu);
    }
}

void mpu9250_start(void)
{
    static THD_WORKING_AREA(mpu9250_reader_thd_wa, 4096);
    chThdCreateStatic(mpu9250_reader_thd_wa,
                      sizeof(mpu9250_reader_thd_wa),
                      HIGHPRIO,
                      mpu9250_reader_thd,
                      NULL);
}
