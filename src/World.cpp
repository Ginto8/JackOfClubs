#include "World.hpp"
#include <stdexcept>
#include <iostream>
#include <cmath>

World::World() {
    for(int i=0;i<WIDTH*WIDTH;++i) {
        _chunks.emplace_back(new Chunk);
    }
    for(auto chunk:_chunks) {
        Chunk::generate(*chunk);
    }
    for(auto& chunk:_loadedChunks) {
        chunk.used = false;
    }
}

Vec2i World::_correctCoords(int x,int z) {
    int chunkX = x/Chunk::WIDTH,chunkZ = z/Chunk::WIDTH;
    x -= chunkX*Chunk::WIDTH;
    z -= chunkZ*Chunk::WIDTH;
    chunkX %= WIDTH;
    chunkZ %= WIDTH;
    if(chunkX < 0) {
        chunkX += WIDTH;
    }
    if(chunkZ < 0) {
        chunkZ += WIDTH;
    }
    return {{chunkX,chunkZ}};
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
    }
}

const Block& World::getBlock(int x,int y,int z) const {
    Vec2i chunkLoc = _correctCoords(x,z);
    x = (x%Chunk::WIDTH+Chunk::WIDTH)%Chunk::WIDTH;
    z = (z%Chunk::WIDTH+Chunk::WIDTH)%Chunk::WIDTH;
    if(y < 0 || y >= Chunk::HEIGHT) {
        throw std::out_of_range("Invalid block y coordinate");
    }
    return _getChunk(chunkLoc[0],chunkLoc[1]).blocks[x][z][y];
}

void World::setViewerLoc(Vec3f loc) {
    _viewerLoc = loc;
    int chunkX = loc[0]/Chunk::WIDTH,
        chunkZ = loc[2]/Chunk::WIDTH;
    int startX = chunkX-LOADED_REGION_WIDTH/2,
        startZ = chunkZ-LOADED_REGION_WIDTH/2;
    for(int x = 0;x<LOADED_REGION_WIDTH;++x) {
        for(int z=0;z<LOADED_REGION_WIDTH;++z) {
            _chunksToLoad.push({{startX+x,startZ+z}});
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
    for(int i=0;i<numLoads && !_chunksToLoad.empty();++i) {
        auto loc = _chunksToLoad.front();
        _chunksToLoad.pop();
        if(!_loadChunk(loc[0],loc[1])) {
            i--;
        }
    }
}

bool World::_loadChunk(int x,int z) {
    x *= Chunk::WIDTH;
    z *= Chunk::WIDTH;
    Vec2i chunkLoc = _correctCoords(x,z);
    auto i = _chunkIndexByLoc.find(chunkLoc);
    if(i != _chunkIndexByLoc.end()) {
        auto& chunk = _loadedChunks[i->second];
        chunk.used = true;
        chunk.loc = Vec3f{{(float)x,0,(float)z}};
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
            return false;
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
    _getChunk(chunkLoc[0],chunkLoc[1]).buildVertArray(chunkSlot.varr);
    chunkSlot.loc = Vec3f{{(float)x,0,(float)z}};
    _chunkIndexByLoc[chunkLoc] = chunkIndex;
    return true;
}

