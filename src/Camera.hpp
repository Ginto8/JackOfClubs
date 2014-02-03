#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Mat44.hpp"

struct Camera {
    Vec3f loc;
    double heading,pitch;
    double fovy,aspectRatio;
    double nearPlane,farPlane;

    Camera(Vec3f _loc = Vec3f::ZERO,
           double _heading = 0,double _pitch = 0,
           double _fovy = 45,double _aspectRatio = 1,
           double _nearPlane = 0.01,double _farPlane = 500);

    void constrain();
    Vec3f relativeTranslation(const Vec3f& diff) const;
    Vec3f viewDirection() const;
    Mat44f projection() const;
    Mat44f modelview() const;
    void begin() const;
    void end() const;
private:
    void _setLighting() const;
};

#endif
