#include "Camera.hpp"

void Camera::translateRelative(const Vec3f& diff) {
    auto absDiff = Mat44f::rotation(-heading,0,1,0)*diff;
    loc += Vec3f({ absDiff[0],absDiff[1],absDiff[2] });
}

Vec3f Camera::viewDirection() const {
    auto v4 =  Mat44f::rotation(-heading, 0,1,0)
               *Mat44f::rotation(-pitch, -1,0,0)
               *Vec3f({0,0,-1});
    return Vec3f({v4[0],v4[1],v4[2]});
}

Mat44f Camera::matrix() const {
    return Mat44f::rotation(pitch,  -1,0,0)
           *Mat44f::rotation(heading, 0,1,0)
           *Mat44f::translation(-loc);
}
