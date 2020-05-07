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

#if 0
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

    engine.gameObjects.push_back( obj );
}
#endif

void makeSimpleScene()
{
    Engine & engine = Engine::instance();

    auto mesh_shader_program = createShaderProgram( vertex_shader_filename, fragment_shader_filename );
    if( !mesh_shader_program ) { exit(EXIT_FAILURE); }
    auto cook_torrance_shader_program = createShaderProgram( "shaders/basic.vs", "shaders/cooktorrance.fs" ); 
    if( !cook_torrance_shader_program ) { exit(EXIT_FAILURE); }

    GameObject * obj = nullptr;

    GameObject * hero = new GameObject( engine.bunnyPath + "/bun_zipper.ply" );
    engine.hero = hero;
    hero->setShaderProgram( mesh_shader_program );
    hero->position = Vector4( 0.0, 0.0, -5.0 );
    hero->animFunc = [](GameObject * self, float gameTime, float deltaTime) {
        Engine & engine = Engine::instance();
        const Vector4 gravity( 0.0, -15.0, 0.0 );
        Vector4 acceleration = gravity;
        self->position += self->velocity * deltaTime;
        self->velocity += acceleration * deltaTime;
        if( self->position.y < 0.0 ) self->position.y = 0.0;
        self->worldTransform = compose( makeTranslation( self->position ),
                                        makeRotation( gameTime * 0.4, Vector4( 0, 1, 0 ) ),
                                        makeScaling( 10.0 ) );
    };
    engine.gameObjects.push_back(hero);

    // Load shared textures
    RGBImage<unsigned char> uvGridImage;
    uvGridImage.loadImage( engine.texturePath + "/uvgrid.jpg" );
    GLuint uvGridTextureID = uvGridImage.uploadGL();

    obj = new GameObject( engine.modelPath + "/test_objects/mitsuba/mitsuba-sphere.obj" );
    obj->setShaderProgram( mesh_shader_program );
    obj->position = Vector4( -3.0, 0.0, 0.0 );
    engine.gameObjects.push_back(obj);

    obj = new GameObject( engine.modelPath + "/test_objects/mitsuba/mitsuba-sphere.obj" );
    obj->setShaderProgram( mesh_shader_program );
    obj->position = Vector4( 0.0, 0.0, 0.0 );
    obj->setTexture( uvGridTextureID );
    engine.gameObjects.push_back(obj);

    obj = new GameObject( engine.modelPath + "/test_objects/mitsuba/mitsuba-sphere.obj" );
    obj->setShaderProgram( cook_torrance_shader_program );
    obj->position = Vector4( 3.0, 0.0, 0.0 );
    obj->setRoughness( 0.3 );
    obj->setTexture( uvGridTextureID );
    engine.gameObjects.push_back(obj);

#if 0
    obj = new GameObject( engine.modelPath + "/uvmonkey.ply" );
    obj->setShaderProgram( cook_torrance_shader_program );
    obj->setTexture( uvGridTextureID );
    obj->setRoughness( 0.1 );
    obj->position = Vector4( -1.0, 0.5, -5.0 );
    engine.gameObjects.push_back( obj );

    int numMatTestObj = 6;
    for(int i = 0; i < numMatTestObj; i++) {
        obj = new GameObject( engine.modelPath + "/test_objects/mitsuba/mitsuba-sphere.obj" );
        obj->position = Vector4( -6.0, 0.0, -6.0 + 3.0 * i );
        obj->setShaderProgram( cook_torrance_shader_program );
        //obj->setRoughness( float(i) / float(numMatTestObj - 1) );
        obj->setRoughness( 0.1 );
        obj->setF0( float(i + 0.5f) / float(numMatTestObj) );
        obj->setTexture( uvGridTextureID );
        engine.gameObjects.push_back(obj);
    }

#if 0
    obj = new GameObject( engine.modelPath + "/casual-effects.com/bmw/bmw.obj" );
    //obj->setShaderProgram( cook_torrance_shader_program );
    obj->setShaderProgram( mesh_shader_program );
    //obj->setTexture( uvGridTextureID );
    obj->setRoughness( 0.1 );
    obj->position = Vector4( 6.0, 0.0, -8.0 );
    obj->animFunc = [](GameObject * self, float gameTime, float deltaTime) {
        self->worldTransform = compose( makeTranslation( self->position ),
                                        makeScaling( 0.01 ) );
    };
    engine.gameObjects.push_back( obj );
#endif

#if 0
    obj = new GameObject( engine.modelPath + "/casual-effects.com/living_room/living_room.obj" );
    //obj->setShaderProgram( cook_torrance_shader_program );
    obj->setShaderProgram( mesh_shader_program );
    //obj->setTexture( uvGridTextureID );
    obj->setRoughness( 0.1 );
    obj->position = Vector4( 6.0, 0.0, -8.0 );
    obj->animFunc = [](GameObject * self, float gameTime, float deltaTime) {
        self->worldTransform = compose( makeTranslation( self->position ),
                                        makeScaling( 1.0 ) );
    };
    engine.gameObjects.push_back( obj );
#endif

#endif

    obj = new GameObject();
    auto ground = std::shared_ptr<Mesh>(makeMeshGroundPlatform( 30.0 ));
    ground->upload();
    ground->setShaderProgram( mesh_shader_program );
    ground->setTexture( uvGridTextureID );
    obj->renderables.push_back(ground);
    engine.gameObjects.push_back(obj);
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
    makeSimpleScene();
    //makeCookTorranceScene();
    //makeSponzaScene();
    //makeLotsOfThings();

    //buildPointCloud();

    engine.start();

    return EXIT_SUCCESS;
}

