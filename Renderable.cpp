
#include <vector>
#include "Renderable.h"
#include "Matrix.h"
#include "ShaderProgram.h"

Renderable::Renderable() 
{
    vao.init();
    vbo.init();
}

Renderable::~Renderable() 
{
}

bool Renderable::uploaded()
{
    return is_uploaded;
}

void Renderable::upload()
{
}

void Renderable::bind()
{
    vao.bind();
}

void Renderable::draw()
{
    if( vao) {
        if( ibo) {
            glDrawElements( GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, NULL);
        }
        else {
            // If we don't have an index buffer object, just assume we can use the vertices in order
            glDrawArrays( GL_TRIANGLES, 0, numVertices);
        }
        GL_WARN_IF_ERROR();
    }
}

void Renderable::setShaderProgram( const std::shared_ptr<Program> & program)
{
    shaderProgram = program;
    glUseProgram(shaderProgram->id);
}

void Renderable::useProgram()
{
    glUseProgram( shaderProgram->id);
}

void Renderable::setWorldMatrix( const Matrix4x4 & mat)
{
    GLint loc = shaderProgram->uniformLocation("world");
    shaderProgram->setUniformMatrix4fv(loc, 1, mat.data);
}

void Renderable::setViewMatrix( const Matrix4x4 & mat)
{
    GLint loc = shaderProgram->uniformLocation("view");
    shaderProgram->setUniformMatrix4fv(loc, 1, mat.data);
}

void Renderable::setProjection( const Matrix4x4 & mat)
{
    GLint loc = shaderProgram->uniformLocation("projection");
    shaderProgram->setUniformMatrix4fv(loc, 1, mat.data);
}

void Renderable::setCameraPosition( const Vector4 & pos)
{
    GLint loc = shaderProgram->uniformLocation("cameraPosition");
    shaderProgram->setUniform4fv(loc, 1, pos.data);
}

void Renderable::setAnimTime( float t)
{
    GLint loc = shaderProgram->uniformLocation("anim_time");
    shaderProgram->setUniform(loc, t);
}

void Renderable::setHighlighted( bool highlighted)
{
    GLint loc = shaderProgram->uniformLocation("highlighted");
    shaderProgram->setUniform(loc, highlighted);
}

void Renderable::setLights( float * pos, float * intensity, int numLights)
{
    GLint loc;

    loc = shaderProgram->uniformLocation("numLights");
    shaderProgram->setUniform(loc, numLights);

    loc = shaderProgram->uniformLocation("lightPositions");
    shaderProgram->setUniform3fv(loc, numLights, pos);

    loc = shaderProgram->uniformLocation("lightIntensities");
    shaderProgram->setUniform3fv(loc, numLights, intensity);
}

void Renderable::setTexture( GLuint texId)
{
    textureId = texId;
    hasTexture = true;
}

void Renderable::setRoughness( float roughness)
{
    this->roughness = std::max(roughness, 0.05f);
}

void Renderable::setF0( float F0)
{
    this->F0 = F0;
}

void Renderable::setDiffuseColor( const RGBColor & c)
{
    diffuseColor = c;
}

void Renderable::uploadMaterialUniforms()
{
    GLint loc = shaderProgram->uniformLocation("roughness");
    shaderProgram->setUniform(loc, roughness);
    loc = shaderProgram->uniformLocation("F0");
    shaderProgram->setUniform(loc, F0);
    loc = shaderProgram->uniformLocation("diffuseColor");
    shaderProgram->setUniform3fv(loc, 1, &diffuseColor.r);
}

void Renderable::uploadTextureUniforms()
{
    GLint loc;
    loc = shaderProgram->uniformLocation("useTexture");
    shaderProgram->setUniform(loc, hasTexture);
    loc = shaderProgram->uniformLocation("tex");
    GLint textureUnit = 0;
    shaderProgram->setUniform(loc, textureUnit);
}

