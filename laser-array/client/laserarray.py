''' Laser Array Client Library

Python bindings for the C client library providing an interface to
liblaserarray.so
'''

import ctypes
from ctypes import c_int, c_uint8, c_uint16, c_void_p, c_char_p, c_size_t, \
                   c_uint, c_ushort, c_ulong, c_longlong
from ctypes import pointer as _ptr
from ctypes import POINTER as _PTR
import struct


LASERARRAY_MAX_DETECTIONS = 8


class LaserArray:
    def __init__(self, device_path):
        self._devptr = c_void_p()
        rc = lib.laserarray_open(device_path.encode('ascii'),
                                 _ptr(self._devptr))
        if rc < 0:
            _raise(rc, "Unable to open device '%s'" % device_path);

    def enable_sensor(self, sensor_id):
        rc = lib.laserarray_enable_sensor(self._devptr, sensor_id)
        if rc != 0:
            _raise(rc, "Could not enable sensor")

    def disable_sensor(self, sensor_id):
        rc = lib.laserarray_disable_sensor(self._devptr, sensor_id)
        if rc != 0:
            _raise(rc, "Could not disable sensor")

    def get_detections(self, sensor_id):
        c_ranges_mm = (c_ushort * LASERARRAY_MAX_DETECTIONS)()
        c_timestamp = c_longlong();
        rc = lib.laserarray_get_detections(self._devptr, sensor_id,
                                           c_ranges_mm, _ptr(c_timestamp))
        if rc < 0:
            _raise(rc, "Could not get detections")

        ranges = []
        for i in range(0, rc):
            ranges.append(c_ranges_mm[i])

        timestamp_int = struct.unpack('q', c_timestamp)[0]
        timestamp = timestamp_int / 1e6
        return timestamp, ranges


    def __del__(self):
        if self._devptr:
            lib.laserarray_close(self._devptr)


def _raise(rc, message, exctype=RuntimeError):
    message = message + ": %s (%s)" % (
                         lib.laserarray_strerror(rc).decode('ascii'),
                         lib.laserarray_error_name(rc).decode('ascii'))
    raise exctype(message)


lib = ctypes.CDLL("liblaserarray.so")

lib.laserarray_open.arg_types = [c_char_p, c_void_p]
lib.laserarray_open.restype = c_int

lib.laserarray_enable_sensor.arg_types = [c_void_p, c_int]
lib.laserarray_enable_sensor.restype = c_int

lib.laserarray_disable_sensor.arg_types = [c_void_p, c_int]
lib.laserarray_disable_sensor.restype = c_int

lib.laserarray_get_detections.arg_types = [c_void_p, c_int, _PTR(c_ushort),
                                           _PTR(c_longlong)]
lib.laserarray_get_detections.restype = c_int

lib.laserarray_close.arg_types = [c_void_p]

lib.laserarray_strerror.arg_types = [c_int]
lib.laserarray_strerror.restype = c_char_p

lib.laserarray_error_name.arg_types = [c_int]
lib.laserarray_error_name.restype = c_char_p
