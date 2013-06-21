#include "Chunk.cpp"

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
                if(validY && validZ) {
                    if(x > 0 && blocks[x-1][z][y].filled) {
                        auto color = blocks[x-1][z][y].color;
                        varr.push_back({{x,y,  z},  color});
                        varr.push_back({{x,y+1,z},  color});
                        varr.push_back({{x,y+1,z+1},color});
                        varr.push_back({{x,y,  z+1},color});
                    }
                    if(x < WIDTH && blocks[x+1][z][y].filled) {
                        auto color = blocks[x+1][z][y].color;
                        varr.push_back({{x+1,y,  z},  color});
                        varr.push_back({{x+1,y,  z+1},color});
                        varr.push_back({{x+1,y+1,z+1},color});
                        varr.push_back({{x+1,y+1,z},  color});
                    }
                }
                if(validX && validY) {
                    if(z > 0 && blocks[x-1][z-1][y].filled) {
                        auto color = blocks[x][z-1][y].color;
                        varr.push_back({{x,  y,  z},color});
                        varr.push_back({{x+1,y,  z},color});
                        varr.push_back({{x+1,y+1,z},color});
                        varr.push_back({{x,  y+1,z},color});
                    }
                    if(z < WIDTH && blocks[x][z+1][y].filled) {
                        auto color = blocks[x][z+1][y].color;
                        varr.push_back({{x,  y,  z+1},color});
                        varr.push_back({{x,  y+1,z+1},color});
                        varr.push_back({{x+1,y+1,z+1},color});
                        varr.push_back({{x+1,y,  z+1},color});
                    }
                }
                if(validX && validZ) {
                    if(y > 0 && blocks[x][z][y-1].filled) {
                        auto color = blocks[x][z][y-1].color;
                        varr.push_back({{x,  y,z},  color});
                        varr.push_back({{x,  y,z+1},color});
                        varr.push_back({{x+1,y,z+1},color});
                        varr.push_back({{x+1,y,z},  color});
                    }
                    if(y < HEIGHT && blocks[x][z][y+1].filled) {
                        auto color = blocks[x][z][y+1].color;
                        varr.push_back({{x,  y+1,z},  color});
                        varr.push_back({{x+1,y+1,z},  color});
                        varr.push_back({{x+1,y+1,z+1},color});
                        varr.push_back({{x,  y+1,z+1},color});
                    }
                }
            }
        }
    }
}
