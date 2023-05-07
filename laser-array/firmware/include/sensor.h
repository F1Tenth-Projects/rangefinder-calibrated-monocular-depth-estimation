#ifndef SENSOR_H
#define SENSOR_H

#include <vl53lx_api.h>

struct sensor {
	VL53LX_Dev_t dev;
	uint8_t xshut_port_bank;
	uint8_t xshut_port_pin;
	uint8_t sensor_id;
	uint8_t enabled;
	uint8_t fault;
};

/* Initialize the time-of-flight sensor 's', with sensor ID 'sensor_id' and I2C
 * slave addresss 'i2c_addr'. 'xshut_bank' and 'xshut_pin' refer to the GPIO
 * group and pin IDs for the GPIO pin attached to the sensor's XSHUT pin. The
 * XSHUT pin for all other uninitialized sensors must be low while calling
 * sensor_init so that the I2C address can be set uniquely. */
void sensor_init(struct sensor *s, uint8_t sensor_id, uint8_t i2c_addr,
                 uint8_t xshut_bank, uint8_t xshut_pin);

/* Activate/deactivate the the measurement process for sensor 's'. Returns 0 on
 * success, or -1 on error. */
int sensor_enable(struct sensor *s, int should_be_enabled);

/* Run an iteration of the main processing loop for the sensor 's'. If data is
 * available, it is processed and transmitted on endpoint 1. Otherwise, the
 * function simply returns. If a fault occurs, s->fault is updated accordingly
 * and a fault message is sent to the host. */
void sensor_process(struct sensor *s);

#endif
