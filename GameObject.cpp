#include "AssetLoader.h"
#include "GameObject.h"

GameObject::GameObject( const std::string & path )
{
    AssetLoader loader;

    if( !loader.loadMesh( path, mesh ) ) {
        fprintf( stderr, "Error loading mesh '%s'\n", path.c_str() );
        exit( EXIT_FAILURE );
    }
}

GameObject::GameObject()
{

}

GameObject::~GameObject() 
{

}

void GameObject::draw()
{
    if( !mesh.uploaded() ) {
        mesh.upload();
    }

    mesh.bind();
    mesh.draw();
}


