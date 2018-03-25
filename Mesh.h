#ifndef __MESH_H_
#define __MESH_H_

#include <cstdint>
#include <vector>

#include "OpenGLUtil.h"
#include "Renderable.h"
#include "Vector4.h"

class Mesh : public Renderable
{
    public:
        Mesh();
        ~Mesh();

        virtual void upload();
        virtual void bind();
        virtual void draw();

        struct UVCoord { float u, v; };

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
