#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "AssetLoader.h"
#include "Image.h"
#include "Transform.h"
#include "GameObject.h"
#include "Mesh.h"
#include "PointCloud.h"
#include "RandomNumberGenerator.h"
#include "ShaderProgram.h"
#include "Engine.h"

const char * vertex_shader_filename = "shaders/basic.vs";
const char * fragment_shader_filename = "shaders/basic.fs";

std::shared_ptr<Program> createShaderProgram( const char * vs, const char * fs ) 
{
    auto program = std::make_shared<Program>();
    if(!program->loadVertexFragmentFiles(vs, fs)) {
        return nullptr;
    }
    return program;
}

void buildPointCloud( void ) 
{
    Engine & engine = Engine::instance();
    // Make some random points
    std::vector<Vector4> points;
    RandomNumberGenerator rng;
    rng.seedCurrentTime();
    int num_points = 100000;
    Vector4 p;
    Vector4 dir( 1.0, 1.0, 0 );
    dir.normalize();
    for( int i = 0; i < num_points; i++ ) {
        p.x = rng.uniformRange( -4.0, -2.0 );
        p.y = rng.uniformRange( 0.0, 1.0 );
        p.z = rng.uniformRange( -10.0, -6.0 );
        points.push_back( p );
    }

    std::shared_ptr<PointCloud> point_cloud = std::make_shared<PointCloud>();
    point_cloud->vertices = points;
    point_cloud->upload();
    point_cloud->setShaderProgram(createShaderProgram( "shaders/points.vs", "shaders/points.fs"));

    GameObject * obj = new GameObject();
    obj->renderable = point_cloud;

    engine.game_objects.push_back( obj );
}

void makeSimpleScene()
{
    Engine & engine = Engine::instance();

    auto mesh_shader_program = createShaderProgram( vertex_shader_filename, fragment_shader_filename );
    if( !mesh_shader_program ) { exit(EXIT_FAILURE); }

    GameObject * obj = nullptr;

    GameObject * hero = new GameObject( engine.bunnyPath + "/bun_zipper_res3.ply" );
    //GameObject * hero = new GameObject( engine.bunnyPath + "/bun_zipper.ply" );
    engine.hero = hero;
    hero->renderable->setShaderProgram( mesh_shader_program );
    hero->position = Vector4( 0.0, 0.0, -5.0 );
    hero->animFunc = [](GameObject * self, float gameTime, float deltaTime) {
        Engine & engine = Engine::instance();
        self->worldTransform = compose( makeTranslation( self->position ),
                                        makeRotation( engine.anim_rotation, Vector4( 0, 1, 0 ) ) );
    };
    engine.game_objects.push_back(hero);

    obj = new GameObject( engine.modelPath + "/tf3dm.com/soccerball/untitled.ply" );
    obj->renderable->setShaderProgram( mesh_shader_program );
    obj->position = Vector4( 1.0, 0.0, -5.0 );
    engine.game_objects.push_back(obj);

    obj = new GameObject();
    obj->renderable = std::shared_ptr<Mesh>(makeMeshGroundPlatform( 30.0 ));
    obj->renderable->setShaderProgram( mesh_shader_program );
    {
    RGBImage<unsigned char> tex_image;
    tex_image.loadImage( engine.texturePath + "/uvgrid.jpg" );
    GLuint texID = tex_image.uploadGL();
    obj->renderable->setTexture( texID );
    }
    engine.game_objects.push_back(obj);

    obj = new GameObject();
    obj->renderable = std::shared_ptr<Mesh>(makeMeshTetrahedron());
    obj->renderable->setShaderProgram( mesh_shader_program );
    obj->position = Vector4( 1.0, 1.0, -5.0 );
    engine.game_objects.push_back(obj);
}

