/*
 *  ShaderProgram.h
 *  FastRender
 *
 *  Created by David Cunningham on 9/15/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

#include <string>

#include "OpenGLUtil.h"

const char * shaderTypeAsString( GLuint type );

class Shader {
    public:
        Shader() : id(0) {}
        ~Shader() {}

        void loadFile( GLuint type, const std::string filename );
        void loadSource( GLuint type, const std::string & src );

        GLuint id;
};

class Program {
    public:
        Program() : id(0) {}
        ~Program() {}

        void create();
        void attach( Shader & shader );
        void link();

        GLuint id;
};




#endif
