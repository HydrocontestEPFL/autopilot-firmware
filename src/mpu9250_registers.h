#ifndef MPU9250_REGISTERS_H
#define MPU9250_REGISTERS_H

// clang-format off

#define MPU9250_REG_SELF_TEST_X_GYRO         0x00
#define MPU9250_REG_SELF_TEST_Y_GYRO         0x01
#define MPU9250_REG_SELF_TEST_Z_GYRO         0x02
#define MPU9250_REG_SELF_TEST_X_ACCEL        0x0d
#define MPU9250_REG_SELF_TEST_Y_ACCEL        0x0e
#define MPU9250_REG_SELF_TEST_Z_ACCEL        0x0f
#define MPU9250_REG_XG_OFFSET_H              0x13
#define MPU9250_REG_XG_OFFSET_L              0x14
#define MPU9250_REG_YG_OFFSET_H              0x15
#define MPU9250_REG_YG_OFFSET_L              0x16
#define MPU9250_REG_ZG_OFFSET_H              0x17
#define MPU9250_REG_ZG_OFFSET_L              0x18
#define MPU9250_REG_SMPLRT_DIV               0x19
#define MPU9250_REG_CONFIG                   0x1a
#define MPU9250_REG_GYRO_CONFIG              0x1b
#define MPU9250_REG_ACCEL_CONFIG             0x1c
#define MPU9250_REG_ACCEL_CONFIG_2           0x1d
#define MPU9250_REG_LP_ACCEL_ODR             0x1e
#define MPU9250_REG_WOM_THR                  0x1f
#define MPU9250_REG_FIFO_EN                  0x23
#define MPU9250_REG_I2C_MST_CTRL             0x24
#define MPU9250_REG_I2C_SLV0_ADDR            0x25
#define MPU9250_REG_I2C_SLV0_REG             0x26
#define MPU9250_REG_I2C_SLV0_CTRL            0x27
#define MPU9250_REG_I2C_SLV1_ADDR            0x28
#define MPU9250_REG_I2C_SLV1_REG             0x29
#define MPU9250_REG_I2C_SLV1_CTRL            0x2a
#define MPU9250_REG_I2C_SLV2_ADDR            0x2b
#define MPU9250_REG_I2C_SLV2_REG             0x2c
#define MPU9250_REG_I2C_SLV2_CTRL            0x2d
#define MPU9250_REG_I2C_SLV3_ADDR            0x2e
#define MPU9250_REG_I2C_SLV3_REG             0x2f
#define MPU9250_REG_I2C_SLV3_CTRL            0x30
#define MPU9250_REG_I2C_SLV4_ADDR            0x31
#define MPU9250_REG_I2C_SLV4_REG             0x32
#define MPU9250_REG_I2C_SLV4_DO              0x33
#define MPU9250_REG_I2C_SLV4_CTRL            0x34
#define MPU9250_REG_I2C_SLV4_DI              0x35
#define MPU9250_REG_I2C_MST_STATUS           0x36
#define MPU9250_REG_INT_PIN_CFG              0x37
#define MPU9250_REG_INT_ENABLE               0x38
#define MPU9250_REG_INT_STATUS               0x3a
#define MPU9250_REG_ACCEL_XOUT_H             0x3b
#define MPU9250_REG_ACCEL_XOUT_L             0x3c
#define MPU9250_REG_ACCEL_YOUT_H             0x3d
#define MPU9250_REG_ACCEL_YOUT_L             0x3e
#define MPU9250_REG_ACCEL_ZOUT_H             0x3f
#define MPU9250_REG_ACCEL_ZOUT_L             0x40
#define MPU9250_REG_TEMP_OUT_H               0x41
#define MPU9250_REG_TEMP_OUT_L               0x42
#define MPU9250_REG_GYRO_XOUT_H              0x43
#define MPU9250_REG_GYRO_XOUT_L              0x44
#define MPU9250_REG_GYRO_YOUT_H              0x45
#define MPU9250_REG_GYRO_YOUT_L              0x46
#define MPU9250_REG_GYRO_ZOUT_H              0x47
#define MPU9250_REG_GYRO_ZOUT_L              0x48
#define MPU9250_REG_EXT_SENS_DATA_00         0x49
#define MPU9250_REG_EXT_SENS_DATA_01         0x4a
#define MPU9250_REG_EXT_SENS_DATA_02         0x4b
#define MPU9250_REG_EXT_SENS_DATA_03         0x4c
#define MPU9250_REG_EXT_SENS_DATA_04         0x4d
#define MPU9250_REG_EXT_SENS_DATA_05         0x4e
#define MPU9250_REG_EXT_SENS_DATA_06         0x4f
#define MPU9250_REG_EXT_SENS_DATA_07         0x50
#define MPU9250_REG_EXT_SENS_DATA_08         0x51
#define MPU9250_REG_EXT_SENS_DATA_09         0x52
#define MPU9250_REG_EXT_SENS_DATA_10         0x53
#define MPU9250_REG_EXT_SENS_DATA_11         0x54
#define MPU9250_REG_EXT_SENS_DATA_12         0x55
#define MPU9250_REG_EXT_SENS_DATA_13         0x56
#define MPU9250_REG_EXT_SENS_DATA_14         0x57
#define MPU9250_REG_EXT_SENS_DATA_15         0x58
#define MPU9250_REG_EXT_SENS_DATA_16         0x59
#define MPU9250_REG_EXT_SENS_DATA_17         0x5a
#define MPU9250_REG_EXT_SENS_DATA_18         0x5b
#define MPU9250_REG_EXT_SENS_DATA_19         0x5c
#define MPU9250_REG_EXT_SENS_DATA_20         0x5d
#define MPU9250_REG_EXT_SENS_DATA_21         0x5e
#define MPU9250_REG_EXT_SENS_DATA_22         0x5f
#define MPU9250_REG_EXT_SENS_DATA_23         0x60
#define MPU9250_REG_I2C_SLV0_DO              0x63
#define MPU9250_REG_I2C_SLV1_DO              0x64
#define MPU9250_REG_I2C_SLV2_DO              0x65
#define MPU9250_REG_I2C_SLV3_DO              0x66
#define MPU9250_REG_I2C_MST_DELAY_CTRL       0x67
#define MPU9250_REG_SIGNAL_PATH_RESET        0x68
#define MPU9250_REG_MOT_DETECT_CTRL          0x69
#define MPU9250_REG_USER_CTRL                0x6a
#define MPU9250_REG_PWR_MGMT_1               0x6b
#define MPU9250_REG_PWR_MGMT_2               0x6c
#define MPU9250_REG_FIFO_COUNTH              0x72
#define MPU9250_REG_FIFO_COUNTL              0x73
#define MPU9250_REG_FIFO_R_W                 0x74
#define MPU9250_REG_WHO_AM_I                 0x75
#define MPU9250_REG_XA_OFFSET_H              0x77
#define MPU9250_REG_XA_OFFSET_L              0x78
#define MPU9250_REG_YA_OFFSET_H              0x7a
#define MPU9250_REG_YA_OFFSET_L              0x7b
#define MPU9250_REG_ZA_OFFSET_H              0x7d
#define MPU9250_REG_ZA_OFFSET_L              0x7e

