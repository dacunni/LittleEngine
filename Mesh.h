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

        void upload();
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
void makeMeshTetrahedron( Mesh & mesh );
void makeMeshCube( Mesh & mesh );
void makeMeshGroundPlatform( Mesh & mesh, float size );




#endif
