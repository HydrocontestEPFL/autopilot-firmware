#ifndef MPU9250_THREAD_H
#define MPU9250_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/** Starts the thread doing the IMU reading. */
void mpu9250_start(void);

#ifdef __cplusplus
}
#endif

#endif
