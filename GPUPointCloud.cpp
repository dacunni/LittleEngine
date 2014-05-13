/*
 *  GPUPointCloud.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 5/12/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "GPUPointCloud.h"

GPUPointCloud::GPUPointCloud() 
{

}

GPUPointCloud::~GPUPointCloud() 
{
    // IMPLEMENT ME - free gpu resources for VAO,etc
}

void GPUPointCloud::upload( std::vector<Vector4> & points )
{
    printf("GPUPointCloud::upload\n");
    // Create vertex array object to hold everything
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Upload vertex positions
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    GLsizeiptr vsize = points.size() * sizeof(points[0]);
    // allocate some space for all of our attributes
    glBufferData( GL_ARRAY_BUFFER, vsize, NULL, GL_STATIC_DRAW );
    // upload positions
    glBufferSubData( GL_ARRAY_BUFFER, 0, vsize, &points[0] );
    GL_WARN_IF_ERROR();
    glVertexAttribPointer( POSITION_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( POSITION_ATTRIB_INDEX );

    num_vertices = points.size();
}

bool GPUPointCloud::uploaded()
{
    return vao != 0;
}

void GPUPointCloud::bind()
{
    if( vao ) {
        glBindVertexArray( vao );
        GL_WARN_IF_ERROR();
    }
}

void GPUPointCloud::draw()
{
    if( vao ) {
        glPointSize( point_size );
        glDrawArrays( GL_POINTS, 0, num_vertices );
        glPointSize( 1 );
        GL_WARN_IF_ERROR();
    }
}

void GPUPointCloud::setPointSize( float s ) 
{ 
    point_size = s; 
}


