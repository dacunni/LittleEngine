
#include <vector>
#include "Renderable.h"
#include "Matrix.h"
#include "ShaderProgram.h"

Renderable::Renderable() 
{
    vao.init();
    vbo.init();
}

Renderable::~Renderable() 
{
}

bool Renderable::uploaded()
{
    return is_uploaded;
}

void Renderable::upload()
{
}

void Renderable::bind()
{
    vao.bind();
}

void Renderable::draw()
{
    if( vao ) {
        if( ibo ) {
            glDrawElements( GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, NULL );
        }
        else {
            // If we don't have an index buffer object, just assume we can use the vertices in order
            glDrawArrays( GL_TRIANGLES, 0, numVertices );
        }
        GL_WARN_IF_ERROR();
    }
}

void Renderable::setShaderProgram( const std::shared_ptr<Program> & program )
{
    shaderProgram = program;
    glUseProgram(shaderProgram->id);
}

void Renderable::useProgram()
{
    glUseProgram( shaderProgram->id );
}

void Renderable::setWorldMatrix( const Matrix4x4 & mat )
{
    GLint loc = shaderProgram->uniformLocation("world");
    if(loc > -1) {
        glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
        GL_WARN_IF_ERROR();
    }
}

void Renderable::setViewMatrix( const Matrix4x4 & mat )
{
    GLint loc = shaderProgram->uniformLocation("view");
    if(loc > -1) {
        glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
        GL_WARN_IF_ERROR();
    }
}

void Renderable::setProjection( const Matrix4x4 & mat )
{
    GLint loc = shaderProgram->uniformLocation("projection");
    if(loc > -1) {
        glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
        GL_WARN_IF_ERROR();
    }
}

void Renderable::setCameraPosition( const Vector4 & pos )
{
    GLint loc = shaderProgram->uniformLocation("cameraPosition");
    if(loc > -1) {
        glUniform4fv( loc, 1, pos.data );
        GL_WARN_IF_ERROR();
    }
}

void Renderable::setAnimTime( float t )
{
    GLint loc = shaderProgram->uniformLocation("anim_time");
    if(loc > -1) {
        glUniform1f( loc, t );
        GL_WARN_IF_ERROR();
    }
}

void Renderable::setHighlighted( bool highlighted )
{
    GLint loc = shaderProgram->uniformLocation("highlighted");
    if(loc > -1) {
        glUniform1i( loc, (int) highlighted );
        GL_WARN_IF_ERROR();
    }
}

void Renderable::setLights( float * pos, float * intensity, int numLights )
{
    GLint loc;

    loc = shaderProgram->uniformLocation("numLights");
    if(loc > -1) {
        glUniform1i( loc, numLights );
        GL_WARN_IF_ERROR();
    }

    loc = shaderProgram->uniformLocation("lightPositions");
    if(loc > -1) {
        glUniform3fv( loc, numLights, pos );
        GL_WARN_IF_ERROR();
    }

    loc = shaderProgram->uniformLocation("lightIntensities");
    if(loc > -1) {
        glUniform3fv( loc, numLights, intensity );
        GL_WARN_IF_ERROR();
    }
}

void Renderable::setTexture( GLuint texId )
{
    textureId = texId;
    hasTexture = true;
}

void Renderable::setRoughness( float roughness )
{
    this->roughness = std::max(roughness, 0.05f);
}

void Renderable::setF0( float F0 )
{
    this->F0 = F0;
}

void Renderable::uploadMaterialUniforms()
{
    GLint loc = shaderProgram->uniformLocation("roughness");
    if(loc > -1) {
        glUniform1f( loc, roughness );
        GL_WARN_IF_ERROR();
    }
    loc = shaderProgram->uniformLocation("F0");
    if(loc > -1) {
        glUniform1f( loc, F0 );
        GL_WARN_IF_ERROR();
    }
}

void Renderable::uploadTextureUniforms()
{
    GLint loc;
    loc = shaderProgram->uniformLocation("useTexture");
    if( loc > -1 ) {
        glUniform1i( loc, (int) hasTexture );
        GL_WARN_IF_ERROR();
    }
    loc = shaderProgram->uniformLocation("tex");
    GLint textureUnit = 0;
    if( loc > -1 ) {
        glUniform1i( loc, textureUnit );
        GL_WARN_IF_ERROR();
    }
}

