#ifndef I2C_H
#define I2C_H

#include <samd51.h>

typedef typeof(SERCOM0) sercom_register;

/* Initializes all I2C interfaces for the application, configuring the SERCOMs
 * to be used for I2C and configuring PORT pinmux settings appropriately. */
void i2c_init(void);

/* Writes to the I2C bus managed by SERCOM 'sercom'. 'size' bytes are written
 * from the buffer 'data', addressed to I2C slave with address 'address'. The
 * 'address' argument is the raw 7-bit address; the left shift is performed
 * internally. Returns 0 on success and -1 on failure. */
int i2c_write(sercom_register sercom, uint8_t address,
              const uint8_t *data, long size);

/* Reads from the I2C slave with address 'address' on the I2C bus managed by
 * SERCOM 'sercom'. 'size' bytes are read from the slave and stored in the
 * buffer 'data'. The 'address' argument is the raw 7-bit address; both the
 * left shift and setting the LSB read bit are handled internally. Returns 0 on
 * success and -1 on failure. */
int i2c_read(sercom_register sercom, uint8_t address,
             uint8_t *data, long size);

#endif
