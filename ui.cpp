#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <sstream>

#include "AssetLoader.h"
#include "AxisAlignedSlab.h"
#include "Image.h"
#include "Transform.h"
#include "OpenGLUtil.h"
#include "GameObject.h"
#include "Mesh.h"
#include "PointCloud.h"
#include "RandomNumberGenerator.h"
#include "ShaderProgram.h"

int window_width = 1200;
int window_height = 650;

int mouse_button_state[3] = { 
    GLUT_UP, // left
    GLUT_UP, // right
    GLUT_UP  // middle
};
int mouse_last_x = -1;
int mouse_last_y = -1;

PointCloud point_cloud;

GameObject * hero = nullptr;
GameObject * enemy = nullptr;
GameObject * ground = nullptr;
GameObject * tetra = nullptr;
std::vector<GameObject*> game_objects; // TODO

std::string modelPath = "models";
std::string texturePath = "textures";
std::string dragonPath = modelPath + "/stanford/dragon/reconstruction";
std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";

const char * vertex_shader_filename = "shaders/basic.vs";
const char * fragment_shader_filename = "shaders/basic.fs";

double start_time = -1.0;
int anim_timeout_millis = 33;
float anim_rotation = 0.0f;
float anim_time = 0.0f;

bool draw_wireframes = false;

Vector4 cameraPosition( 0.0, 0.6, 0.0 );
float cameraXRotation = 0.0;
float cameraYRotation = 0.0;


unsigned char keyState[256] = {0};

void userTimerUpdate( double time_now, double delta_time )
{
    if( keyState['w'] ) {
        cameraPosition.z -= 0.25;
        glutPostRedisplay();
    }
    if( keyState['s'] ) {
        cameraPosition.z += 0.25;
        glutPostRedisplay();
    }
    if( keyState['a'] ) {
        cameraPosition.x -= 0.25;
        glutPostRedisplay();
    }
    if( keyState['d'] ) {
        cameraPosition.x += 0.25;
        glutPostRedisplay();
    }
}

void keyPressed( unsigned char key, int x, int y ) 
{
    switch( key ) {
        case 'W':
            draw_wireframes = !draw_wireframes;
            glutPostRedisplay();
            break;
    }
    keyState[key] = 1;
}

void keyReleased( unsigned char key, int x, int y ) 
{
    keyState[key] = 0;
}

void mouseButton( int button, int state, int x, int y )
{
    mouse_button_state[button] = state;
    mouse_last_x = x;
    mouse_last_y = y;
}

void mouseMotionWhileButtonPressed( int x, int y )
{
    int dx = x - mouse_last_x;
    int dy = y - mouse_last_y;

    //cameraPosition.x += 0.05 * dx;
    //cameraPosition.z += 0.05 * dy;
    cameraXRotation += 0.01 * dy;
    cameraYRotation += 0.01 * dx;
    glutPostRedisplay();

    mouse_last_x = x;
    mouse_last_y = y;
}

void viewportReshaped( int width, int height ) 
{
    window_width = width;
    window_height = height;
    glViewport( 0, 0, window_width, window_height );
    GL_WARN_IF_ERROR();
}

GLuint createShaderProgram( const char * vs, const char * fs ) 
{
    Program program;
    Shader vertex_shader;
    Shader fragment_shader;

    vertex_shader.loadFile( GL_VERTEX_SHADER, vs );
    fragment_shader.loadFile( GL_FRAGMENT_SHADER, fs );

    if( !vertex_shader.id || !fragment_shader.id )
        return 0;

    program.create();
    program.attach( vertex_shader );
    program.attach( fragment_shader );
    program.link();

    return program.id;
}


void buildPointCloud( void ) 
{
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
    point_cloud.upload( points );
    GLuint point_cloud_shader_program = createShaderProgram( "shaders/points.vs", "shaders/points.fs" );
    point_cloud.setShaderProgram( point_cloud_shader_program );
}

