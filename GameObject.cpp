#include "AssetLoader.h"
#include "GameObject.h"

// FIXME Move this somewhere more reasonable
static AssetLoader loader;

static GameObject::AnimationFunction defaultAnimation = [](GameObject * self, float gameTime, float deltaTime) {
    self->worldTransform = compose(makeTranslation(self->position),
                                   makeScaling(self->scaleFactor));
};

GameObject::GameObject( const std::string & path,
                        bool normalizeScale,
                        float normScaleFactor)
{
    MeshDataGroup meshDataGroup;
    std::vector<std::shared_ptr<ImageBase>> textures;

    if(!loader.loadMeshData(path, meshDataGroup, textures)) {
        fprintf( stderr, "Error loading mesh '%s'\n", path.c_str());
        exit( EXIT_FAILURE);
    }

    //printf("Mesh data ground size=%lu\n", meshDataGroup.size());

    for(auto & meshData : meshDataGroup) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        mesh->vertices.insert(mesh->vertices.end(), meshData->vertices.begin(), meshData->vertices.end());
        mesh->normals.insert(mesh->normals.end(), meshData->normals.begin(), meshData->normals.end());
        mesh->textureUVCoords.insert(mesh->textureUVCoords.end(), meshData->textureUVCoords.begin(), meshData->textureUVCoords.end());
        mesh->indices.insert(mesh->indices.end(), meshData->indices.begin(), meshData->indices.end());
        if(meshData->hasTexture) {
            mesh->setTexture(meshData->textureId);
        }
        mesh->setDiffuseColor(meshData->diffuseColor);
        mesh->upload();
        renderables.push_back(mesh);
    }

    animFunc = defaultAnimation;
}

GameObject::GameObject()
{
    animFunc = defaultAnimation;
}

GameObject::~GameObject() 
{

}

void GameObject::draw()
{
    for(auto & renderable : renderables) {
        if(!renderable->visible)
            continue;
        renderable->bind();
        renderable->draw();
    }
}

void GameObject::updateAnimation(float gameTime, float deltaTime)
{
    if( animFunc) {
        animFunc(this, gameTime, deltaTime);
    }
}

void GameObject::setShaderProgram( const std::shared_ptr<Program> & program)
{
    for(auto & r : renderables) {
        r->setShaderProgram(program);
    }
}

void GameObject::setTexture( GLuint texId)
{
    for(auto & r : renderables) {
        r->setTexture(texId);
    }
}

void GameObject::setRoughness( float roughness)
{
    for(auto & r : renderables) {
        r->setRoughness(roughness);
    }
}

void GameObject::setF0( float F0)
{
    for(auto & r : renderables) {
        r->setF0(F0);
    }
}
void GameObject::addRenderable(std::shared_ptr<Renderable> & r)
{
    renderables.push_back(r);
}
