#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include "Mesh.h"
#include "Transform.h"

class TriangleMesh;

class GameObject
{
    public:
        GameObject( const std::string & path );
        GameObject( TriangleMesh * trimesh );
        ~GameObject();

        void draw();

        TriangleMesh * triangleMesh;
        Mesh mesh;
        Transform worldTransform;
};



#endif
