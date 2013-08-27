#include "Camera.hpp"
#include <SFML/OpenGL.hpp>

Camera::Camera(Vec3f _loc,
       double _heading,
       double _pitch) :
     loc(_loc),heading{_heading},pitch{_pitch} {
    //glEnable(GL_LIGHT0);
}
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

void Camera::setLighting() const {
    glLightfv(GL_LIGHT0,GL_AMBIENT,&ambient[0]);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,&diffuse[0]);
    Vec4f lightLoc = {loc[0],loc[1],loc[2],1};
    glLightfv(GL_LIGHT0,GL_POSITION,&lightLoc[0]);
}
