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
        void useProgram();
        void setWorldMatrix( Matrix4x4 & mat );
        void setViewMatrix( Matrix4x4 & mat );
        void setProjection( Matrix4x4 & mat );
        void setAnimTime( float t );

        const GLuint POSITION_ATTRIB_INDEX    = 0;
        const GLuint NORMAL_ATTRIB_INDEX      = 1;
        const GLuint TEX_COORD_ATTRIB_INDEX   = 2;

        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;
        unsigned long long num_vertices = 0;

        GLuint shader_program;
};


#endif
