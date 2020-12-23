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

        bool valid() const { return id != 0; }

        GLuint id;
};

class Program {
    public:
        Program() : id(0) {}
        ~Program() {}

        void create();
        void attach( Shader & shader );
        void link();
        bool valid() const { return id != 0; }

        bool loadVertexFragmentFiles(const std::string & vsFilename,
                                     const std::string & fsFilename);

        Shader vertexShader;
        Shader fragmentShader;
        GLuint id;
};

std::shared_ptr<Program> createShaderProgram(const char * vs, const char * fs);


#endif
