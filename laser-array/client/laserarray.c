#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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

static int is_laserarray_device(libusb_device_handle *handle);
static int maybe_init_usbctx(void);
static int map_libusb_error(int libusb_errno);

static libusb_context *usbctx = NULL;

int laserarray_open(const char *devicepath, laserarray **dev_out)
{
	int i;
	int rc;
	laserarray *dev;

	if ((rc = maybe_init_usbctx()) != 0) {
		return rc;
	}

	dev = malloc(sizeof(*dev));
	if (dev == NULL) {
		rc = -LASERARRAY_EERRNO;
		return rc;
	}
	memset(dev, 0, sizeof(*dev));
	for (i = 0; i < LASERARRAY_NUM_SENSORS; i++) {
		pthread_rwlock_init(&dev->sensor[i].rwlock, NULL);
	}

	dev->devfile = fopen(devicepath, "r+");
	if (dev->devfile == NULL) {
		rc = -LASERARRAY_EERRNO;
		goto err_free_obj;
	}

	rc = libusb_wrap_sys_device(usbctx, fileno(dev->devfile), &dev->usbdev);
	if (rc != 0) {
		rc = map_libusb_error(rc);
		goto err_close_file;
	}

	if ((rc = is_laserarray_device(dev->usbdev)) < 1) {
		if (rc == 0) {
			rc = -LASERARRAY_EBADDEV;
		}
		goto err_close_file;
	}

	if ((rc = libusb_claim_interface(dev->usbdev, 0)) != 0) {
		rc = map_libusb_error(rc);
		goto err_close_dev;
	}

	*dev_out = dev;
	return 0;

err_close_dev:
	libusb_close(dev->usbdev);
err_close_file:
	fclose(dev->devfile);
err_free_obj:
	free(dev);

	return rc;
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
        libusb_release_interface(dev->usbdev, 0);
        libusb_attach_kernel_driver(dev->usbdev, 0);
        libusb_close(dev->usbdev);
        fclose(dev->devfile);
        free(dev);
}

const char *laserarray_strerror(int laserarray_errnum)
{
	switch (abs(laserarray_errnum)) {
	case LASERARRAY_SUCCESS:   return "Success";
	case LASERARRAY_EUNKNOWN:  return "Unknown error";
	case LASERARRAY_EERRNO:    return strerror(errno);
	case LASERARRAY_ENOTIMPL:  return "Function not implemented";
	case LASERARRAY_EBADDEV:   return "Not a laserarray device";
	case LASERARRAY_EINVAL:    return "Invalid argument";
	case LASERARRAY_ENODATA:   return "Data not available";
	case LASERARRAY_ETIMEOUT:  return "Communications timeout";
	case LASERARRAY_EHARDWARE: return "Hardware error";
	case LASERARRAY_ELOSTDEV:  return "Connection to device lost";
	case LASERARRAY_EBUSY:     return "Device busy";
	case LASERARRAY_EIO:       return "Input/output error";
	default:                   return "Unknown error";
	};
}

const char *laserarray_error_name(int laserarray_errnum)
{
	#define ERROR_NAME(name) case name: return #name;
	switch (abs(laserarray_errnum)) {
	ERROR_NAME(LASERARRAY_SUCCESS)
	ERROR_NAME(LASERARRAY_EUNKNOWN)
	ERROR_NAME(LASERARRAY_EERRNO)
	ERROR_NAME(LASERARRAY_ENOTIMPL)
	ERROR_NAME(LASERARRAY_EBADDEV)
	ERROR_NAME(LASERARRAY_EINVAL)
	ERROR_NAME(LASERARRAY_ENODATA)
	ERROR_NAME(LASERARRAY_ETIMEOUT)
	ERROR_NAME(LASERARRAY_EHARDWARE)
	ERROR_NAME(LASERARRAY_ELOSTDEV)
	ERROR_NAME(LASERARRAY_EBUSY)
	ERROR_NAME(LASERARRAY_EIO)
	default: return "";
	};
	#undef ERROR_NAME
}

static int maybe_init_usbctx(void)
{
	if (usbctx == NULL) {
		if (libusb_init(&usbctx) < 0) {
			return -LASERARRAY_EUNKNOWN;
		}
	}

	return 0;
}

static int map_libusb_error(int libusb_errno)
{
	switch (libusb_errno) {
	case 0:
		return 0;
	case LIBUSB_ERROR_TIMEOUT:
		return -LASERARRAY_ETIMEOUT;
	case LIBUSB_ERROR_PIPE:
		return -LASERARRAY_EHARDWARE;
	case LIBUSB_ERROR_NO_DEVICE:
		return -LASERARRAY_ELOSTDEV;
	case LIBUSB_ERROR_BUSY:
		return -LASERARRAY_EBUSY;
	case LIBUSB_ERROR_IO:
		return -LASERARRAY_EIO;
	default:
		return -LASERARRAY_EUNKNOWN;
	}
}

static int is_laserarray_device(libusb_device_handle *handle)
{
	int rc;
	unsigned char stringdesc[sizeof(PRODUCT_STRING) + 1];
	struct libusb_device_descriptor desc;
	libusb_device *dev;

	dev = libusb_get_device(handle);

	rc = libusb_get_device_descriptor(dev, &desc);
	if (rc != 0) {
		return map_libusb_error(rc);
	}

	rc = libusb_get_string_descriptor_ascii(handle, desc.iProduct,
	                                        stringdesc, sizeof(stringdesc));
	if (rc < 0) {
		return map_libusb_error(rc);
	}

	return strcmp((char *) stringdesc, PRODUCT_STRING) == 0;
}
