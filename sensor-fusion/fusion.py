#!/usr/bin/env python3

def main():
    while True:
        ...

def get_midas_map():
    ''' get_midas_map() -> cv2.Mat

    Returns the greyscale normalized depth map as an OpenCV Mat.
    '''
    raise NotImplementedError()

def get_sensor_data(device):
    ''' get_sensor_data(laserarray_device) -> [[int, ...], ...]

    Returns a list containing lists ranges for each sensor on the
    platform. The outer list will have a fixed size of 5 elements, and
    the inner lists may contain between 0 and 8 range values.
    '''

    raise NotImplementedError()


if __name__ == "__main__":
    main()
