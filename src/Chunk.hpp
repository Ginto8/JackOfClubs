#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Block.hpp"
#include "VertexArray.hpp"

struct Chunk {
    static constexpr int WIDTH  = 16,
                         HEIGHT = 64;

    Block blocks[WIDTH][WIDTH][HEIGHT];

    void buildVertArray(VertexArray& varr);
};

#endif
