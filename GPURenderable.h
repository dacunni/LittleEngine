/*
 *  GPURenderable.h
 *  FastRender
 *
 *  Created by David Cunningham on 4/11/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _GPU_RENDERABLE_H_
#define _GPU_RENDERABLE_H_

#include "OpenGLUtil.h"

class Matrix4x4;
class Program;

class GPURenderable
{
    public:
        GPURenderable();
        ~GPURenderable();

        virtual bool uploaded();
        virtual void bind();
        virtual void draw();

        void setShaderProgram( GLuint program );
        void setModelViewMatrix( Matrix4x4 & mat );
        void setProjection( Matrix4x4 & mat );

        const GLuint POSITION_ATTRIB_INDEX = 0;
        const GLuint NORMAL_ATTRIB_INDEX   = 1;

        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;
        unsigned long long num_vertices = 0;

        GLuint shader_program;
};


#endif