#include "Chunk.hpp"
#include <ctime>
#include <cstdlib>

constexpr Color3f Chunk::ROCKCOLOR,
                  Chunk::DIRTCOLOR;

void Chunk::generate(Chunk& c) {
    for(int x=0;x<WIDTH;++x) {
        for(int z=0;z<WIDTH;++z) {
            int height = SEALEVEL + (std::rand()%8-4);
            for(int y=0;y<HEIGHT;++y) {
                auto& block = c.blocks[x][z][y];
                if(y <= height) {
                    block.filled = true;
                    block.color = (height-y < DIRTLAYERS) ?
                                                DIRTCOLOR :
                                                ROCKCOLOR;
                } else {
                    block.filled = false;
                }
            }
        }
    }
}

void Chunk::buildVertArray(VertexArray& varr) {
    for(int x=-1;x<=WIDTH;++x) {
        for(int z=-1;z<=WIDTH;++z) {
            for(int y=-1;y<=HEIGHT;++y) {
                bool validX = (x > 0 && x < WIDTH),
                     validY = (y > 0 && y < HEIGHT),
                     validZ = (z > 0 && z < WIDTH);
                if(validX && validY && validZ &&
                   blocks[x][z][y].filled) {
                    continue;
                }
                float fx = x,fy = y,fz = z;
                if(validY && validZ) {
                    if(x > 0 && blocks[x-1][z][y].filled) {
                        auto color = blocks[x-1][z][y].color;
                        varr.push_back({{fx,fy,  fz},  color});
                        varr.push_back({{fx,fy+1,fz},  color});
                        varr.push_back({{fx,fy+1,fz+1},color});
                        varr.push_back({{fx,fy,  fz+1},color});
                    }
                    if(x < WIDTH-1 && blocks[x+1][z][y].filled) {
                        auto color = blocks[x+1][z][y].color;
                        varr.push_back({{fx+1,fy,  fz},  color});
                        varr.push_back({{fx+1,fy,  fz+1},color});
                        varr.push_back({{fx+1,fy+1,fz+1},color});
                        varr.push_back({{fx+1,fy+1,fz},  color});
                    }
                }
                if(validX && validY) {
                    if(z > 0 && blocks[x][z-1][y].filled) {
                        auto color = blocks[x][z-1][y].color;
                        varr.push_back({{fx,  fy,  fz},color});
                        varr.push_back({{fx+1,fy,  fz},color});
                        varr.push_back({{fx+1,fy+1,fz},color});
                        varr.push_back({{fx,  fy+1,fz},color});
                    }
                    if(z < WIDTH-1 && blocks[x][z+1][y].filled) {
                        auto color = blocks[x][z+1][y].color;
                        varr.push_back({{fx,  fy,  fz+1},color});
                        varr.push_back({{fx,  fy+1,fz+1},color});
                        varr.push_back({{fx+1,fy+1,fz+1},color});
                        varr.push_back({{fx+1,fy,  fz+1},color});
                    }
                }
                if(validX && validZ) {
                    if(y > 0 && blocks[x][z][y-1].filled) {
                        auto color = blocks[x][z][y-1].color;
                        varr.push_back({{fx,  fy,fz},  color});
                        varr.push_back({{fx,  fy,fz+1},color});
                        varr.push_back({{fx+1,fy,fz+1},color});
                        varr.push_back({{fx+1,fy,fz},  color});
                    }
                    if(y < HEIGHT-1 && blocks[x][z][y+1].filled) {
                        auto color = blocks[x][z][y+1].color;
                        varr.push_back({{fx,  fy+1,fz},  color});
                        varr.push_back({{fx+1,fy+1,fz},  color});
                        varr.push_back({{fx+1,fy+1,fz+1},color});
                        varr.push_back({{fx,  fy+1,fz+1},color});
                    }
                }
            }
        }
    }
}
