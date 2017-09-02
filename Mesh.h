#ifndef __MESH_H_
#define __MESH_H_

#include "OpenGLUtil.h"
#include "Renderable.h"

class TriangleMesh;

class Mesh : public Renderable
{
    public:
        Mesh();
        ~Mesh();

        void upload( TriangleMesh & mesh );
        virtual void bind();
        virtual void draw();
};






#endif
