#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include "GPUMesh.h"
#include "Transform.h"

class TriangleMesh;

class GameObject
{
    public:
        GameObject( const std::string & path );
        GameObject( TriangleMesh * trimesh );
        ~GameObject();

        void draw();

        TriangleMesh * mesh;
        GPUMesh gpu_mesh;
        Transform worldTransform;
};



#endif
