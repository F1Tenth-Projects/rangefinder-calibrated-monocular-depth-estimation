#include <string.h>
#include <panic.h>
#include <i2c.h>
#include <usb.h>
#include <usb/requests.h>
#include <laserarray-common/usbproto.h>
#include <vl53lx_api.h>

static void ctrl_xfer_handler(void *ctx, const struct usb_request *r,
                              const uint8_t *data, int data_size);

VL53LX_Dev_t dev;

int main()
{
	int rc;

	usb_init();
	usb_set_ctl_callback(&ctrl_xfer_handler, NULL);

	i2c_init();

	memset(&dev, 0, sizeof(dev));
	dev.i2c_slave_address = 0x29;
	dev.comms_type = 0;
	dev.comms_speed_khz = 100;
	while (VL53LX_WaitDeviceBooted(&dev) == 0);
	if ((rc = VL53LX_DataInit(&dev)) != 0) {
		panic();
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
	switch (r->bRequest) {
	case LASERARRAY_RQ_RESET:
		usb_stall_ep(0);
		break;
	case LASERARRAY_RQ_ENABLE:
		if (r->wIndex != 0) {
			usb_stall_ep(0);
		}
		if (VL53LX_StartMeasurement(&dev) != 0) {
			usb_stall_ep(0);
		}
		usb_send_zlp(0);
		break;
	case LASERARRAY_RQ_DISABLE:
		if (r->wIndex != 0) {
			usb_stall_ep(0);
		}
		if (VL53LX_StopMeasurement(&dev) != 0) {
			usb_stall_ep(0);
		}
		usb_send_zlp(0);
		break;
	default:
		usb_stall_ep(0);
		break;
	}
}
