/*
 *  GPURenderable.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 5/12/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

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

void GPURenderable::setModelViewMatrix( Matrix4x4 & mat )
{
    GLint mv_loc = glGetUniformLocation( shader_program, "model_view" );
    GL_WARN_IF_ERROR();
    glUniformMatrix4fv( mv_loc, 1, GL_TRUE, mat.data );
    GL_WARN_IF_ERROR();
}

void GPURenderable::setProjection( Matrix4x4 & mat )
{
    GLint proj_loc = glGetUniformLocation( shader_program, "projection" );
    GL_WARN_IF_ERROR();
    glUniformMatrix4fv( proj_loc, 1, GL_TRUE, mat.data );
    GL_WARN_IF_ERROR();
}


