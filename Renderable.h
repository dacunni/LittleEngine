#ifndef __RENDERABLE_H_
#define __RENDERABLE_H_

#include "OpenGLUtil.h"
#include "ShaderProgram.h"

class Matrix4x4;
class Program;
class Vector4;

struct UVCoord { float u, v; };
struct RGBColor { float r, g, b; };

class Renderable
{
    public:
        Renderable();
        ~Renderable();

        bool uploaded();
        virtual void upload();
        virtual void bind();
        virtual void draw();

        void setShaderProgram( const std::shared_ptr<Program> & program );
        void useProgram();
        void setWorldMatrix( const Matrix4x4 & mat );
        void setViewMatrix( const Matrix4x4 & mat );
        void setProjection( const Matrix4x4 & mat );
        void setCameraPosition( const Vector4 & pos );
        void setAnimTime( float t );
        void setHighlighted( bool highlighted );
        void setLights( float * pos, float * intensity, int numLights );
        void setTexture( GLuint texId );
        void setRoughness( float roughness );
        void setF0( float F0 );
        void setDiffuseColor( const RGBColor & c );
        void uploadMaterialUniforms();
        void uploadTextureUniforms();

        const GLuint POSITION_ATTRIB_INDEX    = 0;
        const GLuint NORMAL_ATTRIB_INDEX      = 1;
        const GLuint TEX_COORD_ATTRIB_INDEX   = 2;

        bool is_uploaded = false;
        VertexArrayObject vao;
        BufferObject vbo;
        BufferObject ibo;
        unsigned long long numVertices = 0;
        GLuint textureId = 0;
        bool hasTexture = false;

        std::shared_ptr<Program> shaderProgram;

        // Material properties
        float roughness = 0.1;
        float F0 = 0.25;
        RGBColor diffuseColor;

        bool visible = true;
};


#endif
