
#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Block.hpp"
#include "Vertex.hpp"

struct Chunk {
    static constexpr int WIDTH  = 16,
                         HEIGHT = 80;


    static void generate(Chunk& c);

    Block blocks[WIDTH][WIDTH][HEIGHT];
};

#endif
