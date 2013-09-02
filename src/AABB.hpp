#ifndef AABB_HPP
#define AABB_HPP

#include "Vector.hpp"

struct AABB {
    Vec3f center,size;

    bool checkCollision(const AABB& other,Vec3f* overlapOut=nullptr) const;
};

#endif