double timeAsDouble()
{
    struct timeval tm = {};
    gettimeofday( &tm, NULL );
    return (double) tm.tv_sec + 1.0e-6 * (double) tm.tv_usec;
}

void animTimerCallback( int value )
{
    double time_now = timeAsDouble() - start_time;
    double delta_time = time_now - anim_time;
    anim_time = (float) time_now;

    userTimerUpdate( time_now, delta_time );
    for(auto obj : game_objects ) {
        obj->updateAnimation( time_now, delta_time );
    }

    anim_rotation = anim_time * 0.4;
    glutPostRedisplay();
}

void repaintViewport( void ) 
{
    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    Matrix4x4 projection;
    projection.glProjectionSymmetric( 0.20 * (float) window_width / window_height, 0.20, 0.25, 100.0 );

    Transform camera_translation = makeTranslation( Vector4( -cameraPosition.x, -cameraPosition.y, -cameraPosition.z ) );
    Transform camera_rotation = 
        compose( makeRotation( cameraYRotation, Vector4( 0, 1, 0 ) ),
                 makeRotation( cameraXRotation, Vector4( 1, 0, 0 ) ) );
    Transform camera = compose( camera_translation, camera_rotation );

    if( draw_wireframes ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes
        glFrontFace( GL_CCW );
        glEnable( GL_CULL_FACE );
    }

    for(auto obj : game_objects ) {
        obj->mesh.useProgram();
        obj->mesh.setWorldMatrix( obj->worldTransform.fwd );
        obj->mesh.setViewMatrix( camera.fwd );
        obj->mesh.setProjection( projection );
        obj->draw();
    }

    if( draw_wireframes ) {                           // revert to normal behavior
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  // Draw polygons filled
        glDisable( GL_CULL_FACE );
    }

    if( !point_cloud.uploaded() ) {
        //buildPointCloud();
    }

    if( point_cloud.uploaded() ) {
        Transform model_translation = makeTranslation( Vector4( 0.0, 0.0, 0.0 ) );
        Transform world = model_translation;

        point_cloud.useProgram();
        point_cloud.setWorldMatrix( world.fwd );
        point_cloud.setViewMatrix( camera.fwd );
        point_cloud.setProjection( projection );
        point_cloud.bind();
        point_cloud.draw();
    }

    glDisable( GL_DEPTH_TEST );

    GL_WARN_IF_ERROR();

    glutSwapBuffers();
    glutTimerFunc( anim_timeout_millis, animTimerCallback, 0 );
}

void makeSimpleScene()
{
    GLuint mesh_shader_program = createShaderProgram( vertex_shader_filename, fragment_shader_filename );
    if( !mesh_shader_program ) { exit(EXIT_FAILURE); }

    hero = new GameObject( bunnyPath + "/bun_zipper_res2.ply" );
    hero->mesh.setShaderProgram( mesh_shader_program );
    hero->position = Vector4( 0.0, 0.0, -5.0 );
    hero->setAnimationFunction( [](GameObject * self, float gameTime, float deltaTime) {
        self->worldTransform = compose( makeTranslation( self->position ),
                                        makeRotation( anim_rotation, Vector4( 0, 1, 0 ) ) );
    } );
    game_objects.push_back(hero);

    enemy = new GameObject( modelPath + "/tf3dm.com/soccerball/untitled.ply" );
    enemy->mesh.setShaderProgram( mesh_shader_program );
    enemy->position = Vector4( 1.0, 0.0, -5.0 );
    game_objects.push_back(enemy);

    ground = new GameObject();
    makeMeshGroundPlatform( ground->mesh, 30.0 );
    ground->mesh.setShaderProgram( mesh_shader_program );
    {
    RGBImage<unsigned char> tex_image;
    tex_image.loadImage( texturePath + "/uvgrid.jpg" );
    GLuint texID = tex_image.uploadGL();
    ground->mesh.setTexture( texID );
    }
    game_objects.push_back(ground);

    tetra = new GameObject();
    makeMeshTetrahedron( tetra->mesh );
    tetra->mesh.setShaderProgram( mesh_shader_program );
    tetra->position = Vector4( 1.0, 1.0, -5.0 );
    game_objects.push_back(tetra);
}

