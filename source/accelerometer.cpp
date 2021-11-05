#include "accelerometer.h"
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <pico/binary_info.h>

#define I2C_INTERFACE i2c1
#define I2C_DEV_ADDR  0x68
#define I2C_SDA_PIN   18
#define I2C_SCL_PIN   19

static void mpu6050_reset() {
  // Two byte reset. First byte register, second byte data
  // There are a load more options to set up the device in different ways that could be added here
  uint8_t buf[] = {0x6B, 0x00};
  i2c_write_blocking(I2C_INTERFACE, I2C_DEV_ADDR, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
  // For this particular device, we send the device the register we want to read
  // first, then subsequently read from the device. The register is auto incrementing
  // so we don't need to keep sending the register we want, just the first.

  uint8_t buffer[6];

  // Start reading acceleration registers from register 0x3B for 6 bytes
  uint8_t val = 0x3B;
  i2c_write_blocking(I2C_INTERFACE, I2C_DEV_ADDR, &val, 1, true); // true to keep master control of bus
  i2c_read_blocking(I2C_INTERFACE, I2C_DEV_ADDR, buffer, 6, false);

  for (int i = 0; i < 3; i++) {
    accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
  }

  // Now gyro data from reg 0x43 for 6 bytes
  // The register is auto incrementing on each read
  val = 0x43;
  i2c_write_blocking(I2C_INTERFACE, I2C_DEV_ADDR, &val, 1, true);
  i2c_read_blocking(I2C_INTERFACE, I2C_DEV_ADDR, buffer, 6, false);  // False - finished with bus

  for (int i = 0; i < 3; i++) {
    gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);;
  }

  // Now temperature from reg 0x41 for 2 bytes
  // The register is auto incrementing on each read
  val = 0x41;
  i2c_write_blocking(I2C_INTERFACE, I2C_DEV_ADDR, &val, 1, true);
  i2c_read_blocking(I2C_INTERFACE, I2C_DEV_ADDR, buffer, 2, false);  // False - finished with bus

  *temp = buffer[0] << 8 | buffer[1];
}

void accel_init(void)
{
  i2c_init(I2C_INTERFACE, 400 * 1000);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA_PIN);
  gpio_pull_up(I2C_SCL_PIN);
  // Make the I2C pins available to picotool
  bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

  mpu6050_reset();
}

void accel_get_data(float* data)
{
  int16_t acceleration_raw[3], gyro[3], temp;

  mpu6050_read_raw(acceleration_raw, gyro, &temp);

  for(int i=0; i<3; i++) {
    // convert raw reading to m/s2 assuming accelerometer range is 2g
    data[i] = (float)acceleration_raw[i]/1638.40f;
  }
}
