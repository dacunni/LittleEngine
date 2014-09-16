/*
 *  ShaderProgram.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 9/15/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "ShaderProgram.h"


const char * shaderTypeAsString( GLuint type )
{
    switch( type ) {
        case GL_VERTEX_SHADER:
            return "VERTEX SHADER";
        case GL_FRAGMENT_SHADER:
            return "FRAGMENT SHADER";
        default:
            return "UNKNOWN SHADER";
    }
}

void Shader::loadFile( GLuint type, const std::string filename )
{
    std::ifstream ifs( filename );
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string src = ss.str();
    loadSource( type, src );
}

void Shader::loadSource( GLuint type, const std::string & src )
{
    const char * type_string = shaderTypeAsString( type );
    int status = 0;

    id = glCreateShader( type );    
    printf( ">>>> %s id=%u >>>>\n%s<<<< %s id=%u <<<<\n", 
            type_string, id, src.c_str(), type_string, id );

    // Make an array of pointers for GL
    const char * srcs[] = { src.c_str() };
    glShaderSource( id, 1, srcs, NULL );

    // Compile shader
    glCompileShader( id );
    glGetShaderiv( id, GL_COMPILE_STATUS, &status );

    printf( "Shader Compile status: %d\n", status );

    if( !status ) {
        GLint len = 0;
        glGetShaderiv( id, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetShaderInfoLog( id, len, &len, &log[0] );
        printf( "Compiler Error Message:\n%s", (char *) &log[0] );
        glDeleteShader( id );
        return;
    }
}



