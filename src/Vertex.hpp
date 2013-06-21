#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "Vector.hpp"
#include <vector>
#include <SFML/OpenGL.hpp>

struct Vertex {
    Vec3f loc = Vec3f::ZERO;
    Color3f color = BLACK;

    Vertex() = default;
    Vertex(Vec3f _loc,Color3f _color) :
        loc(_loc),color(_color) {}
    Vertex(const Vertex&) = default;
};

using VertexArray = std::vector<Vertex>;

void drawVertArray(GLenum shape,const VertexArray& varr);

#endif
