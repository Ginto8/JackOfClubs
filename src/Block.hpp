#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Vector.hpp"

struct Block {
    bool filled = false;
    Color3f color = WHITE;

    Block() = default;
    Block(bool _filled,Color3f _color) :
        filled{_filled},color{_color} {}
    Block(const Block&) = default;
    Block& operator=(const Block&) = default;
};

#endif
