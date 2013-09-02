#include "AABB.hpp"

bool AABB::checkCollision(const AABB& other,Vec3f* overlapOut) const {
    Vec3f overlap;
    for(int i=0;i<3;++i) {
        float dist = center[i]-other.center[i];
        float minDist = (other.size[i]+size[i])/2;
        if(std::abs(dist) > minDist) {
            return false;
        }
        overlap[i] = dist-(dist > 0 ? minDist : -minDist);
    }
    int minOverlap = 0;
    for(int i=1;i<3;++i) {
        if(std::abs(overlap[i]) < std::abs(overlap[minOverlap])) {
            minOverlap = i;
        }
    }
    overlap[(minOverlap+1)%3] = overlap[(minOverlap+2)%3] = 0;
    if(overlapOut) {
        *overlapOut = overlap;
    }
    return true;
}
