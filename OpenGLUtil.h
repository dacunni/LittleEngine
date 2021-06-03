#ifndef _OPENGL_UTIL_H_
#define _OPENGL_UTIL_H_

#include <string>

#define USE_GLFW

#include <GL/glew.h>

#ifdef USE_GLFW

#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#else
#   ifdef __APPLE__
#   include <GLUT/glut.h>      // GLUT + OpenGL
#   include <OpenGL/gl3.h>     // Core OpenGL 3.x+
#   else
#   include <GL/glut.h>
#   include <GL/gl.h>
#   endif
#endif // USE_GLFW

#define GL_WARN_IF_ERROR() warnIfError(__FUNCTION__, __LINE__)

GLenum warnIfError(const char * func, int line);

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

std::string OpenGLErrorString(GLenum status);

#endif
