
#include "GPURenderable.h"
#include "Matrix.h"
#include "ShaderProgram.h"

GPURenderable::GPURenderable() 
{

}

GPURenderable::~GPURenderable() 
{
    // IMPLEMENT ME - free gpu resources for VAO,etc
}

bool GPURenderable::uploaded()
{
    return vao != 0;
}

void GPURenderable::bind()
{
    if( vao ) {
        glBindVertexArray( vao );
        GL_WARN_IF_ERROR();
    }
}

void GPURenderable::draw()
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

void GPURenderable::setShaderProgram( GLuint program )
{
    glUseProgram( program );
    shader_program = program;
}

void GPURenderable::useProgram()
{
    glUseProgram( shader_program );
}

void GPURenderable::setWorldMatrix( Matrix4x4 & mat )
{
    GLint loc = glGetUniformLocation( shader_program, "world" );
    GL_WARN_IF_ERROR();
    glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
    GL_WARN_IF_ERROR();
}

void GPURenderable::setViewMatrix( Matrix4x4 & mat )
{
    GLint loc = glGetUniformLocation( shader_program, "view" );
    GL_WARN_IF_ERROR();
    glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
    GL_WARN_IF_ERROR();
}

void GPURenderable::setProjection( Matrix4x4 & mat )
{
    GLint loc = glGetUniformLocation( shader_program, "projection" );
    GL_WARN_IF_ERROR();
    glUniformMatrix4fv( loc, 1, GL_TRUE, mat.data );
    GL_WARN_IF_ERROR();
}

void GPURenderable::setAnimTime( float t )
{
    GLint loc = glGetUniformLocation( shader_program, "anim_time" );
    GL_WARN_IF_ERROR();
    glUniform1f( loc, t );
    GL_WARN_IF_ERROR();
}


