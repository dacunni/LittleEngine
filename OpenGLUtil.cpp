#include <stdio.h>
#include "OpenGLUtil.h"

GLenum warnIfError( const char * func, int line ) 
{
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {
        printf("GL Error: %s:%d %s\n", func, line, gluErrorString(err));
    }

    return err;
}


