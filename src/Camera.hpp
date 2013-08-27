#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Mat44.hpp"

struct Camera {
    Vec3f loc;
    double heading,pitch;

    Color4f diffuse = {1,1,1,1},
            ambient = {0.5,0.5,0.5,1};

    Camera(Vec3f _loc = Vec3f::ZERO,
           double _heading = 0,
           double _pitch = 0);

    void translateRelative(const Vec3f& diff);
    Vec3f viewDirection() const;
    Mat44f matrix() const;
    void setLighting() const;
};

#endif
