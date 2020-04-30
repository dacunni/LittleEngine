
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
            glDrawElements( GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, NULL );
        }
        else {
            // If we don't have an index buffer object, just assume we can use the vertices in order
            glDrawArrays( GL_TRIANGLES, 0, num_vertices );
        }
        GL_WARN_IF_ERROR();
    }
}

void Renderable::setShaderProgram( GLuint program )
{
    glUseProgram( program );
    shader_program = program;
}

void Renderable::setShaderProgram( const std::shared_ptr<Program> & program )
{
    shaderProgram = program;
    shader_program = program->id;
    glUseProgram( shader_program );
}

void Renderable::useProgram()
{
    glUseProgram( shader_program );
}

void Renderable::setWorldMatrix( const Matrix4x4 & mat )
{
    GLint loc = glGetUniformLocation( shader_program, "world" );
    glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
}

void Renderable::setViewMatrix( const Matrix4x4 & mat )
{
    GLint loc = glGetUniformLocation( shader_program, "view" );
    glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
}

void Renderable::setProjection( const Matrix4x4 & mat )
{
    GLint loc = glGetUniformLocation( shader_program, "projection" );
    glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
}

void Renderable::setCameraPosition( const Vector4 & pos )
{
    GLint loc = glGetUniformLocation( shader_program, "cameraPosition" );
    glUniform4fv( loc, 1, pos.data );
}

void Renderable::setAnimTime( float t )
{
    GLint loc = glGetUniformLocation( shader_program, "anim_time" );
    glUniform1f( loc, t );
}

void Renderable::setLights( float pos[][3], float intensity[][3], int numLights )
{
    GLint loc;

    loc = glGetUniformLocation( shader_program, "numLights" );
    glUniform1i( loc, numLights );

    loc = glGetUniformLocation( shader_program, "lightPositions" );
    glUniform3fv( loc, numLights, (float*)pos );

    loc = glGetUniformLocation( shader_program, "lightIntensities" );
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

void Renderable::uploadMaterialUniforms()
{
    GLint loc = glGetUniformLocation( shader_program, "roughness" );
    glUniform1f( loc, roughness );
}

void Renderable::uploadTextureUniforms()
{
    GLint loc;
    loc = glGetUniformLocation( shader_program, "useTexture" );
    GL_WARN_IF_ERROR();
    if( loc ) {
        glUniform1i( loc, (int) hasTexture );
        GL_WARN_IF_ERROR();
    }
    loc = glGetUniformLocation( shader_program, "tex" );
    GLint textureUnit = 0;
    GL_WARN_IF_ERROR();
    if( loc ) {
        glUniform1i( loc, 0 );
        GL_WARN_IF_ERROR();
    }
}

