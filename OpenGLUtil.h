#ifndef _OPENGL_UTIL_H_
#define _OPENGL_UTIL_H_

#ifdef __APPLE__
#include <GLUT/glut.h>      // GLUT + OpenGL
#include <OpenGL/gl3.h>     // Core OpenGL 3.x+
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#define GL_WARN_IF_ERROR() warnIfError( __FUNCTION__, __LINE__ )

GLenum warnIfError( const char * func, int line );

struct VertexArrayObject {
    VertexArrayObject();
    ~VertexArrayObject();

    operator bool() const { return value != 0; }
    operator GLuint() const { return value; }

    void init();
    void bind();

    GLuint value;
};

struct BufferObject {
    BufferObject();
    ~BufferObject();

    operator bool() const { return value != 0; }
    operator GLuint() const { return value; }

    void init();
    void bind(GLenum target);

    GLuint value;
};


#endif
