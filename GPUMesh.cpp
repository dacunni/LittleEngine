/*
 *  GPUMesh.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 5/12/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "GPUMesh.h"
#include "Matrix.h"
#include "ShaderProgram.h"
#include "TriangleMesh.h"

GPUMesh::GPUMesh() 
{

}

GPUMesh::~GPUMesh() 
{
    // IMPLEMENT ME - free gpu resources for VAO,etc
}

void GPUMesh::upload( TriangleMesh & mesh )
{
    printf("GPUMesh::upload\n");
    // Create vertex array object to hold everything
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Upload vertex positions
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    GLsizeiptr vsize = mesh.vertices.size() * sizeof(float) * 4;
    GLsizeiptr nsize = mesh.normals.size() * sizeof(float) * 4;
    GLsizeiptr tcsize = mesh.textureUVCoords.size() * sizeof(float) * 2;
    GLintptr vstart = 0;
    GLintptr nstart = vstart + vsize;
    GLintptr tcstart = nstart + nsize;
    printf("vsize = %ld vstart = %ld ", vsize, vstart); // TEMP
    printf("nsize = %ld nstart = %ld ", nsize, nstart); // TEMP
    printf("tcsize = %ld tcstart = %ld ", tcsize, tcstart); // TEMP
    printf("tris  = %ld\n", mesh.triangles.size()); // TEMP

    // allocate some space for all of our attributes
    glBufferData( GL_ARRAY_BUFFER, vsize + nsize + tcsize, NULL, GL_STATIC_DRAW );

    // upload positions
    glBufferSubData( GL_ARRAY_BUFFER, vstart, vsize, &mesh.vertices[0].x );
    GL_WARN_IF_ERROR();
    glVertexAttribPointer( POSITION_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, 0, (void *) vstart );
    glEnableVertexAttribArray( POSITION_ATTRIB_INDEX );

    // upload normals
    glBufferSubData( GL_ARRAY_BUFFER, nstart, nsize, &mesh.normals[0].x );
    glVertexAttribPointer( NORMAL_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, 0, (void *) nstart );
    glEnableVertexAttribArray( NORMAL_ATTRIB_INDEX );
    GL_WARN_IF_ERROR();

    // upload UV coordinates
    if( mesh.hasTextureUVCoordinates() ) {
        glBufferSubData( GL_ARRAY_BUFFER, tcstart, tcsize, &mesh.textureUVCoords[0].u );
        glVertexAttribPointer( TEX_COORD_ATTRIB_INDEX, 2, GL_FLOAT, GL_FALSE, 0, (void *) tcstart );
        glEnableVertexAttribArray( TEX_COORD_ATTRIB_INDEX );
        GL_WARN_IF_ERROR();
    }

    // Upload vertex indices
    glGenBuffers( 1, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh.triangles.size() * sizeof(unsigned int) * 3, &mesh.triangles[0], GL_STATIC_DRAW );
    GL_WARN_IF_ERROR();

    num_vertices = mesh.triangles.size() * 3;
}

void GPUMesh::bind()
{
    if( vao ) {
        glBindVertexArray( vao );
        GL_WARN_IF_ERROR();
    }
}

void GPUMesh::draw()
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


