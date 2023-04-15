#ifndef SENSOR_H
#define SENSOR_H

#include <vl53lx_api.h>

struct sensor {
	VL53LX_Dev_t dev;
	uint8_t sensor_id;
	uint8_t enabled;
	uint8_t fault;
};

/* Initialize the time-of-flight sensor 's', with sensor ID 'sensor_id' and I2C
 * slave addresss 'i2c_addr'. */
void sensor_init(struct sensor *s, uint8_t sensor_id, uint8_t i2c_addr);

/* Activate/deactivate the the measurement process for sensor 's'. Returns 0 on
 * success, or -1 on error. */
int sensor_enable(struct sensor *s, int should_be_enabled);

#endif
