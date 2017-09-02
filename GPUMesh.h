#ifndef _GPU_MESH_H_
#define _GPU_MESH_H_

#include "OpenGLUtil.h"
#include "GPURenderable.h"

class TriangleMesh;

class GPUMesh : public GPURenderable
{
    public:
        GPUMesh();
        ~GPUMesh();

        void upload( TriangleMesh & mesh );
        virtual void bind();
        virtual void draw();
};






#endif
