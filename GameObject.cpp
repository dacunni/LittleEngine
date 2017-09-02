#include "AssetLoader.h"
#include "GameObject.h"
#include "TriangleMesh.h"

GameObject::GameObject( const std::string & path )
    : triangleMesh(nullptr)
{
    AssetLoader loader;
    triangleMesh = loader.load( path );

    if( !triangleMesh ) {
        fprintf( stderr, "Error loading mesh '%s'\n", path.c_str() );
        exit( EXIT_FAILURE );
    }
}

GameObject::GameObject( TriangleMesh * trimesh )
    : triangleMesh(trimesh)
{

}

GameObject::~GameObject() 
{
    delete triangleMesh;
}

void GameObject::draw()
{
    if( !triangleMesh ) {
        return;
    }

    if( !mesh.uploaded() ) {
        mesh.upload( *triangleMesh );
    }

    mesh.bind();
    mesh.draw();
}


