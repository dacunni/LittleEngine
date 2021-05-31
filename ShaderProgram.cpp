#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>

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
    printf("Loading shader from file %s\n", filename.c_str());

    std::ifstream ifs( filename );

    if(ifs.fail()) {
        std::cerr << strerror(errno) << '\n';
    }

    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string src = ss.str();
    loadSource( type, src );
    description = filename;
}

void Shader::loadSource( GLuint type, const std::string & src )
{
    const char * type_string = shaderTypeAsString( type );
    int status = 0;

    id = glCreateShader( type );    
#if 0
    printf( ">>>> %s id=%u >>>>\n%s<<<< %s id=%u <<<<\n", 
            type_string, id, src.c_str(), type_string, id );
#endif
    description = "source";

    // Make an array of pointers for GL
    const char * srcs[] = { src.c_str() };
    glShaderSource( id, 1, srcs, NULL );

    // Compile shader
    glCompileShader( id );
    glGetShaderiv( id, GL_COMPILE_STATUS, &status );

    printf( "Shader Compile status: %s\n", status ? "OK" : "ERROR" );

    if( !status ) {
        GLint len = 0;
        glGetShaderiv( id, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetShaderInfoLog( id, len, &len, &log[0] );
        printf( "Compiler Error Message:\n%s", (char *) &log[0] );
        glDeleteShader( id );
        exit(EXIT_FAILURE); // FIXME - return error
        return;
    }
}

void Program::create()
{
    id = glCreateProgram();
}

void Program::attach( Shader & shader )
{
    if( shader.id != 0 ) {
        glAttachShader( id, shader.id );
    }
}

void Program::link()
{
    GLint status = 0;
    glLinkProgram( id );
    glGetProgramiv( id, GL_LINK_STATUS, &status ); 
    printf( "Link status: %s\n", status ? "OK" : "ERROR" );

    if( !status ) {
        GLint len = 0;
        glGetProgramiv( id, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetProgramInfoLog( id, len, &len, &log[0] );
        printf( "Linker Error Message:\n%s", (char *) &log[0] );
        glDeleteProgram( id );
        exit(EXIT_FAILURE); // FIXME - return error
    }
}

bool Program::loadVertexFragmentFiles(const std::string & vsFilename,
                                      const std::string & fsFilename)
{
    vertexShader.loadFile(GL_VERTEX_SHADER, vsFilename);
    fragmentShader.loadFile(GL_FRAGMENT_SHADER, fsFilename);

    if(!vertexShader.valid() || !fragmentShader.valid()) {
        return false;
    }

    create();
    attach(vertexShader);
    attach(fragmentShader);
    link();

    return valid();
}

GLint Program::uniformLocation(const char * name)
{
    GLint loc = glGetUniformLocation(id, name);
#if 0
    printf("Uniform(program %u %s %s): %s -> %d\n", id,
           vertexShader.description.c_str(),
           fragmentShader.description.c_str(),
           name, loc); 
#endif
    GL_WARN_IF_ERROR();
    return loc;
}

void Program::setUniform(GLint loc, int value)
{
    if(loc < 0) { return; }
    glUniform1i(loc, value);
    GL_WARN_IF_ERROR();
}

void Program::setUniform(GLint loc, bool value)
{
    setUniform(loc, (int) value);
}

void Program::setUniform(GLint loc, float value)
{
    if(loc < 0) { return; }
    glUniform1f(loc, value);
    GL_WARN_IF_ERROR();
}

void Program::setUniform3fv(GLint loc, int count, const float * values)
{
    if(loc < 0) { return; }
    glUniform3fv(loc, count, values);
    GL_WARN_IF_ERROR();
}

void Program::setUniform4fv(GLint loc, int count, const float * values)
{
    if(loc < 0) { return; }
    glUniform4fv(loc, count, values);
    GL_WARN_IF_ERROR();
}

void Program::setUniformMatrix4fv(GLint loc, int count, const float * values)
{
    if(loc < 0) { return; }
    glUniformMatrix4fv(loc, count, GL_TRUE, values);
    GL_WARN_IF_ERROR();
}

std::shared_ptr<Program> createShaderProgram(const char * vs, const char * fs) 
{
    auto program = std::make_shared<Program>();

    if(!program->loadVertexFragmentFiles(vs, fs)) {
        return nullptr;
    }

    return program;
}

