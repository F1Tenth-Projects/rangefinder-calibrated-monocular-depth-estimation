#ifndef LASERARRAY_COMMON_USBPROTO_H
#define LASERARRAY_COMMON_USBPROTO_H

#include <laserarray-common/constants.h>

#define LASERARRAY_RQ_RESET      0x01
#define LASERARRAY_RQ_ENABLE     0x02
#define LASERARRAY_RQ_DISABLE    0x03

#define LASERARRAY_MSG_SENSOR_DATA    0x01
#define LASERARRAY_MSG_FAULT          0x02

struct laserarray_sensor_data {
	uint8_t msg_type;
	uint8_t sensor_id;
	uint8_t n_detections;
	uint16_t ranges[LASERARRAY_MAX_DETECTIONS];
} __attribute__((packed));

#define LASERARRAY_FAULT_NONE           (0)
#define LASERARRAY_FAULT_GENERAL        (1 << 0)
#define LASERARRAY_FAULT_I2C_COMMS      (1 << 1)
struct laserarray_fault {
	uint8_t msg_type;
	uint8_t sensor_id;
	uint8_t fault;
} __attribute__((packed));

#endif
