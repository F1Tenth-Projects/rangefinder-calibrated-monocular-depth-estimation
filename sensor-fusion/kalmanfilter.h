#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <iostream>
#include <vector>
#include <Eigen/Dense>

using namespace Eigen;

class KalmanFilter
{
public:
    KalmanFilter();
    void initialize();
    void predict();
    void update(const VectorXd &measurement);
    VectorXd getState();

private:
    MatrixXd A; // system dynamics matrix
    MatrixXd B; // control input matrix
    MatrixXd C; // measurement matrix
    MatrixXd Q; // process noise covariance matrix
    MatrixXd R; // measurement noise covariance matrix

    VectorXd x; // state vector
    MatrixXd P; // state covariance matrix

    VectorXd y; // measurement residual
    MatrixXd S; // residual covariance matrix
    MatrixXd K; // Kalman gain
};

#endif // KALMANFILTER_H