void makeCookTorranceScene()
{
    Engine & engine = Engine::instance();

    auto cook_torrance_shader_program = createShaderProgram( "shaders/basic.vs", "shaders/cooktorrance.fs" ); 
    if( !cook_torrance_shader_program ) { exit(EXIT_FAILURE); }

    GameObject * obj = nullptr;

    GameObject * hero = new GameObject( engine.bunnyPath + "/bun_zipper_res2.ply" );
    engine.hero = hero;
    hero->renderable->setShaderProgram( cook_torrance_shader_program );
    hero->position = Vector4( 0.0, 0.0, -5.0 );
    engine.game_objects.push_back(hero);

    obj = new GameObject( engine.modelPath + "/tf3dm.com/soccerball/untitled.ply" );
    obj->renderable->setShaderProgram( cook_torrance_shader_program );
    obj->position = Vector4( 1.0, 0.0, -5.0 );
    engine.game_objects.push_back(obj);

    obj = new GameObject( engine.modelPath + "/uvmonkey.ply" );
    obj->renderable->setShaderProgram( cook_torrance_shader_program );
    obj->position = Vector4( -2.0, 0.0, -5.0 );
    engine.game_objects.push_back( obj );

    obj = new GameObject( engine.modelPath + "/uvmonkey.ply" );
    obj->renderable->setShaderProgram( cook_torrance_shader_program );
    {
    RGBImage<unsigned char> tex_image;
    tex_image.loadImage( engine.texturePath + "/uvgrid.jpg" );
    GLuint texID = tex_image.uploadGL();
    obj->renderable->setTexture( texID );
    }
    obj->position = Vector4( -1.0, 0.0, -5.0 );
    engine.game_objects.push_back( obj );

    obj = new GameObject( engine.modelPath + "/uvmonkey.ply" );
    obj->renderable->setShaderProgram( cook_torrance_shader_program );
    {
    RGBImage<unsigned char> tex_image;
    tex_image.loadImage( engine.texturePath + "/Dog_Hair_UV_H_CM_1.jpg" );
    GLuint texID = tex_image.uploadGL();
    obj->renderable->setTexture( texID );
    }
    obj->position = Vector4( -1.0, 1.0, -5.0 );
    engine.game_objects.push_back( obj );

    obj = new GameObject( engine.dragonPath + "/dragon_vrip_res2.ply" );
    obj->renderable->setShaderProgram( cook_torrance_shader_program );
    obj->position = Vector4( +2.0, 1.0, -5.0 );
    engine.game_objects.push_back(obj);
}

void makeSponzaScene()
{
    Engine & engine = Engine::instance();

    auto mesh_shader_program = createShaderProgram( vertex_shader_filename, fragment_shader_filename );
    if( !mesh_shader_program ) { exit(EXIT_FAILURE); }

    GameObject * obj = nullptr;

    GameObject * hero = new GameObject( engine.bunnyPath + "/bun_zipper_res2.ply" );
    engine.hero = hero;
    hero->renderable->setShaderProgram( mesh_shader_program );
    hero->position = Vector4( 0.0, 0.0, -5.0 );
    hero->animFunc = [](GameObject * self, float gameTime, float deltaTime) {
        Engine & engine = Engine::instance();
        const Vector4 gravity( 0.0, -15.0, 0.0 );
        Vector4 acceleration = gravity;
        self->position += self->velocity * deltaTime;
        self->velocity += acceleration * deltaTime;
        if( self->position.y < 0.0 ) self->position.y = 0.0;
        self->worldTransform = compose( makeTranslation( self->position ),
                                        makeRotation( engine.anim_rotation, Vector4( 0, 1, 0 ) ) );
    };
    engine.game_objects.push_back(hero);

#if 1
    AssetLoader loader;

#if 0
    // Default texture
    RGBImage<unsigned char> tex_image;
    // TODO: Added texture loading during mesh loading
    tex_image.loadImage( engine.texturePath + "/uvgrid.jpg" );
    //tex_image.loadImage( engine.modelPath + "/gallery/gallery_small.jpg" );
    //tex_image.loadImage( engine.modelPath + "/dabrovic-sponza/01_STUB.JPG" );
    GLuint texID = tex_image.uploadGL();
#endif

    std::vector< RGBImage<unsigned char> > textures;

    std::vector<std::shared_ptr<Mesh>> meshes;
    if( loader.loadMeshes( engine.modelPath + "/casual-effects.com/fireplace_room/fireplace_room.obj",
    //if( loader.loadMeshes( engine.modelPath + "/dabrovic-sponza/sponza.obj",
    //if( loader.loadMeshes( engine.modelPath + "/casual-effects.com/gallery/gallery.obj",
    //if( loader.loadMeshes( engine.modelPath + "/san-miguel/san-miguel.obj",
                           meshes, textures, false ) ) {
        for( auto & mesh : meshes ) {
            obj = new GameObject();
            obj->renderable = mesh;
            obj->renderable->setShaderProgram( mesh_shader_program );
            engine.game_objects.push_back( obj );
        }
    }

#else
    //obj = new GameObject( engine.modelPath + "/dabrovic-sponza/sponza.obj", false );
    //obj = new GameObject( engine.modelPath + "/crytek-sponza/sponza.obj", true, 100.0 );
    //obj = new GameObject( engine.modelPath + "/san-miguel/san-miguel.obj", true, 100.0 );
    //obj = new GameObject( engine.modelPath + "/casual-effects.com/gallery/gallery.obj", true, 50.0 );
    obj = new GameObject( engine.modelPath + "/casual-effects.com/fireplace_room/fireplace_room.obj", true, 4.0 );
    obj->renderable->setShaderProgram( mesh_shader_program );
    {
    RGBImage<unsigned char> tex_image;
    // TODO: Added texture loading during mesh loading
    tex_image.loadImage( engine.texturePath + "/uvgrid.jpg" );
    //tex_image.loadImage( engine.modelPath + "/gallery/gallery_small.jpg" );
    //tex_image.loadImage( engine.modelPath + "/dabrovic-sponza/01_STUB.JPG" );
    GLuint texID = tex_image.uploadGL();
    obj->renderable->setTexture( texID );
    }
    obj->position = Vector4( 1.0, 0.0, -5.0 );
    engine.game_objects.push_back(obj);
#endif
}

