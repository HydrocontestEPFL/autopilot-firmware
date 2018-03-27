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
    /*
     * SPI3 configuration structure for MPU9250.
     * SPI3 is on APB2 @ 84MHz / 128 = 656.25kHz
     * CPHA=1, CPOL=1, 8bits frames, MSb transmitted first.
     */
    static SPIConfig spi_cfg = {.end_cb = NULL,
                                .ssport = GPIOB,
                                .sspad = GPIOB_MPU_CSN,
                                .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA};
    spiStart(&SPID3, &spi_cfg);

    mpu9250_init(mpu, &SPID3);

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
