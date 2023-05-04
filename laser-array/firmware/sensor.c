#include <samd51.h>
#include <usb.h>
#include <sensor.h>
#include <panic.h>
#include <gpio.h>
#include <vl53lx_api.h>
#include <laserarray-common/usbproto.h>

static void handle_fault(struct sensor *s, VL53LX_Error e);

void sensor_init(struct sensor *s, uint8_t sensor_id, uint8_t i2c_addr,
                 uint8_t xshut_bank, uint8_t xshut_pin)
{
	/* Make 'rc' volatile so it isn't optimized out and can be checked from
	 * the debugger on an initialization failure. */
	volatile int rc;

	memset(s, 0, sizeof(*s));
	s->sensor_id = sensor_id;
	s->xshut_port_pin = xshut_pin;
	s->xshut_port_bank = xshut_bank;
	s->dev.i2c_slave_address = 0x29;  // default address
	s->dev.comms_type = 0;
	s->dev.comms_speed_khz = 100;

	gpio_set_value(xshut_bank, xshut_pin, 1);

	while (VL53LX_WaitDeviceBooted(&s->dev) == 0);
	if ((rc = VL53LX_SetDeviceAddress(&s->dev, i2c_addr << 1)) != 0) {
		panic();
	}

	s->dev.i2c_slave_address = i2c_addr;
	if ((rc = VL53LX_DataInit(&s->dev)) != 0) {
		panic();
	}

	rc = VL53LX_SetDistanceMode(&s->dev, VL53LX_DISTANCEMODE_LONG);
	if (rc != 0) {
		panic();
	}

	rc = VL53LX_SetTuningParameter(&s->dev,
	                               VL53LX_TUNINGPARM_RESET_MERGE_THRESHOLD,
				       15000 + 5000);
	if (rc != 0) {
		panic();
	}
}

int sensor_enable(struct sensor *s, int should_be_enabled)
{
	int rc;

	if (!s->enabled && should_be_enabled) {
		if ((rc = VL53LX_StartMeasurement(&s->dev)) != 0) {
			handle_fault(s, rc);
			return -1;
		}
		s->enabled = 1;
	} else if (s->enabled && !should_be_enabled) {
		if ((rc = VL53LX_StopMeasurement(&s->dev)) != 0) {
			handle_fault(s, rc);
			return -1;
		}
		s->enabled = 0;
	}

	return 0;
}

void sensor_process(struct sensor *s)
{
	int rc;
	uint8_t ready;

	if (!s->enabled || s->fault) {
		return;
	}

	ready = 0;
	rc = VL53LX_GetMeasurementDataReady(&s->dev, &ready);
	if (rc < 0) {
		handle_fault(s, rc);
		return;
	} else if (ready) {
		int i;
		struct laserarray_sensor_data msg;
		VL53LX_MultiRangingData_t sensor_data;

		rc = VL53LX_GetMultiRangingData(&s->dev, &sensor_data);
		if (rc < 0) {
			handle_fault(s, rc);
			return;
		}

		rc = VL53LX_ClearInterruptAndStartMeasurement(&s->dev);
		if (rc < 0) {
			handle_fault(s, rc);
		}

		memset(&msg, 0, sizeof(msg));
		msg.msg_type = LASERARRAY_MSG_SENSOR_DATA;
		msg.sensor_id = s->sensor_id;
		msg.n_detections = sensor_data.NumberOfObjectsFound;
		for (i = 0; i < sensor_data.NumberOfObjectsFound; i++) {
			msg.ranges[i] = sensor_data.RangeData[i]
			                           .RangeMilliMeter;
		}

		usb_send_nonblocking(1, (uint8_t *) &msg, sizeof(msg));
	}
}

static void handle_fault(struct sensor *s, VL53LX_Error e)
{
	struct laserarray_fault msg;

	switch (e) {
	case -13:
		s->fault |= LASERARRAY_FAULT_I2C_COMMS;
		break;
	default:
		s->fault |= LASERARRAY_FAULT_GENERAL;
		break;
	}

	msg.msg_type = LASERARRAY_MSG_FAULT;
	msg.sensor_id = s->sensor_id;
	msg.fault = s->fault;
	usb_send(1, (uint8_t *) &msg, sizeof(msg));
}
