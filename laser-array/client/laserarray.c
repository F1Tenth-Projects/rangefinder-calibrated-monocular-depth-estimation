#include <stdio.h>
#include <pthread.h>
#include <libusb.h>
#include <laserarray.h>
#include <laserarray-common/usbproto.h>

#define USB_VID         0x03EB  /* Atmel */
#define USB_PID         0x2423  /* USB_PID_ATMEL_ASF_VENDOR_CLASS */
#define PRODUCT_STRING  "F1TENTH VL53L4CX Array"

struct laserarray {
	FILE *devfile;
	libusb_device_handle *usbdev;

	struct {
		pthread_rwlock_t rwlock;
		int64_t update_time_us; /* UNIX time of last update in us */
		uint16_t ranges[LASERARRAY_MAX_DETECTIONS];
		uint8_t n_detections;
		uint8_t enabled;
	} sensor[LASERARRAY_NUM_SENSORS];
};


int laserarray_open(const char *devicepath, laserarray **dev_out)
{
	return -LASERARRAY_ENOTIMPL;
}

int laserarray_enable_sensor(laserarray *dev, int sensor_id)
{
	return -LASERARRAY_ENOTIMPL;
}

int laserarray_disable_sensor(laserarray *dev, int sensor_id)
{
	return -LASERARRAY_ENOTIMPL;
}

int laserarray_get_detections(const laserarray *dev, int sensor_id,
                              uint16_t *ranges, uint64_t *timestamp_us)
{
	return -LASERARRAY_ENOTIMPL;
}

int laserarray_reset(laserarray *dev, int sensor_id)
{
	return -LASERARRAY_ENOTIMPL;
}

void laserarray_close(laserarray *dev)
{
	return -LASERARRAY_ENOTIMPL;
}
