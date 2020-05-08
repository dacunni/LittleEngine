#ifndef __MESH_H_
#define __MESH_H_

#include <cstdint>
#include <vector>
#include <map>

#include "OpenGLUtil.h"
#include "Renderable.h"
#include "Vector4.h"

struct UVCoord { float u, v; };

struct MeshData
{
    std::vector<Vector4>  vertices;
    std::vector<Vector4>  normals;
    std::vector<UVCoord>  textureUVCoords;
    std::vector<uint32_t> indices;
};

using MeshDataGroup = std::vector<std::shared_ptr<MeshData>>;

class MeshDataCache
{
    public:
        MeshDataCache() = default;
        MeshDataCache(const MeshDataCache &) = delete;
        ~MeshDataCache() = default;

        using Key = std::string;

        void add(const Key & key, const MeshDataGroup & meshDataGroup) {
            std::shared_ptr<MeshDataGroup> sp = std::make_shared<MeshDataGroup>();
            *sp = meshDataGroup;
            meshData[key] = sp;
        }

        bool contains(const Key & key) {
            return meshData.find(key) != meshData.end();
        }

        MeshDataGroup & get(const Key & key) {
            return *meshData[key];
        }

        int size() const { return (int) meshData.size(); }

    protected:
        std::map<Key, std::shared_ptr<MeshDataGroup>> meshData;
};

class Mesh : public Renderable
{
    public:
        Mesh();
        ~Mesh();

        virtual void upload();
        virtual void bind();
        virtual void draw();

        // Local storage
        std::vector< Vector4 >  vertices;
        std::vector< Vector4 >  normals;
        std::vector< UVCoord >  textureUVCoords;
        std::vector< uint32_t > indices;
};


// Simple Shapes
Mesh * makeMeshTetrahedron();
Mesh * makeMeshCube();
Mesh * makeMeshGroundPlatform( float size );




#endif
