#include "Chunk.hpp"
#include <ctime>
#include <cstdlib>
#include <vector>
#include <cmath>
using namespace std;

constexpr Color3f Chunk::ROCKCOLOR,
                  Chunk::DIRTCOLOR;

// an implementation of what I interpret to be Perlin noise.
void gradientNoise(vector<float>& v,int w,int h) {
    static constexpr Vec2f gradients[8] = {
                           { 1,0},
                           {M_SQRT2,M_SQRT2},
                           {0, 1},
                           {-M_SQRT2, M_SQRT2},
                           {-1,0},
                           {-M_SQRT2,-M_SQRT2},
                           {0,-1},
                           { M_SQRT2,-M_SQRT2} };
    int n = w*h;
    v.clear();
    v.reserve(n);
    int gw = w+1,gh = h+1;
    int gn = gw*gh;
    vector<Vec2f> grid;
    grid.reserve(gn);
    for(int i=0;i<gn;++i) {
        grid.push_back(gradients[rand()%8]);
    }
    for(int y=0;y<h;++y) {
        for(int x=0;x<w;++x) {
            float u = 0.5;
            float n00 = dot(grid[y*gw+x],      Vec2f{{u,    u}}),
                  n10 = dot(grid[y*gw+x+1],    Vec2f{{u-1,  u}}),
                  n01 = dot(grid[(y+1)*gw+x],  Vec2f{{u,  u-1}}),
                  n11 = dot(grid[(y+1)*gw+x+1],Vec2f{{u-1,u-1}});
            // f(t) = 6*t^5-15t^4+10t^3
            float fu = 6*u*u*u*u*u-15*u*u*u*u+10*u*u*u;
            float nx0 = n00*(1-fu)+n10*fu,
                  nx1 = n01*(1-fu)+n11*fu;
            v.push_back(nx0*(1-fu)+nx1*fu);
        }
    }
}

void smooth(vector<float>& v,int w,int h) {
    vector<float> out = v;
    for(int y=0;y<h;++y) {
        for(int x=0;x<w;++x) {
            float corners = 0,sides = 0,
                  center  =  v[y*w+x]/4;
            if(x > 0 && x < w-1 &&
               y > 0 && y < h-1) {
                corners = (v[(y-1)*w+x-1]
                           +v[(y-1)*w+x+1]
                           +v[(y+1)*w+x-1]
                           +v[(y+1)*w+x+1])
                          /16,
                sides   = (v[y*w+x-1]
                           +v[y*w+x+1]
                           +v[(y+1)*w+x]
                           +v[(y-1)*w+x])
                          /8;
            } else {
                if(x > 0) {
                    sides += v[y*w+x-1];
                    if(y > 0) {
                        corners += v[(y-1)*w+x-1];
                    }
                    if(y < h-1) {
                        corners += v[(y+1)*w+x-1];
                    }
                }
                if(x < w-1) {
                    sides += v[y*w+x+1];
                    if(y > 0) {
                        corners += v[(y-1)*w+x+1];
                    }
                    if(y < h-1) {
                        corners += v[(y+1)*w+x+1];
                    }
                }
                if(y > 0) {
                    sides += v[(y-1)*w+x];
                }
                if(y < h-1) {
                    sides += v[(y+1)*w+x];
                }
                sides /= 8;
                corners /= 16;
            }
            out[y*w+x] = center + sides + corners;
        }
    }
    v = out;
}

void Chunk::generate(Chunk& c) {
    int noiseRes = 4;
    vector<float> noise;
    gradientNoise(noise,WIDTH*noiseRes,WIDTH*noiseRes);
    smooth(noise,WIDTH*noiseRes,WIDTH*noiseRes);
    for(int x=0;x<WIDTH;++x) {
        for(int z=0;z<WIDTH;++z) {
            int height = SEALEVEL + 4*(noise[x*noiseRes*WIDTH+noiseRes*z]-0.5);
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
                bool validX = (x >= 0 && x < WIDTH),
                     validY = (y >= 0 && y < HEIGHT),
                     validZ = (z >= 0 && z < WIDTH);
                if(validX && validY && validZ &&
                   blocks[x][z][y].filled) {
                    continue;
                }
                float fx = x,fy = y,fz = z;
                if(validY && validZ) {
                    auto normal = Vec3f{{1,0,0}};
                    if(x > 0 && blocks[x-1][z][y].filled) {
                        auto color = blocks[x-1][z][y].color;
                        varr.push_back({{fx,fy,  fz},
                                        color,normal});
                        varr.push_back({{fx,fy+1,fz},
                                        color,normal});
                        varr.push_back({{fx,fy+1,fz+1},
                                        color,normal});
                        varr.push_back({{fx,fy,  fz+1},
                                        color,normal});
                    }
                    if(x < WIDTH-1 && blocks[x+1][z][y].filled) {
                        auto color = blocks[x+1][z][y].color;
                        varr.push_back({{fx+1,fy,  fz},
                                        color,-normal});
                        varr.push_back({{fx+1,fy,  fz+1},
                                        color,-normal});
                        varr.push_back({{fx+1,fy+1,fz+1},
                                        color,-normal});
                        varr.push_back({{fx+1,fy+1,fz},
                                        color,-normal});
                    }
                }
                if(validX && validY) {
                    auto normal = Vec3f{{0,0,1}};
                    if(z > 0 && blocks[x][z-1][y].filled) {
                        auto color = blocks[x][z-1][y].color;
                        varr.push_back({{fx,  fy,  fz},color,normal});
                        varr.push_back({{fx+1,fy,  fz},color,normal});
                        varr.push_back({{fx+1,fy+1,fz},color,normal});
                        varr.push_back({{fx,  fy+1,fz},color,normal});
                    }
                    if(z < WIDTH-1 && blocks[x][z+1][y].filled) {
                        auto color = blocks[x][z+1][y].color;
                        varr.push_back({{fx,  fy,  fz+1},
                                        color,-normal});
                        varr.push_back({{fx,  fy+1,fz+1},
                                        color,-normal});
                        varr.push_back({{fx+1,fy+1,fz+1},
                                        color,-normal});
                        varr.push_back({{fx+1,fy,  fz+1},
                                        color,-normal});
                    }
                }
                if(validX && validZ) {
                    auto normal = Vec3f{{0,1,0}};
                    if(y > 0 && blocks[x][z][y-1].filled) {
                        auto color = blocks[x][z][y-1].color;
                        varr.push_back({{fx,  fy,fz},  color,normal});
                        varr.push_back({{fx,  fy,fz+1},color,normal});
                        varr.push_back({{fx+1,fy,fz+1},color,normal});
                        varr.push_back({{fx+1,fy,fz},  color,normal});
                    }
                    if(y < HEIGHT-1 && blocks[x][z][y+1].filled) {
                        auto color = blocks[x][z][y+1].color;
                        varr.push_back({{fx,  fy+1,fz},  color,-normal});
                        varr.push_back({{fx+1,fy+1,fz},  color,-normal});
                        varr.push_back({{fx+1,fy+1,fz+1},color,-normal});
                        varr.push_back({{fx,  fy+1,fz+1},color,-normal});
                    }
                }
            }
        }
    }
}
