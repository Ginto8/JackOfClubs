#ifndef AABB_HPP
#define AABB_HPP

#include "Vector.hpp"
#include "Maybe.hpp"

struct Collision {
    Vec3f overlap;

    Collision(const Collision& other) = default;
    Collision(Vec3f _overlap) : overlap(_overlap) {}
};

struct AABB {
    Vec3f center,size;

    AABB() = default;
    AABB(const AABB& other) = default;
    AABB(Vec3f _center,Vec3f _size) : center(_center),size(_size) {}
    Maybe<Collision> checkCollision(const AABB& other) const;
};

#endif
