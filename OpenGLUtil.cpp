#include <stdio.h>
#include "OpenGLUtil.h"

GLenum warnIfError( const char * func, int line) 
{
    GLenum err = glGetError();

    if( err != GL_NO_ERROR) {
        //printf("GL Error: %s:%d %s\n", func, line, gluErrorString(err));
        printf("GL Error: %s:%d %s\n", func, line, "FIXME");
    }

    return err;
}

VertexArrayObject::VertexArrayObject() : value(0)
{
}

VertexArrayObject::~VertexArrayObject()
{
    if(value)
        glDeleteVertexArrays(1, &value);
}

void VertexArrayObject::init()
{
    glGenVertexArrays(1, &value);
}

void VertexArrayObject::bind()
{
    glBindVertexArray(value);
}

BufferObject::BufferObject() : value(0)
{
}

BufferObject::~BufferObject()
{
    if(value)
        glDeleteBuffers(1, &value);
}

void BufferObject::init()
{
    glGenBuffers(1, &value);
}

void BufferObject::bind(GLenum target)
{
    glBindBuffer(target, value);
}

std::string OpenGLErrorString(GLenum status)
{
    std::string errstr;

    switch(status) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            errstr += "Framebuffer incomplete attachment"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            errstr += "Framebuffer missing attachment"; break;
        default: errstr += "OpenGL error code + " + std::to_string(status);
    }

    return errstr;
}

