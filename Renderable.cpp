
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
    GLint loc = glGetUniformLocation( shaderProgram->id, "world" );
    glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
}

void Renderable::setViewMatrix( const Matrix4x4 & mat )
{
    GLint loc = glGetUniformLocation( shaderProgram->id, "view" );
    glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
}

void Renderable::setProjection( const Matrix4x4 & mat )
{
    GLint loc = glGetUniformLocation( shaderProgram->id, "projection" );
    glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
}

void Renderable::setCameraPosition( const Vector4 & pos )
{
    GLint loc = glGetUniformLocation( shaderProgram->id, "cameraPosition" );
    glUniform4fv( loc, 1, pos.data );
}

void Renderable::setAnimTime( float t )
{
    GLint loc = glGetUniformLocation( shaderProgram->id, "anim_time" );
    glUniform1f( loc, t );
}

void Renderable::setLights( float pos[][3], float intensity[][3], int numLights )
{
    GLint loc;

    loc = glGetUniformLocation( shaderProgram->id, "numLights" );
    glUniform1i( loc, numLights );

    loc = glGetUniformLocation( shaderProgram->id, "lightPositions" );
    glUniform3fv( loc, numLights, (float*)pos );

    loc = glGetUniformLocation( shaderProgram->id, "lightIntensities" );
    glUniform3fv( loc, numLights, (float*)intensity );
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
    GLint loc = glGetUniformLocation( shaderProgram->id, "roughness" );
    glUniform1f( loc, roughness );
    loc = glGetUniformLocation( shaderProgram->id, "F0" );
    glUniform1f( loc, F0 );
}

void Renderable::uploadTextureUniforms()
{
    GLint loc;
    loc = glGetUniformLocation( shaderProgram->id, "useTexture" );
    GL_WARN_IF_ERROR();
    if( loc ) {
        glUniform1i( loc, (int) hasTexture );
        GL_WARN_IF_ERROR();
    }
    loc = glGetUniformLocation( shaderProgram->id, "tex" );
    GLint textureUnit = 0;
    GL_WARN_IF_ERROR();
    if( loc ) {
        glUniform1i( loc, 0 );
        GL_WARN_IF_ERROR();
    }
}

