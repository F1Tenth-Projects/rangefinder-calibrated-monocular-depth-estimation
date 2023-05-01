#ifdef __cplusplus
extern "C" {
#endif

#ifndef LASERARRAY_H
#define LASERARRAY_H

#include <stdint.h>
#include <laserarray-common/constants.h>

#define LASERARRAY_SUCCESS      0  /* Success */
#define LASERARRAY_EUNKNOWN     1  /* Unknown error */
#define LASERARRAY_EERRNO       2  /* Check libc errno */
#define LASERARRAY_ENOTIMPL     3  /* Function not implemented */
#define LASERARRAY_EBADDEV      4  /* Not a laserarray device */
#define LASERARRAY_EINVAL       5  /* Invalid argument */
#define LASERARRAY_ENODATA      6  /* Data not available */
#define LASERARRAY_ETIMEOUT     7  /* Communications timeout */
#define LASERARRAY_EHARDWARE    8  /* Hardware error */
#define LASERARRAY_ELOSTDEV     9  /* Connection to device lost */
#define LASERARRAY_EBUSY       10  /* Device busy */
#define LASERARRAY_EIO         11  /* Input/output error */

struct laserarray;
typedef struct laserarray laserarray;


/* Opens the laserarray USB device associated with the /dev character device
 * file at path 'devicepath'. On success, *dev_out is set to a valid pointer to
 * a laserarray_dev handle, and 0 is returned. On failure, a negative error
 * code is returned. */
int laserarray_open(const char *devicepath, laserarray **dev_out);

/* Commands the sensor element with ID 'sensor_id' to begin ranging
 * measurements and provide data. Sensor IDs are numbered from left to right,
 * starting with zero. Returns zero if the enable operation was successful, and
 * a negative error code on failure. */
int laserarray_enable_sensor(laserarray *dev, int sensor_id);

/* Commands the sensor element with ID 'sensor_id' to cease ranging
 * measurements. Sensor IDs are numbered from left to right, starting with
 * zero. Returns zero if the enable operation was successful, and a negative
 * error code on failure. */
int laserarray_disable_sensor(laserarray *dev, int sensor_id);

/* Retrieves the range data for the most recent detections received from the
 * sensor element with ID 'sensor_id'. On success, the range (in millimeters)
 * of each detection is stored in the array pointed to by 'ranges_mm' (which
 * must have a size of at least LASERARRAY_MAX_DETECTIONS), the timestamp (in
 * UNIX time microseconds) with the corresponding receive time is stored in
 * 'timestamp_us', and the number of valid ranges is is returned. On error, a
 * negative error code is returned. */
int laserarray_get_detections(laserarray *dev, int sensor_id,
                              uint16_t *ranges_mm, int64_t *timestamp_us);

/* Toggles the XSHUT signal  to the sensor element with ID 'sensor_id', forcing
 * it to reset. This may be necessary if the I2C interface enters an
 * inconsistent state and the system is otherwise unable to recover. Sensor IDs
 * are numbered from left to right, starting with zero. Returns zero if the
 * reset operation was successful, and a negative error code on failure. */
int laserarray_reset(laserarray *dev, int sensor_id);

/* Closes the device 'dev' and frees all resources associated with it. */
void laserarray_close(laserarray *dev);

/* Returns the string description for the error code 'laserarray_errnum'. */
const char *laserarray_strerror(int laserarray_errnum);

/* Returns the string representation of the #define constant name for the error
 * code 'laserarray_errnum'. */
const char *laserarray_error_name(int laserarray_errnum);


#endif

#ifdef __cplusplus
}
#endif
