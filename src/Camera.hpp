#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Mat44.hpp"

struct Camera {
    Vec3f loc;
    double heading,pitch;

    Camera(Vec3f _loc = Vec3f::ZERO,
           double _heading = 0,
           double _pitch = 0) : 
        loc(_loc),heading{_heading},pitch{_pitch} {}

    void translateRelative(const Vec3f& diff);
    Vec3f viewDirection() const;
    Mat44f matrix() const;
};

#endif
