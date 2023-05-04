#include <iostream>
#include <vector>
#include <cmath>
#include "kalmanfilter.h"

// @brief sensor fusion using the depth map histogram and range measurements
// @param depth_histogram(5 histgrams computed from 5 subsets of one greyscale image corresponding to the views of 5 ToF sensors), range_data(5 values from 5 ToF sensors from 5 Tof sensors, each value is the distance from the sensor to the nearest object, might be zero if no object is detected)
// @return fused_depth_map

std::vector<double> performSensorFusion(const std::vector<std::vector<int>> &depth_histograms, const std::vector<int> &range_data)
{
    // Perform sensor fusion for each ToF sensor separately
    std::vector<double> fused_depth_values;

    // Iterate over each ToF sensor
    for (int i = 0; i < depth_histograms.size(); ++i)
    {
        // Compute the histogram for the current ToF sensor's depth data
        const std::vector<int> &hist_depth = depth_histograms[i];

        // Perform linear regression to obtain scaling factor and bias factor
        std::vector<double> factors = linearRegression(hist_depth, range_data);

        // Apply scaling factor and bias factor to depth values of the current ToF sensor
        std::vector<double> adjusted_depth_values;
        for (int j = 0; j < hist_depth.size(); ++j)
        {
            double adjusted_depth = hist_depth[j] * factors[0] + factors[1];
            adjusted_depth_values.push_back(adjusted_depth);
        }

        // Merge adjusted depth values with the fused depth values
        fused_depth_values.insert(fused_depth_values.end(), adjusted_depth_values.begin(), adjusted_depth_values.end());
    }

    // Return the fused depth values
    return fused_depth_values;
}
// std::vector<double> performSensorFusion(const std::vector<int> &depth_histogram, const std::vector<int> &range_data)
// {
//     int num_peaks = std::min(depth_histogram.size(), range_data.size());

//     // Perform linear regression using the range data and depth histogram and get the scaling factor and bias factor for each of the 5 ToF sensors(also 5 slices of the greyscale image)
//     double scaling_factor = 1.0; // Scaling factor initialized to 1
//     double bias_factor = 0.0;    // Bias factor initialized to 0

//     if (num_peaks > 0)
//     {
//         // Calculate the scaling factor using linear regression
//         double sum_depth = 0.0, sum_range = 0.0;
//         double sum_depth_squared = 0.0, sum_range_squared = 0.0;
//         double sum_depth_range = 0.0;

//         for (int i = 0; i < num_peaks; ++i)
//         {
//             sum_depth += depth_histogram[i];
//             sum_range += range_data[i];
//             sum_depth_squared += depth_histogram[i] * depth_histogram[i];
//             sum_range_squared += range_data[i] * range_data[i];
//             sum_depth_range += depth_histogram[i] * range_data[i];
//         }

//         double mean_depth = sum_depth / num_peaks;
//         double mean_range = sum_range / num_peaks;

//         double cov = sum_depth_range / num_peaks - mean_depth * mean_range;
//         double var_depth = sum_depth_squared / num_peaks - mean_depth * mean_depth;

//         scaling_factor = cov / var_depth;
//         bias_factor = mean_range - scaling_factor * mean_depth;
//     }

//     // Create an instance of the KalmanFilter class
//     KalmanFilter kalmanFilter;

//     // Initialize the Kalman filter
//     kalmanFilter.initialize();

//     // Define the state vector and measurement vector for the Kalman filter
//     VectorXd state(num_peaks);       // [depth_1, depth_2, ..., depth_n]
//     VectorXd measurement(num_peaks); // [depth_1]

//     // Perform sensor fusion using the Kalman filter
//     std::vector<double> fused_depth_map(depth_histogram.size(), 0.0);
//     for (int i = 0; i < depth_histogram.size(); ++i)
//     {
//         // Set the measurement vector
//         measurement(0) = depth_histogram[i];

//         // Set the state vector
//         for (int j = 0; j < num_peaks; ++j)
//         {
//             state(j) = depth_histogram[i] * scaling_factor + bias_factor;
//         }

//         // Predict the next state using the Kalman filter
//         kalmanFilter.predict();

//         // Update the state using the measurement
//         kalmanFilter.update(measurement);

//         // Get the fused depth estimate from the Kalman filter
//         VectorXd fused_state = kalmanFilter.getState();
//         double fused_depth = fused_state(0);

//         // Store the fused depth estimate in the depth map
//         fused_depth_map[i] = fused_depth;
//     }

//     // Return the fused depth map
//     return fused_depth_map;
// }
