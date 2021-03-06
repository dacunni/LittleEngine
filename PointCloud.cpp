
#include "PointCloud.h"

PointCloud::PointCloud() 
{

}

PointCloud::~PointCloud() 
{
    // IMPLEMENT ME - free gpu resources for VAO,etc
}

void PointCloud::upload()
{
    //printf("PointCloud::upload\n");
    vao.bind();

    // Upload vertex positions
    vbo.bind( GL_ARRAY_BUFFER);
    GLsizeiptr vsize = vertices.size() * sizeof(vertices[0]);
    // allocate some space for all of our attributes
    glBufferData( GL_ARRAY_BUFFER, vsize, NULL, GL_STATIC_DRAW);
    // upload positions
    glBufferSubData( GL_ARRAY_BUFFER, 0, vsize, &vertices[0]);
    GL_WARN_IF_ERROR();
    glVertexAttribPointer( POSITION_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray( POSITION_ATTRIB_INDEX);

    numVertices = vertices.size();
    is_uploaded = true;
}

void PointCloud::bind()
{
    vao.bind();
}

void PointCloud::draw()
{
    if( vao) {
        // Enable gl_PointSize in VS
        glEnable( GL_PROGRAM_POINT_SIZE);
        glDrawArrays( GL_POINTS, 0, numVertices);
        GL_WARN_IF_ERROR();
    }
}


