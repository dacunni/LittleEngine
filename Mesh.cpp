#include "Mesh.h"
#include "Matrix.h"
#include "ShaderProgram.h"

Mesh::Mesh() 
{

}

Mesh::~Mesh() 
{
    // IMPLEMENT ME - free gpu resources for VAO,etc
}

void Mesh::upload()
{
    printf("Mesh::upload\n");
    // Create vertex array object to hold everything
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Upload vertex positions
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    GLsizeiptr vsize = vertices.size() * sizeof(float) * 4;
    GLsizeiptr nsize = normals.size() * sizeof(float) * 4;
    GLsizeiptr tcsize = textureUVCoords.size() * sizeof(float) * 2;
    GLsizeiptr isize = indices.size() * sizeof(uint32_t);
    GLintptr vstart = 0;
    GLintptr nstart = vstart + vsize;
    GLintptr tcstart = nstart + nsize;
    printf("vertices = %lu vsize = %ld vstart = %ld ", vertices.size(), vsize, vstart); // TEMP
    printf("normals  = %lu nsize = %ld nstart = %ld ", normals.size(), nsize, nstart); // TEMP
    printf("tcoords  = %lu tcsize = %ld tcstart = %ld ", textureUVCoords.size(), tcsize, tcstart); // TEMP
    printf("indices  = %lu isize = %ld\n", indices.size(), isize); // TEMP

    // allocate some space for all of our attributes
    glBufferData( GL_ARRAY_BUFFER, vsize + nsize + tcsize, NULL, GL_STATIC_DRAW );

    // upload positions
    glBufferSubData( GL_ARRAY_BUFFER, vstart, vsize, &vertices[0].x );
    glVertexAttribPointer( POSITION_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, 0, (void *) vstart );
    glEnableVertexAttribArray( POSITION_ATTRIB_INDEX );
    GL_WARN_IF_ERROR();

    // upload normals
    glBufferSubData( GL_ARRAY_BUFFER, nstart, nsize, &normals[0].x );
    glVertexAttribPointer( NORMAL_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, 0, (void *) nstart );
    glEnableVertexAttribArray( NORMAL_ATTRIB_INDEX );
    GL_WARN_IF_ERROR();

    // upload UV coordinates
    if( tcsize > 0 ) {
        glBufferSubData( GL_ARRAY_BUFFER, tcstart, tcsize, &textureUVCoords[0].u );
        glVertexAttribPointer( TEX_COORD_ATTRIB_INDEX, 2, GL_FLOAT, GL_FALSE, 0, (void *) tcstart );
        glEnableVertexAttribArray( TEX_COORD_ATTRIB_INDEX );
        GL_WARN_IF_ERROR();
    }

    // Upload vertex indices
    glGenBuffers( 1, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, isize, &indices[0], GL_STATIC_DRAW );
    GL_WARN_IF_ERROR();

    num_vertices = vertices.size();
}

void Mesh::bind()
{
    if( vao ) {
        glBindVertexArray( vao );
        GL_WARN_IF_ERROR();
    }
}

void Mesh::draw()
{
    uploadTextureUniforms();
    glBindTexture( GL_TEXTURE_2D, textureId );
    //glActiveTexture( GL_TEXTURE0 + 0 );
    if( vao ) {
        if( ibo ) {
            glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL );
        }
        else {
            // If we don't have an index buffer object, just assume we can use the vertices in order
            glDrawArrays( GL_TRIANGLES, 0, vertices.size() );
        }
        GL_WARN_IF_ERROR();
    }
}

////////////////////////////////////////////////////////////////////////
// Simple Shapes
////////////////////////////////////////////////////////////////////////

void makeMeshTetrahedron( Mesh & mesh )
{
    mesh.vertices.resize( 4 );
    mesh.normals.resize( 4 );
    mesh.indices.resize( 12 );
    
    const float one_ov_sqrt_two = 0.70710678118;
    // length of a side
    const float len = 1.0;
    const float halflen = len * 0.5;

    mesh.vertices[0] = Vector4(  halflen,  0.0, -one_ov_sqrt_two * halflen );
    mesh.vertices[1] = Vector4( -halflen,  0.0, -one_ov_sqrt_two * halflen );
    mesh.vertices[2] = Vector4(  0.0, -halflen,  one_ov_sqrt_two * halflen );
    mesh.vertices[3] = Vector4(  0.0,  halflen,  one_ov_sqrt_two * halflen );

    mesh.normals[0] = mesh.vertices[0].normalized();
    mesh.normals[1] = mesh.vertices[1].normalized();
    mesh.normals[2] = mesh.vertices[2].normalized();
    mesh.normals[3] = mesh.vertices[3].normalized();
    
    uint32_t index = 0;
    mesh.indices[index++] = 0;
    mesh.indices[index++] = 1;
    mesh.indices[index++] = 2;
    mesh.indices[index++] = 1;
    mesh.indices[index++] = 2;
    mesh.indices[index++] = 3;
    mesh.indices[index++] = 0;
    mesh.indices[index++] = 1;
    mesh.indices[index++] = 3;
    mesh.indices[index++] = 0;
    mesh.indices[index++] = 3;
    mesh.indices[index++] = 2;
}

void makeMeshCube( Mesh & mesh )
{
    // IMPLEMENT ME
}

void makeMeshGroundPlatform( Mesh & mesh, float size )
{
    mesh.vertices.resize( 4 );
    mesh.normals.resize( 4 );
    mesh.indices.resize( 6 );
    mesh.textureUVCoords.resize( 4 );
    
    float yoffset = 0.0;
    
    mesh.vertices[0] = Vector4( -size / 2.0, yoffset, -size / 2.0 );
    mesh.vertices[1] = Vector4( -size / 2.0, yoffset, size / 2.0 );
    mesh.vertices[2] = Vector4( size / 2.0, yoffset, size / 2.0 );
    mesh.vertices[3] = Vector4( size / 2.0, yoffset, -size / 2.0 );
    
    Vector4 up( 0.0, 1.0, 0.0 );
    mesh.normals[0] = up;
    mesh.normals[1] = up;
    mesh.normals[2] = up;
    mesh.normals[3] = up;

    uint32_t index = 0;
    mesh.indices[index++] = 0;
    mesh.indices[index++] = 1;
    mesh.indices[index++] = 2;
    mesh.indices[index++] = 0;
    mesh.indices[index++] = 2;
    mesh.indices[index++] = 3;

    mesh.textureUVCoords[0] = { 0.0, 0.0 };
    mesh.textureUVCoords[1] = { 0.0, 1.0 };
    mesh.textureUVCoords[2] = { 1.0, 1.0 };
    mesh.textureUVCoords[3] = { 1.0, 0.0 };
}

