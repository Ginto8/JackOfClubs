#include "AABB.hpp"

Maybe<Collision> AABB::checkCollision(const AABB& other,
                                      bool calcMinOverlap) const {
    Vec3f overlap;
    for(int i=0;i<3;++i) {
        float low       = center[i]-std::abs(size[i]/2),
              high      = center[i]+std::abs(size[i]/2),
              otherLow  = other.center[i]-std::abs(other.size[i]/2),
              otherHigh = other.center[i]+std::abs(other.size[i]/2);
        if(high <= otherLow || low >= otherHigh) {
            return {};
        }
        float overlaps[] = { otherLow-high,otherHigh-low };
        overlap[i] = std::abs(overlaps[0]) < std::abs(overlaps[1]) ?
                     overlaps[0] : overlaps[1];
    }
    if(calcMinOverlap) {
        int minOverlap = 0;
        for(int i=1;i<3;++i) {
            if(std::abs(overlap[i]) < std::abs(overlap[minOverlap])) {
                minOverlap = i;
            }
        }
        overlap[(minOverlap+1)%3] = overlap[(minOverlap+2)%3] = 0;
    }
    return {{overlap}};
}
