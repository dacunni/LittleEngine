#ifndef __RENDERABLE_H_
#define __RENDERABLE_H_

#include "OpenGLUtil.h"

class Matrix4x4;
class Program;

class Renderable
{
    public:
        Renderable();
        ~Renderable();

        virtual bool uploaded();
        virtual void bind();
        virtual void draw();

        void setShaderProgram( GLuint program );
        void useProgram();
        void setWorldMatrix( Matrix4x4 & mat );
        void setViewMatrix( Matrix4x4 & mat );
        void setProjection( Matrix4x4 & mat );
        void setAnimTime( float t );
        void setTexture( GLuint texId );
        void uploadTextureUniforms();

        const GLuint POSITION_ATTRIB_INDEX    = 0;
        const GLuint NORMAL_ATTRIB_INDEX      = 1;
        const GLuint TEX_COORD_ATTRIB_INDEX   = 2;

        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;
        unsigned long long num_vertices = 0;
        GLuint textureId = 0;
        bool hasTexture = false;

        GLuint shader_program;
};


#endif
