#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Block.hpp"
#include "Vertex.hpp"

struct Chunk {
    static constexpr int WIDTH  = 16,
                         HEIGHT = 64;

    static constexpr int SEALEVEL   = 32,
                         DIRTLAYERS =  5;
    static constexpr Color3f ROCKCOLOR = {  0.7,  0.7, 0.7 },
                             DIRTCOLOR = { 0.59, 0.29,   0 };

    static void generate(Chunk& c);

    Block blocks[WIDTH][WIDTH][HEIGHT];

    void buildVertArray(VertexArray& varr);
};

#endif
