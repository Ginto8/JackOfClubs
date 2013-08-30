#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "Vertex.hpp"
#include "Perlin.hpp"
#include <queue>
#include <unordered_map>
#include <memory>

class World {
public:
    static constexpr int WIDTH = 512/Chunk::WIDTH,
                         LOADED_REGION_WIDTH = 8;
    static constexpr float CHUNK_LOADS_PER_SECOND = 8;
    static constexpr float PERLINSCALE = 8;
    static constexpr int SEALEVEL   = 32,
                         DIRTLAYERS =  5;
    static constexpr Color3f ROCKCOLOR = {  0.7,  0.7, 0.7 },
                             DIRTCOLOR = { 0.59, 0.29,   0 };

    World();

    void deleteBlock(int x,int y,int z);
    void setBlock(int x,int y,int z,const Color3f& c);
    const Block& getBlock(int x,int y,int z) const;

    void setViewerLoc(Vec3f loc);
    void draw(Vec3f viewDir) const;
    void update(float dt);

private:
    static Vec2i _correctCoords(int x,int z);

    bool _loadChunk(int x,int z);
    Chunk& _getChunk(int x,int z);
    const Chunk& _getChunk(int x,int z) const;

    void _buildChunkVarr(int x,int z,VertexArray& varr) const;
    void _generateChunk(int x,int z);

    struct _LoadedChunkSlot {
        VertexArray varr;
        bool used = false;
        Vec3f loc;
    };

    std::vector<_LoadedChunkSlot> _loadedChunks;
    std::unordered_map<Vec2i,int> _chunkIndexByLoc;
    std::queue<Vec2i> _chunksToLoad;
    Vec3f _viewerLoc;
    float _timeSinceLoad = 0;
    std::vector<std::shared_ptr<Chunk>> _chunks;
    Perlin perlin;
};

#endif
