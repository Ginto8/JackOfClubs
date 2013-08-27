#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "Vector.hpp"
#include <vector>
#include <SFML/OpenGL.hpp>

struct Vertex {
    Vec3f loc = Vec3f::ZERO;
    Color3f color = BLACK;
    Vec3f normal = Vec3f{{0,0,1}};

    Vertex() = default;
    Vertex(Vec3f _loc,Color3f _color,Vec3f _normal) :
        loc(_loc),color(_color),normal(_normal) {}
    Vertex(const Vertex&) = default;
};

using VertexArray = std::vector<Vertex>;

void drawVertArray(GLenum shape,const VertexArray& varr);

#endif
