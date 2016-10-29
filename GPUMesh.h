/*
 *  GPUMesh.h
 *  FastRender
 *
 *  Created by David Cunningham on 4/11/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
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
