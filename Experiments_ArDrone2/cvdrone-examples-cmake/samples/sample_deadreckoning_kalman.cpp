#include "ardrone.h"



// --------------------------------------------------------------------------
// main(Number of arguments, Argument values)
// Description  : This is the entry point of the program.
// Return value : SUCCESS:0  ERROR:-1
// --------------------------------------------------------------------------
int main(int argc, char **argv)
{
    // AR.Drone class
    ARDrone ardrone;

    // Initialize
    if (!ardrone.open()) {
        printf("Failed to initialize.\n");
        return -1;
    }

    // Battery
    printf("Battery = %d%%\n", ardrone.getBatteryPercentage());

    // Map
    cv::Mat map = cv::Mat::zeros(500, 500, CV_8UC3);

    // Kalman filter
    cv::KalmanFilter kalman(6, 4, 0);

    // Sampling time [s]
    const double dt = 0.033;

    // Transition matrix (x, y, z, vx, vy, vz)
    cv::Mat1f F(6, 6);
    F << 1.0, 0.0, 0.0,  dt, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0,  dt, 0.0,
         0.0, 0.0, 1.0, 0.0, 0.0,  dt,
         0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 0.0, 1.0;
    kalman.transitionMatrix = F;

    // Measurement matrix (0, 0, z, vx, vy, vz)
    cv::Mat1f H(4, 6);
    H << 0, 0, 1, 0, 0, 0,
         0, 0, 0, 1, 0, 0,
         0, 0, 0, 0, 1, 0,
         0, 0, 0, 0, 0, 1;
    kalman.measurementMatrix = H;

    // Process noise covairance (x, y, z, vx, vy, vz)
    cv::Mat1f Q(6, 6);
    Q << 0.1, 0.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 0.1, 0.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.1, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 0.3, 0.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 0.3, 0.0,
         0.0, 0.0, 0.0, 0.0, 0.0, 0.3;
    kalman.processNoiseCov = Q;

    // Measurement noise covariance (z, vx, vy, vz)
    cv::Mat1f R(4, 4);
    R << 0.1, 0.0, 0.00, 0.00,
         0.0, 0.1, 0.00, 0.00,
         0.0, 0.0, 0.05, 0.00,
         0.0, 0.0, 0.00, 0.05;
    kalman.measurementNoiseCov = R;

    // Main loop
    while (1) {
        // Key input
        int key = cv::waitKey(33);
        if (key == 0x1b) break;

        // Update
        if (!ardrone.update()) break;

        // Get an image
        cv::Mat image = cv::Mat(ardrone.getImage());

        // Prediction
        cv::Mat prediction = kalman.predict();

        // Altitude
        double altitude = ardrone.getAltitude();

        // Orientations
        double roll  = ardrone.getRoll();
        double pitch = ardrone.getPitch();
        double yaw   = ardrone.getYaw();

        // Velocities
        double vx, vy, vz;
        double velocity = ardrone.getVelocity(&vx, &vy, &vz);
        cv::Mat V  = (cv::Mat1f(3,1) << vx, vy, vz);

        // Rotation matrices
        cv::Mat RZ = (cv::Mat1f(3,3) <<   cos(yaw), -sin(yaw),        0.0,
                                          sin(yaw),  cos(yaw),        0.0,
                                               0.0,       0.0,        1.0);
        cv::Mat RY = (cv::Mat1f(3,3) << cos(pitch),       0.0,  sin(pitch),
                                               0.0,       1.0,        0.0,
                                       -sin(pitch),       0.0,  cos(pitch));
        cv::Mat RX = (cv::Mat1f(3,3) <<        1.0,       0.0,        0.0,
                                               0.0, cos(roll), -sin(roll),
                                               0.0, sin(roll),  cos(roll));

        // Time [s]
        static int64 last = cv::getTickCount();
        double dt = (cv::getTickCount() - last) / cv::getTickFrequency();
        last = cv::getTickCount();

        // Local movements (z, vx, vy, vz)
        cv::Mat1f M = RZ * RY * RX * V * dt;
        cv::Mat measurement = (cv::Mat1f(4,1) << altitude, M(0,0), M(1,0), M(2,0));

        // Correction
        cv::Mat1f estimated = kalman.correct(measurement);

        // Position (x, y, z)
        double pos[3] = {estimated(0,0), estimated(1,0), estimated(2,0)};
        printf("x = %3.2fm, y = %3.2fm, z = %3.2fm\n", pos[0], pos[1], pos[2]);

        // Take off / Landing 
        if (key == ' ') {
            if (ardrone.onGround()) ardrone.takeoff();
            else                    ardrone.landing();
        }

        // Move
        double x = 0.0, y = 0.0, z = 0.0, r = 0.0;
        if (key == KEY_UP)    x = +1.0;
        if (key == KEY_DOWN)  x = -1.0;
        if (key == KEY_RIGHT) r = -1.0;
        if (key == KEY_LEFT)  r = +1.0;
        if (key == 'q')      z =  1.0;
        if (key == 'a')      z = -1.0;
        ardrone.move3D(x, y, z, r);

        // Change camera
        static int mode = 0;
        if (key == 'c') ardrone.setCamera(++mode%4);

        // Display the image
        cv::circle(map, cv::Point(-pos[1]*100.0 + map.cols/2, -pos[0]*100.0 + map.rows/2), 2, CV_RGB(255,0,0));
        cv::imshow("map", map);
        cv::imshow("camera", image);
    }

    // See you
    ardrone.close();

    return 0;
}
