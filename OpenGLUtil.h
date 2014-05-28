/*
 *  OpenGLUtil.h
 *  FastRender
 *
 *  Created by David Cunningham on 4/11/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _OPENGL_UTIL_H_
#define _OPENGL_UTIL_H_

#include <GLUT/glut.h>      // GLUT + OpenGL
#include <OpenGL/gl3.h>     // Core OpenGL 3.x+

#define GL_WARN_IF_ERROR() warnIfError( __FUNCTION__, __LINE__ )

GLenum warnIfError( const char * func, int line );


#endif