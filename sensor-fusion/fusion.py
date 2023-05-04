#!/usr/bin/env python3

def main():
    while True:
        midas_map = get_midas_map()
        sensor_data = get_sensor_data()
        absolute_depth_map = fuse_data(midas_map, sensor_data)
        publish_laserscan(absolute_depth_map)

def fuse_data(midas_map, sensor_data):
    ''' fuse_data(midas_map, [[range, ...], ...]) -> cv2.Mat
    
    Takes the normalized depth map and range measurements from each
    sensor and fuses them into a depth map with absolute range values.
    Returns a cv2.Map object containing absolute depth measurements.
    '''
    img = []
    img.append(midas_map[101:154, 0:49])
    img.append(midas_map[101:154, 49:102])
    img.append(midas_map[101:154, 102:155])
    img.append(midas_map[101:154, 155:208])
    img.append(midas_map[101:154, 208:256])

    scale_factors = []
    for i in range(0, len(img)):
        scale = calc_scale_factor(img[i], sensor_data[i])
        scale_factors.append(scale)

    merged_scale = merge_scaling_factors(scale_factors)

    return midas_map * merged_scale

def calc_scale_factor(imageslice, rangelist):
    ''' calc_scale_factor(imageslice, [range, ...]) -> float

    Takes the segment 'imageslice' of a normalized depth map and
    determines a floating point scaling factor to convert the
    normalized distances to absolute distances.
    '''
    raise NotImplementedError()

def merge_scale_factor(scale_list):
    ''' merge_scale_factor([scale, ...]) -> float

    Intelligently determine the global scaling factor for the depth
    map based on the list of image segment scaling factors provided. 
    '''
    raise NotImplementedError()

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

def publish_laserscan(depth_map):
    ''' publish_laserscan(depth_map):

    Publishes the ROS LaserScan message using the scaled depth map data.
    'depth_map' is a cv2.Mat object.
    '''
    raise NotImplementedError()


if __name__ == "__main__":
    main()
