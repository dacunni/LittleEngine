/*
 *  GameObject.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 5/12/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

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


