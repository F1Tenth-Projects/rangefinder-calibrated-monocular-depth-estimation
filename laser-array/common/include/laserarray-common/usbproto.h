#ifndef LASERARRAY_COMMON_USBPROTO_H
#define LASERARRAY_COMMON_USBPROTO_H

#include <laserarray-common/constants.h>

#define LASERARRAY_RQ_RESET      0x01
#define LASERARRAY_RQ_ENABLE     0x02
#define LASERARRAY_RQ_DISABLE    0x03

struct laserarray_sensor_data {
	uint8_t sensor_id;
	uint8_t n_detections;
	uint16_t ranges[LASERARRAY_MAX_DETECTIONS];
} __attribute__((packed));

#endif
