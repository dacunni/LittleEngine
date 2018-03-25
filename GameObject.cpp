#include "AssetLoader.h"
#include "GameObject.h"

GameObject::AnimationFunction defaultAnimation = [](GameObject * self, float gameTime, float deltaTime) {
    self->worldTransform = makeTranslation( self->position );
};

GameObject::GameObject( const std::string & path,
                        bool normalizeScale,
                        float normScaleFactor )
{
    AssetLoader loader;

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    if( !loader.loadMesh( path, *mesh, normalizeScale, normScaleFactor ) ) {
        fprintf( stderr, "Error loading mesh '%s'\n", path.c_str() );
        exit( EXIT_FAILURE );
    }

    renderable = mesh;
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
    if( !renderable ) {
        return;
    }

    if( !renderable->uploaded() ) {
        renderable->upload();
    }

    renderable->bind();
    renderable->draw();
}

void GameObject::updateAnimation(float gameTime, float deltaTime)
{
    if( animFunc ) {
        animFunc(this, gameTime, deltaTime);
    }
}