void makeCookTorranceScene()
{
    GLuint cook_torrance_shader_program = createShaderProgram( "shaders/basic.vs", "shaders/cooktorrance.fs" ); 
    if( !cook_torrance_shader_program ) { exit(EXIT_FAILURE); }

    hero = new GameObject( bunnyPath + "/bun_zipper_res2.ply" );
    hero->mesh.setShaderProgram( cook_torrance_shader_program );
    hero->position = Vector4( 0.0, 0.0, -5.0 );
    game_objects.push_back(hero);

    enemy = new GameObject( modelPath + "/tf3dm.com/soccerball/untitled.ply" );
    enemy->mesh.setShaderProgram( cook_torrance_shader_program );
    enemy->position = Vector4( 1.0, 0.0, -5.0 );
    game_objects.push_back(enemy);

    GameObject * obj = nullptr;

    obj = new GameObject( modelPath + "/uvmonkey.ply" );
    obj->mesh.setShaderProgram( cook_torrance_shader_program );
    obj->position = Vector4( -2.0, 0.0, -5.0 );
    game_objects.push_back( obj );

    obj = new GameObject( modelPath + "/uvmonkey.ply" );
    obj->mesh.setShaderProgram( cook_torrance_shader_program );
    {
    RGBImage<unsigned char> tex_image;
    tex_image.loadImage( texturePath + "/uvgrid.jpg" );
    GLuint texID = tex_image.uploadGL();
    obj->mesh.setTexture( texID );
    }
    obj->position = Vector4( -1.0, 0.0, -5.0 );
    game_objects.push_back( obj );

    obj = new GameObject( modelPath + "/uvmonkey.ply" );
    obj->mesh.setShaderProgram( cook_torrance_shader_program );
    {
    RGBImage<unsigned char> tex_image;
    tex_image.loadImage( texturePath + "/Dog_Hair_UV_H_CM_1.jpg" );
    GLuint texID = tex_image.uploadGL();
    obj->mesh.setTexture( texID );
    }
    obj->position = Vector4( -1.0, 1.0, -5.0 );
    game_objects.push_back( obj );

    obj = new GameObject( dragonPath + "/dragon_vrip_res2.ply" );
    obj->mesh.setShaderProgram( cook_torrance_shader_program );
    obj->position = Vector4( +2.0, 1.0, -5.0 );
    game_objects.push_back(obj);
}


int main (int argc, char * const argv[]) 
{
    glutInit( &argc, const_cast<char **>(argv) );
    glutInitDisplayMode( GLUT_DOUBLE              // Double buffered
                         | GLUT_RGBA | GLUT_DEPTH
                         | GLUT_3_2_CORE_PROFILE  // Core profile context
                         );
    glutInitWindowSize( window_width, window_height );
    glutInitWindowPosition( 0, 0 );
    glutCreateWindow("LittleEngine");
    printf( "Renderer: %s\n", glGetString( GL_RENDERER ) );
    printf( "GL Version: %s\n", glGetString( GL_VERSION ) );
    printf( "GLSL Version: %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    //
    // Parse command line arguments
    //
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

    glutReshapeFunc( viewportReshaped );
    glutDisplayFunc( repaintViewport );
    glutKeyboardFunc( keyPressed );
    glutKeyboardUpFunc( keyReleased );
    glutMouseFunc( mouseButton );
    glutMotionFunc( mouseMotionWhileButtonPressed );

#if 1
    makeSimpleScene();
#elif 1
    makeCookTorranceScene();
#endif

    GL_WARN_IF_ERROR();
    start_time = timeAsDouble();
    glutMainLoop();
    return EXIT_SUCCESS;
}

