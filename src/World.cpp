#include "World.hpp"
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <limits>

constexpr Color3f World::ROCKCOLOR,
                  World::DIRTCOLOR;

World::World() : perlin(WIDTH*Chunk::WIDTH/PERLINSCALE+0.5,
                        WIDTH*Chunk::WIDTH/PERLINSCALE+0.5) {
    for(int x=0;x<WIDTH;++x) {
        for(int z=0;z<WIDTH;++z) {
            _chunks.emplace_back(new Chunk);
            _generateChunk(x,z);
        }
    }
    for(auto& chunk:_loadedChunks) {
        chunk.used = false;
    }
}

Vec2i World::_correctCoords(int x,int z) {
    int width = WIDTH*Chunk::WIDTH;
    return Vec2i{{((x%width+width)%width)/Chunk::WIDTH,
                  ((z%width+width)%width)/Chunk::WIDTH}};
}

Chunk& World::_getChunk(int x,int z) {
    return *_chunks[x*WIDTH+z];
}
const Chunk& World::_getChunk(int x,int z) const {
    return *_chunks[x*WIDTH+z];
}

void World::deleteBlock(int x,int y,int z) {
    Vec2i chunkLoc = _correctCoords(x,z);
    x = (x%Chunk::WIDTH+Chunk::WIDTH)%Chunk::WIDTH;
    z = (z%Chunk::WIDTH+Chunk::WIDTH)%Chunk::WIDTH;
    if(y >= 0 && y < Chunk::HEIGHT) {
        auto& b = _getChunk(chunkLoc[0],chunkLoc[1]).blocks[x][z][y];
        b.filled = false;
        _chunksToUpdate.insert(chunkLoc);
    }
}

void World::setBlock(int x,int y,int z,const Color3f& c) {
    Vec2i chunkLoc = _correctCoords(x,z);
    x = (x%Chunk::WIDTH+Chunk::WIDTH)%Chunk::WIDTH;
    z = (z%Chunk::WIDTH+Chunk::WIDTH)%Chunk::WIDTH;
    if(y >= 0 && y < Chunk::HEIGHT) {
        auto& b = _getChunk(chunkLoc[0],chunkLoc[1]).blocks[x][z][y];
        b.filled = true;
        b.color = c;
        _chunksToUpdate.insert(chunkLoc);
    }
}

Block World::getBlock(int x,int y,int z) const {
    Vec2i chunkLoc = _correctCoords(x,z);
    x = (x%Chunk::WIDTH+Chunk::WIDTH)%Chunk::WIDTH;
    z = (z%Chunk::WIDTH+Chunk::WIDTH)%Chunk::WIDTH;
    if(y < 0 || y >= Chunk::HEIGHT) {
        return Block();
    }
    return _getChunk(chunkLoc[0],chunkLoc[1]).blocks[x][z][y];
}

template<class T>
T signum(T x) {
    return x > 0 ?  1 :
           x < 0 ? -1 :
                    0;
}

// Traces viewDir out from _viewerLoc until either a filled block
// is reached or the distance has reached maxDist.
// Uses Bresenham's Line Algorithm generalized to 3D.
bool World::selectedBlock(Vec3i& out,Vec3f viewDir,float maxDist) {
    if(viewDir.magSquared() < std::numeric_limits<float>::epsilon()) {
        return false;
    }
    Vec3i step;
    for(int i=0;i<3;++i) {
        step[i] = signum(viewDir[i]);
    }
    int maxAxis;
    for(int i=0;i<3;++i) {
        if(i == 0 || std::abs(viewDir[i]) > std::abs(viewDir[maxAxis])) {
            maxAxis = i;
        }
    }
    int i0 = maxAxis,i1 = (maxAxis+1)%3,i2 = (maxAxis+2)%3;

    Vec2f delta = Vec2f{{viewDir[i1],viewDir[i2]}}/viewDir[i0];
    Vec2f err = {{0,0}};
    Vec3i blockLoc = _viewerLoc;

    while(((Vec3f)(blockLoc)-_viewerLoc).magSquared()<maxDist*maxDist) {
        if(getBlock(blockLoc[0],blockLoc[1],blockLoc[2]).filled) {
            //deleteBlock(blockLoc[0],blockLoc[1],blockLoc[2]);
            out = blockLoc;
            return true;
        }
        err += delta;
        
        if(err[0] >= 0.5) {
            blockLoc[i1] += step[i1];
            err[0] -= 1;
        }
        if(err[1] >= 0.5) {
            blockLoc[i2] += step[i2];
            err[1] -= 1;
        }
        blockLoc[i0] += step[i0];
    }
    return false;
}

void World::setViewerLoc(Vec3f loc) {
    _viewerLoc = loc;
    int chunkX = loc[0]/Chunk::WIDTH,
        chunkZ = loc[2]/Chunk::WIDTH;
    int startX = chunkX-LOADED_REGION_WIDTH/2,
        startZ = chunkZ-LOADED_REGION_WIDTH/2;
    for(int x = 0;x<LOADED_REGION_WIDTH;++x) {
        for(int z=0;z<LOADED_REGION_WIDTH;++z) {
            _chunksToLoad.insert({{startX+x,startZ+z}});
        }
    }
}

