#include "kalmanfilter.h"

KalmanFilter::KalmanFilter() {}

void KalmanFilter::initialize()
{
    // Initialize matrices and vectors
    A = MatrixXd::Identity(x.size(), x.size());
    B = MatrixXd::Zero(x.size(), 1);
    C = MatrixXd::Identity(x.size(), x.size());
    Q = MatrixXd::Zero(x.size(), x.size());
    R = MatrixXd::Zero(x.size(), x.size());

    x = VectorXd::Zero(x.size());
    P = MatrixXd::Zero(x.size(), x.size());

    y = VectorXd::Zero(x.size());
    S = MatrixXd::Zero(x.size(), x.size());
    K = MatrixXd::Zero(x.size(), x.size());
}

void KalmanFilter::predict()
{
    x = A * x;
    P = A * P * A.transpose() + Q;
}

void KalmanFilter::update(const VectorXd &measurement)
{
    y = measurement - C * x;
    S = C * P * C.transpose() + R;
    K = P * C.transpose() * S.inverse();

    x = x + K * y;
    P = (MatrixXd::Identity(x.size(), x.size()) - K * C) * P;
}

VectorXd KalmanFilter::getState()
{
    return x;
}
