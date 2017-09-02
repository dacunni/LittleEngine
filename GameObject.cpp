#include "AssetLoader.h"
#include "GameObject.h"
#include "TriangleMesh.h"

GameObject::GameObject( const std::string & path )
    : mesh(nullptr)
{
    AssetLoader loader;
    mesh = loader.load( path );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh '%s'\n", path.c_str() );
        exit( EXIT_FAILURE );
    }
}

GameObject::GameObject( TriangleMesh * trimesh )
    : mesh(trimesh)
{

}

GameObject::~GameObject() 
{
    delete mesh;
}

void GameObject::draw()
{
    if( !mesh ) {
        return;
    }

    if( !gpu_mesh.uploaded() ) {
        gpu_mesh.upload( *mesh );
    }

    gpu_mesh.bind();
    gpu_mesh.draw();
}