void World::draw(Vec3f viewDir) const {
    static constexpr Vec3f cornerOffsets[] = {
        {{0,0,0}},
        {{Chunk::WIDTH,0,0}},
        {{Chunk::WIDTH,0,Chunk::WIDTH}},
        {{0,0,Chunk::WIDTH}},
        {{0,Chunk::HEIGHT,0}},
        {{Chunk::WIDTH,Chunk::HEIGHT,0}},
        {{Chunk::WIDTH,Chunk::HEIGHT,Chunk::WIDTH}},
        {{0,Chunk::HEIGHT,Chunk::WIDTH}} };
    int drawCount = 0;
    for(auto& chunk:_loadedChunks) {
        if(!chunk.used) {
            continue;
        }
        bool inFront = false;
        for(auto offset:cornerOffsets) {
            if(dot(viewDir,chunk.loc+offset-_viewerLoc) > 0) {
                inFront = true;
                break;
            }
        }
        if(inFront) {
            drawCount++;
            glPushMatrix();
            glTranslatef(chunk.loc[0],chunk.loc[1],chunk.loc[2]);
            drawVertArray(GL_QUADS,chunk.varr);
            glPopMatrix();
        }
    }
}

void World::update(float dt) {
    _timeSinceLoad += dt;
    int numLoads = (int)(_timeSinceLoad*CHUNK_LOADS_PER_SECOND);
    for(int i=0;i<numLoads && !(_chunksToUpdate.empty() &&
                                _chunksToLoad.empty());++i) {

        Vec2i loc;
        bool update = !_chunksToUpdate.empty();
        if(update) {
            auto j = _chunksToUpdate.begin();
            loc = *j;
            _chunksToUpdate.erase(j);
        } else {
            auto j = _chunksToLoad.begin();
            loc = *j;
            _chunksToLoad.erase(j);
        }
        if(!_loadChunk(loc[0],loc[1],update)) {
            i--;
        }
    }
}

bool World::_loadChunk(int x,int z,bool changed) {
    x *= Chunk::WIDTH;
    z *= Chunk::WIDTH;
    Vec2i chunkLoc = _correctCoords(x,z);
    auto i = _chunkIndexByLoc.find(chunkLoc);
    if(i != _chunkIndexByLoc.end()) {
        auto& chunk = _loadedChunks[i->second];
        chunk.used = true;
        chunk.loc = Vec3f{{(float)x,0,(float)z}};
        if(changed) {
            chunk.varr.clear();
            _buildChunkVarr(chunkLoc[0],chunkLoc[1],chunk.varr);
            return true;
        }
        return false;
    }
    int viewerX = _viewerLoc[0]/Chunk::WIDTH,
        viewerZ = _viewerLoc[2]/Chunk::WIDTH;
    int chunkIndex = -1;
    float farthestDist = 0;
    for(int i=_loadedChunks.size()-1;i>=0;--i) {
        auto& chunk = _loadedChunks[i];
        Vec2i loc = _correctCoords(chunk.loc[0],
                                   chunk.loc[2]);
        if(loc == chunkLoc) {
            chunk.used = true;
            chunk.loc = Vec3f{{(float)x,0,(float)z}};
            if(changed) {
                chunk.varr.clear();
                _buildChunkVarr(chunkLoc[0],chunkLoc[1],chunk.varr);
            }
            return true;
        }
        int loadStartX = viewerX-LOADED_REGION_WIDTH/2,
            loadStartZ = viewerZ-LOADED_REGION_WIDTH/2;
        int loadEndX = loadStartX+LOADED_REGION_WIDTH,
            loadEndZ = loadStartZ+LOADED_REGION_WIDTH;
        if(!chunk.used ||
           (int)(chunk.loc[0]/Chunk::WIDTH) < loadStartX ||
           (int)(chunk.loc[0]/Chunk::WIDTH) > loadEndX ||
           (int)(chunk.loc[2]/Chunk::WIDTH) < loadStartZ ||
           (int)(chunk.loc[2]/Chunk::WIDTH) > loadEndZ) {
            float dist = (Vec2f{{_viewerLoc[0],_viewerLoc[2]}}
                          -Vec2f{{chunk.loc[0],chunk.loc[2]}}).mag();
            if(chunkIndex < 0 || dist > farthestDist) {
                farthestDist = dist;
                chunkIndex = i;
            }
        }
    }
    if(chunkIndex < 0) {
        chunkIndex = _loadedChunks.size();
        _loadedChunks.resize(chunkIndex+1);
    }
    auto& chunkSlot = _loadedChunks[chunkIndex];
    if(chunkSlot.used) {
        std::vector<Vec2i> removedChunks;
        for(auto i=_chunkIndexByLoc.begin();
             i != _chunkIndexByLoc.end();++i) {
            if(i->second == chunkIndex) {
                removedChunks.push_back(i->first);
                _loadedChunks[i->second].used = false;
            }
        }
        for(auto x: removedChunks) {
            _chunkIndexByLoc.erase(x);
        }
        auto i = _chunkIndexByLoc.find(chunkLoc);
        if(i != _chunkIndexByLoc.end()) {
            _loadedChunks[i->second].used = false;
        }
    }
    chunkSlot.used = true;
    chunkSlot.varr.clear();
    _buildChunkVarr(chunkLoc[0],chunkLoc[1],chunkSlot.varr);
    chunkSlot.loc = Vec3f{{(float)x,0,(float)z}};
    _chunkIndexByLoc[chunkLoc] = chunkIndex;
    return true;
}

