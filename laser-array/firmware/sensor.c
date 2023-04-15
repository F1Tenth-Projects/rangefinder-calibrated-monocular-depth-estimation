#include <usb.h>
#include <sensor.h>
#include <panic.h>
#include <vl53lx_api.h>
#include <laserarray-common/usbproto.h>

static void handle_fault(struct sensor *s, VL53LX_Error e);

void sensor_init(struct sensor *s, uint8_t sensor_id, uint8_t i2c_addr)
{
	int rc;

	memset(s, 0, sizeof(*s));
	s->sensor_id = sensor_id;
	s->dev.i2c_slave_address = i2c_addr;
	s->dev.comms_type = 0;
	s->dev.comms_speed_khz = 100;

	while (VL53LX_WaitDeviceBooted(&s->dev) == 0);
	if ((rc = VL53LX_DataInit(&s->dev)) != 0) {
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
	} else if (s->enabled && !should_be_enabled) {
		if ((rc = VL53LX_StopMeasurement(&s->dev)) != 0) {
			handle_fault(s, rc);
			return -1;
		}
	}

	return 0;
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
