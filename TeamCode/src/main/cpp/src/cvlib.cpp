#include <cvlib.hpp>

#include <Eigen/Core>

#include <AprilTags/TagDetector.h>
#include <AprilTags/Tag36h11.h>

using namespace cvlib;

const double mm_per_tile = 11./18.;
const PosRot aprilTagTransforms[] = {
        {.x=1.,.y=6.,.angle=0.},
        {.x=0.,.y=3.,.angle=CV_PI/2.},
        {.x=1.,.y=0.,.angle=CV_PI},
        {.x=5.,.y=0.,.angle=CV_PI},
        {.x=6.,.y=3.,.angle=3.*CV_PI/2.},
        {.x=5.,.y=6.,.angle=0.}
};

inline PosRot getAprilTagTransform(int aprilTagID) {
    return aprilTagTransforms[aprilTagID-11];
}

//Bound a radian value in the range [-pi, pi]
inline double standardRad(double t) {
    if (t >= 0.) {t = fmod(t + CV_PI, CV_2PI) - CV_PI;}
    else {t = fmod(t - CV_PI, -CV_2PI) + CV_PI;}
    return t;
}

//Convert a rotation matrix to Euler angles
void matToEuler(const Eigen::Matrix3d& rotationMatrix, double& yaw, double& pitch, double& roll) {
    yaw = standardRad(atan2(rotationMatrix(1, 0), rotationMatrix(0, 0)));
    double c = cos(yaw);
    double s = sin(yaw);
    pitch = standardRad(atan2(-rotationMatrix(2,0), rotationMatrix(0,0) * c + rotationMatrix(1,0) * s));
    roll = standardRad(atan2(rotationMatrix(0,2) * s - rotationMatrix(1, 2) * c, -rotationMatrix(0, 1) * s + rotationMatrix(1, 1) * c));
}

PosRot getPlayingFieldPos(const cv::Mat& frame) {
    //We find any AprilTags in the picture
    AprilTags::TagDetector detector(AprilTags::tagCodes36h11);
    std::vector<AprilTags::TagDetection> tags = detector.extractTags(frame);

    //No tag; this function can't help us
    if (tags.empty()) return {.x=-1,.y=-1,.angle=-1.};

    //millimetres
    double tag_size = 10.16;
    Eigen::Vector3d translation;
    Eigen::Matrix3d rotation;

    //I think that the rotation is in radians and the translation is in millimetres...
    //TODO: Find out what fx and fy are for this camera
    tags[0].getRelativeTranslationRotation(tag_size, 600, 600, frame.rows/2., frame.cols/2., translation, rotation);

    //We only care about the X and Y coordinates, since we pretend the field is 2d
    Eigen::Vector2d translation2d{translation[0], translation[1]};

    //We only care about yaw (rotation about the Z axis)
    double yaw, pitch, roll;
    matToEuler(rotation, yaw, pitch, roll);

    //We figure out how the tag is transformed w.r.t. the field
    PosRot absoluteTransform = getAprilTagTransform(tags[0].code);
    Eigen::Vector2d absoluteTrans{absoluteTransform.x, absoluteTransform.y};
    double absoluteYaw = absoluteTransform.angle;

    //We calculate the yaw of the robot
    double robotYaw = absoluteYaw - yaw;

    //We rotate the relative translation vector so that its axes agree with the AprilTag's
    Eigen::Matrix2d rotationMatrix;
    rotationMatrix << cos(robotYaw), sin(robotYaw), -sin(robotYaw), cos(robotYaw);
    Eigen::Vector2d aprilTagTrans = rotationMatrix * translation2d;

    //Finally, we figure out where on the field the robot is
    Eigen::Vector2d robotPos = aprilTagTrans + absoluteTrans;

    return {.x=robotPos[0],.y=robotPos[1],.angle=robotYaw};
}