void World::_buildChunkVarr(int chunkX,int chunkZ,VertexArray& varr) const {
    auto& c = _getChunk(chunkX,chunkZ);
    //std::cout << "Building chunk varr" << std::endl;
    //int vertCount = varr.size();
    for(int x=-1;x<=Chunk::WIDTH;++x) {
        for(int z=-1;z<=Chunk::WIDTH;++z) {
            for(int y=-1;y<=Chunk::HEIGHT;++y) {
                bool validX = (x >= 0 && x < Chunk::WIDTH),
                     validY = (y >= 0 && y < Chunk::HEIGHT),
                     validZ = (z >= 0 && z < Chunk::WIDTH);
                //if((validX && validY && validZ && c.blocks[x][z][y].filled) ||
                //   (validY && ((!validX && validZ) || (validX && !validZ)) &&
                if(validY && getBlock(chunkX*Chunk::WIDTH+x,
                                      y,
                                      chunkZ*Chunk::WIDTH+z).filled) {
                    if((x < 0 || z < 0) && y >= SEALEVEL) {
                        //std::cout << Vec3i{{x,y,z}} << std::endl;
                    }
                    continue;
                }
                float fx = x,fy = y,fz = z;
                if(validY && validZ) {
                    auto normal = Vec3f{{1,0,0}};
                    if(x > 0 && c.blocks[x-1][z][y].filled) {
                        auto color = c.blocks[x-1][z][y].color;
                        varr.push_back({{fx,fy,  fz},
                                        color,normal});
                        varr.push_back({{fx,fy+1,fz},
                                        color,normal});
                        varr.push_back({{fx,fy+1,fz+1},
                                        color,normal});
                        varr.push_back({{fx,fy,  fz+1},
                                        color,normal});
                    }
                    if(x < Chunk::WIDTH-1 && c.blocks[x+1][z][y].filled) {
                        auto color = c.blocks[x+1][z][y].color;
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
                    if(z > 0 && c.blocks[x][z-1][y].filled) {
                        auto color = c.blocks[x][z-1][y].color;
                        varr.push_back({{fx,  fy,  fz},color,normal});
                        varr.push_back({{fx+1,fy,  fz},color,normal});
                        varr.push_back({{fx+1,fy+1,fz},color,normal});
                        varr.push_back({{fx,  fy+1,fz},color,normal});
                    }
                    if(z < Chunk::WIDTH-1 && c.blocks[x][z+1][y].filled) {
                        auto color = c.blocks[x][z+1][y].color;
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
                    if(y > 0 && c.blocks[x][z][y-1].filled) {
                        auto color = c.blocks[x][z][y-1].color;
                        varr.push_back({{fx,  fy,fz},  color,normal});
                        varr.push_back({{fx,  fy,fz+1},color,normal});
                        varr.push_back({{fx+1,fy,fz+1},color,normal});
                        varr.push_back({{fx+1,fy,fz},  color,normal});
                    }
                    if(y < Chunk::HEIGHT-1 && c.blocks[x][z][y+1].filled) {
                        auto color = c.blocks[x][z][y+1].color;
                        varr.push_back({{fx,  fy+1,fz},  color,-normal});
                        varr.push_back({{fx+1,fy+1,fz},  color,-normal});
                        varr.push_back({{fx+1,fy+1,fz+1},color,-normal});
                        varr.push_back({{fx,  fy+1,fz+1},color,-normal});
                    }
                }
            }
        }
    }
    //std::cout << varr.size()-vertCount << " verts" << std::endl;
}

void World::_generateChunk(int chunkX,int chunkZ) {
    Chunk& c = _getChunk(chunkX,chunkZ);
    chunkX *= Chunk::WIDTH;
    chunkZ *= Chunk::WIDTH;
    for(int x=0;x<Chunk::WIDTH;++x) {
        for(int z=0;z<Chunk::WIDTH;++z) {
            int height = SEALEVEL
                         +16*(perlin.octaveNoise((chunkX+x)/PERLINSCALE,
                                                 (chunkZ+z)/PERLINSCALE,
                                                 2,2,3));
            for(int y=0;y<Chunk::HEIGHT;++y) {
                Block& block = c.blocks[x][z][y];
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

