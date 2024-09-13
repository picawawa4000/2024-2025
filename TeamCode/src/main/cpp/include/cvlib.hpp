#ifndef INC_2024_2025_B_CVLIB_HPP
#define INC_2024_2025_B_CVLIB_HPP

#include <opencv2/core.hpp>

namespace cvlib {
    struct PosRot {
        double x, y, angle;
    };

    /**
     * This function operates in 2D.
     * (0, 0) is the blue basket, (6, 0) is the red park,
     * (6, 6) is the red basket, (0, 6) is the blue park
     * 0 radians is towards the wall between (0, 0) and (6, 0)
     * Radians increase clockwise so that, if the robot rotates
     * by however many radians this function returns, it is
     * facing zero degrees.
     * **/
    PosRot getPlayingFieldPos(cv::Mat frame);
}

#endif //INC_2024_2025_B_CVLIB_HPP
