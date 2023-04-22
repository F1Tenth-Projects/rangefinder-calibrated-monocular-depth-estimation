#include <string.h>
#include <panic.h>
#include <i2c.h>
#include <gpio.h>
#include <usb.h>
#include <usb/requests.h>
#include <sensor.h>
#include <laserarray-common/usbproto.h>


static void ctrl_xfer_handler(void *ctx, const struct usb_request *r,
                              const uint8_t *data, int data_size);

const struct {
	uint8_t xshut_bank;
	uint8_t xshut_pin;
	uint8_t i2c_addr;
} sensor_cfg[LASERARRAY_NUM_SENSORS] = {
	{2, 0, 0x10},
	{2, 1, 0x11}
};

int main()
{
	int i;
	struct sensor sensors[LASERARRAY_NUM_SENSORS];

	usb_init();
	usb_set_ctl_callback(&ctrl_xfer_handler, sensors);

	i2c_init();

	for (i = 0; i < LASERARRAY_NUM_SENSORS; i++) {
		gpio_set_direction(sensor_cfg[i].xshut_bank,
		                   sensor_cfg[i].xshut_pin, 1);
		gpio_set_value(sensor_cfg[i].xshut_bank,
		               sensor_cfg[i].xshut_pin, 0);
	}

	for (i = 0; i < LASERARRAY_NUM_SENSORS; i++) {
		sensor_init(&sensors[i], i, sensor_cfg[i].i2c_addr,
		            sensor_cfg[i].xshut_bank, sensor_cfg[i].xshut_pin);
	}

	usb_attach();

	while (1) {
		for (i = 0; i < LASERARRAY_NUM_SENSORS; i++) {
			sensor_process(&sensors[i]);
		}
		usb_process();
	}

	return 0;
}

static void ctrl_xfer_handler(void *ctx, const struct usb_request *r,
                              const uint8_t *data, int data_size)
{
	struct sensor *sensors;

	sensors = ctx;

	switch (r->bRequest) {
	case LASERARRAY_RQ_RESET:
		usb_stall_ep(0);
		break;
	case LASERARRAY_RQ_ENABLE:
		if (r->wIndex < LASERARRAY_NUM_SENSORS) {
			sensor_enable(&sensors[r->wIndex], 1);
			usb_send_zlp(0);
		} else {
			usb_stall_ep(0);
		}
		break;
	case LASERARRAY_RQ_DISABLE:
		if (r->wIndex < LASERARRAY_NUM_SENSORS) {
			sensor_enable(&sensors[r->wIndex], 0);
			usb_send_zlp(0);
		} else {
			usb_stall_ep(0);
		}
		break;
	default:
		usb_stall_ep(0);
		break;
	}
}