/* FIFO Enable */
#define MPU9250_REG_FIFO_EN_TEMP_OUT         (1 << 7)
#define MPU9250_REG_FIFO_EN_GYRO_XOUT        (1 << 6)
#define MPU9250_REG_FIFO_EN_GYRO_YOUT        (1 << 5)
#define MPU9250_REG_FIFO_EN_GYRO_ZOUT        (1 << 4)
#define MPU9250_REG_FIFO_EN_ACCEL_OUT        (1 << 3)

/* INT Pin / Bypass Enable Configuration */
#define MPU9250_REG_INT_PIN_CFG_ACTIVE_LOW   (1 << 7)
#define MPU9250_REG_INT_PIN_CFG_OPEN_DRAIN   (1 << 6)
#define MPU9250_REG_INT_PIN_CFG_LATCHED_INT  (1 << 5)
#define MPU9250_REG_INT_PIN_CFG_ANYREADD_CLR (1 << 4)

/* Interrupt Enable */
#define MPU9250_REG_INT_ENABLE_WOM           (1 << 6)
#define MPU9250_REG_INT_ENABLE_FIFO_OVERFLOW (1 << 4)
#define MPU9250_REG_INT_ENABLE_FSYNC_INT     (1 << 3)
#define MPU9250_REG_INT_ENABLE_DATA_RDY      (1 << 0)

/* User Control */
#define MPU9250_REG_USER_CTRL_FIFO_EN        (1 << 6)
#define MPU9250_REG_USER_CTRL_FIFO_RST       (1 << 2)
#define MPU9250_REG_USER_CTRL_I2C_MST_EN     (1 << 5)
#define MPU9250_REG_USER_CTRL_I2C_IF_DIS     (1 << 4)

/* Address of the AK8963 accelerometer built in the device. */
#define AK8963_I2C_ADDR   0x0c
#define AK8963_REG_CNTL1  0x0a
#define AK8963_REG_CNTL2  0x0b
#define AK8963_REG_WHOAMI 0x00
#define AK8963_REG_HXL    0x03

// clang-format on

#endif // MPU9250_REGISTERS_H