void makeLotsOfThings()
{
    Engine & engine = Engine::instance();

    auto hero_torrance_shader_program = createShaderProgram( "shaders/basic.vs", "shaders/basic.fs" ); 
    if( !hero_torrance_shader_program ) { exit(EXIT_FAILURE); }
    auto cook_torrance_shader_program = createShaderProgram( "shaders/basic.vs", "shaders/cooktorrance.fs" ); 
    if( !cook_torrance_shader_program ) { exit(EXIT_FAILURE); }
    auto floor_shader_program = createShaderProgram( vertex_shader_filename, fragment_shader_filename );
    if( !floor_shader_program ) { exit(EXIT_FAILURE); }

    GameObject * obj = nullptr;

    GameObject * hero = new GameObject( engine.bunnyPath + "/bun_zipper_res2.ply" );
    engine.hero = hero;
    hero->animFunc = [](GameObject * self, float gameTime, float deltaTime) {
        Engine & engine = Engine::instance();
        const Vector4 gravity( 0.0, -15.0, 0.0 );
        Vector4 acceleration = gravity;
        self->position += self->velocity * deltaTime;
        self->velocity += acceleration * deltaTime;
        if( self->position.y < 0.0 ) self->position.y = 0.0;
        self->worldTransform = compose( makeTranslation( self->position ),
                                        makeRotation( engine.anim_rotation, Vector4( 0, 1, 0 ) ) );
    };
    hero->renderable->setShaderProgram( hero_torrance_shader_program );
    hero->position = Vector4( 0.0, 0.0, -5.0 );
    engine.game_objects.push_back(hero);

    AssetLoader loader;

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    std::string path = engine.modelPath + "/uvmonkey.ply";
    //std::string path = engine.dragonPath + "/dragon_vrip_res2.ply";
    if( !loader.loadMesh( path, *mesh, true, 1.0f ) ) {
        fprintf( stderr, "Error loading mesh '%s'\n", path.c_str() );
        exit( EXIT_FAILURE );
    }

    RGBImage<unsigned char> tex_image;
    tex_image.loadImage( engine.texturePath + "/uvgrid.jpg" );
    GLuint texID = tex_image.uploadGL();

    int xdim = 4, zdim = 4;
    float spacing = 1.25;
    for(int xi = 0; xi < xdim; xi++) {
        float x = (float) xi * spacing - (xdim - 1) / 2.0f;
        for(int zi = 0; zi < zdim; zi++) {
            float z = (float) zi * spacing - (xdim - 1) / 2.0f - 5.0;
            obj = new GameObject();
            obj->renderable = mesh;
            obj->renderable->setShaderProgram( cook_torrance_shader_program );
            obj->position = Vector4( x, 0.0, z );
            obj->renderable->setTexture( texID );
            engine.game_objects.push_back( obj );
        }
    }

    // floor
    obj = new GameObject();
    obj->renderable = std::shared_ptr<Mesh>(makeMeshGroundPlatform( 30.0 ));
    obj->renderable->setShaderProgram( floor_shader_program );
    {
    RGBImage<unsigned char> tex_image;
    tex_image.loadImage( engine.texturePath + "/uvgrid.jpg" );
    GLuint texID = tex_image.uploadGL();
    obj->renderable->setTexture( texID );
    }
    engine.game_objects.push_back(obj);
}

int main (int argc, char ** argv) 
{
    Engine & engine = Engine::instance();
    engine.createWindow(argc, argv);

    // Parse command line arguments
    const char * options = "v:f:";
    int opt = 0;

    while( (opt = getopt( argc, argv, options )) > -1 ) {
        if( opt == '?' ) { exit(-1); }
        switch( opt ) {
            case 'v': vertex_shader_filename = optarg; break;
            case 'f': fragment_shader_filename = optarg; break;
            default:
                fprintf( stderr, "Unsupported option '-%c'\n", opt );
        }
    }

    // Make a scene
    //makeSimpleScene();
    //makeCookTorranceScene();
    makeSponzaScene();
    //makeLotsOfThings();

    //buildPointCloud();

    engine.start();

    return EXIT_SUCCESS;
}

