#include <string.h>
#include <panic.h>
#include <i2c.h>
#include <usb.h>
#include <usb/requests.h>
#include <sensor.h>
#include <laserarray-common/usbproto.h>


static void ctrl_xfer_handler(void *ctx, const struct usb_request *r,
                              const uint8_t *data, int data_size);


int main()
{
	int i;
	struct sensor sensors[LASERARRAY_NUM_SENSORS];

	usb_init();
	usb_set_ctl_callback(&ctrl_xfer_handler, sensors);

	i2c_init();

	for (i = 0; i < LASERARRAY_NUM_SENSORS; i++) {
		sensor_init(&sensors[i], i, 0x29);
	}

	usb_attach();

	while (1) {
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
		if (r->wIndex != 0) {
			usb_stall_ep(0);
		}
		sensor_enable(&sensors[r->wIndex], 1);
		usb_send_zlp(0);
		break;
	case LASERARRAY_RQ_DISABLE:
		if (r->wIndex != 0) {
			usb_stall_ep(0);
		}
		sensor_enable(&sensors[r->wIndex], 0);
		usb_send_zlp(0);
		break;
	default:
		usb_stall_ep(0);
		break;
	}
}
