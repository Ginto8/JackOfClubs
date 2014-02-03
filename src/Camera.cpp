#include "Camera.hpp"
#include "Chunk.hpp"
#include <SFML/OpenGL.hpp>

Camera::Camera(Vec3f _loc,
               double _heading,double _pitch,
               double _fovy,double _aspectRatio,
               double _nearPlane,double _farPlane) :
     loc(_loc),heading{_heading},pitch{_pitch},
     fovy{_fovy},aspectRatio{_aspectRatio},
     nearPlane{_nearPlane},farPlane{_farPlane} {
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    Color4f ambient[] = {{{0,0,0,1}},{{0.3,0.3,0.3,1}}},
            diffuse[] = {{{1,1,1,1}},{{1,1,1,1}}},
            specular  = {{0.3,0.3,0.3,0.3}};
    glLightfv(GL_LIGHT0,GL_AMBIENT,&ambient[0][0]);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,&diffuse[0][0]);
    glLightfv(GL_LIGHT0,GL_SPECULAR,&specular[0]);
    glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,1.0/Chunk::WIDTH);
    glLightfv(GL_LIGHT1,GL_AMBIENT,&ambient[1][0]);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,&diffuse[1][0]);
    glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,1.0/(5*Chunk::WIDTH));
}

void Camera::constrain() {
    if(pitch > 90) {
        pitch = 90;
    }
    if(pitch < -90) {
        pitch = -90;
    }
}

Vec3f Camera::relativeTranslation(const Vec3f& diff) const {
    auto absDiff = Mat44f::rotation(-heading,0,1,0)*diff;
    return Vec3f{{ absDiff[0],absDiff[1],absDiff[2] }};
}

Vec3f Camera::viewDirection() const {
    auto v4 =  Mat44f::rotation(-heading, 0,1,0)
               *Mat44f::rotation(-pitch, -1,0,0)
               *Vec3f{{0,0,-1}};
    return Vec3f{{v4[0],v4[1],v4[2]}};
}

Mat44f Camera::projection() const {
    return Mat44f::perspectiveProjection(fovy,aspectRatio,
                                         nearPlane,farPlane);
}

Mat44f Camera::modelview() const {
    return Mat44f::rotation(pitch,  -1,0,0)
           *Mat44f::rotation(heading, 0,1,0)
           *Mat44f::translation(-loc);
}

void Camera::begin() const {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(projection().values);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(modelview().values);
    glEnable(GL_LIGHTING);
    _setLighting();
}

void Camera::end() const {
    glDisable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Camera::_setLighting() const {
    Vec4f lightLoc{{loc[0],loc[1],loc[2],1}};
    glLightfv(GL_LIGHT0,GL_POSITION,&lightLoc[0]);
    Vec4f light2Loc{{Chunk::WIDTH/2,Chunk::HEIGHT,Chunk::WIDTH/2,1}};
    glLightfv(GL_LIGHT1,GL_POSITION,&light2Loc[0]);
}
