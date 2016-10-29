#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <sstream>

#include "AssetLoader.h"
#include "AxisAlignedSlab.h"
#include "Transform.h"
#include "TriangleMesh.h"
#include "OpenGLUtil.h"
#include "GameObject.h"
#include "GPUMesh.h"
#include "GPUPointCloud.h"
#include "RandomNumberGenerator.h"
#include "BoundingVolume.h"
#include "ShaderProgram.h"

int window_width = 650;
int window_height = 650;

int mouse_button_state[3] = { 
    GLUT_UP, // left
    GLUT_UP, // right
    GLUT_UP  // middle
};
int mouse_last_x = -1;
int mouse_last_y = -1;

GPUPointCloud gpu_point_cloud;

GameObject * hero = nullptr;
GameObject * enemy = nullptr;
GameObject * ground = nullptr;

GLuint mesh_shader_program = 0;
GLuint point_cloud_shader_program = 0;
GLuint ground_shader_program = 0;

double start_time = -1.0;
int anim_timeout_millis = 33;
float anim_rotation = 0.0f;
float anim_time = 0.0f;

bool draw_wireframes = false;

Vector4 cameraPosition( 0.0, 0.6, 0.0 );

void keyPressed( unsigned char key, int width, int height ) 
{
    switch( key ) {
        case 'k':
            cameraPosition.z -= 0.25;
            glutPostRedisplay();
            break;
        case 'j':
            cameraPosition.z += 0.25;
            glutPostRedisplay();
            break;
        case 'w':
            draw_wireframes = !draw_wireframes;
            glutPostRedisplay();
            break;
    }
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

    cameraPosition.x += 0.05 * dx;
    cameraPosition.z += 0.05 * dy;
    glutPostRedisplay();

    mouse_last_x = x;
    mouse_last_y = y;
}

void viewportReshaped( int width, int height ) 
{
    //printf("reshape: %d x %d\n", width, height);
    window_width = width;
    window_height = height;
    glViewport( 0, 0, window_width, window_height );
    GL_WARN_IF_ERROR();
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
#if 1
        p.x = rng.uniformRange( -3.0, -2.0 );
        p.y = rng.uniformRange( -1.0, 1.0 );
        p.z = rng.uniformRange( -6.0, -10.0 );
#else
        rng.uniformSurfaceUnitSphere( p );     
        //rng.uniformSurfaceUnitHalfSphere( dir, p );     
#endif
        points.push_back( p );
    }
    gpu_point_cloud.setPointSize( 1 );
    gpu_point_cloud.upload( points );
}

double timeAsDouble()
{
    struct timeval tm = {};
    gettimeofday( &tm, NULL );
    return (double) tm.tv_sec + 1.0e-6 * (double) tm.tv_usec;
}

void animTimerCallback( int value )
{
    anim_time = (float) (timeAsDouble() - start_time);
    anim_rotation = anim_time * 0.4;
    glutPostRedisplay();
}

void repaintViewport( void ) 
{
    //printf("repaint\n");
    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    Matrix4x4 projection;
    projection.glProjectionSymmetric( 0.25, 0.2, 0.5, 100.0 );

    Transform camera_translation = makeTranslation( Vector4( -cameraPosition.x, -cameraPosition.y, -cameraPosition.z ) );

    if( draw_wireframes ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes
        glFrontFace( GL_CCW );
        glEnable( GL_CULL_FACE );
    }

    if( ground ) {
        Transform model_translation = makeTranslation( Vector4( 0.0, 0.0, 0.0 ) );
        Transform model_rotation = makeRotation( anim_rotation, Vector4( 1, 1, 0 ) );
        Transform model_view = compose( model_translation, model_rotation );
        model_view = model_translation;
        model_view = compose( camera_translation, model_view );

        ground->gpu_mesh.setShaderProgram( ground_shader_program );
        ground->gpu_mesh.setModelViewMatrix( model_view.fwd );
        ground->gpu_mesh.setProjection( projection );
        ground->draw();
    }

    if( hero ) {
        Transform model_translation = makeTranslation( Vector4( 0.0, 0.0, -5.0 ) );
        Transform model_rotation = makeRotation( anim_rotation, Vector4( 0, 1, 0 ) );
        Transform model_view = compose( model_translation, model_rotation );
        model_view = compose( camera_translation, model_view );

        hero->gpu_mesh.setShaderProgram( mesh_shader_program );
        hero->gpu_mesh.setModelViewMatrix( model_view.fwd );
        hero->gpu_mesh.setProjection( projection );
        hero->draw();
    }

    if( enemy ) {
        Transform model_translation = makeTranslation( Vector4( 1.0, 0.0, -5.0 ) );
        Transform model_rotation = makeRotation( anim_rotation, Vector4( 0, 1, 0 ) );
        Transform model_view = compose( model_translation, model_rotation );
        model_view = compose( camera_translation, model_view );

        enemy->gpu_mesh.setShaderProgram( mesh_shader_program );
        enemy->gpu_mesh.setModelViewMatrix( model_view.fwd );
        enemy->gpu_mesh.setProjection( projection );
        enemy->draw();
        enemy->draw();
    }

    if( draw_wireframes ) {                           // revert to normal behavior
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  // Draw polygons filled
        glDisable( GL_CULL_FACE );
    }

    if( point_cloud_shader_program != 0 ) {
        glUseProgram( point_cloud_shader_program );
    }

    if( !gpu_point_cloud.uploaded() ) {
        buildPointCloud();
    }

    if( gpu_point_cloud.uploaded() ) {
        Transform model_translation = makeTranslation( Vector4( 0.0, 0.0, 0.0 ) );
        //Transform model_rotation = makeRotation( anim_rotation, Vector4( 0, 1, 0 ) );
        //Transform model_view = compose( model_translation, model_rotation );
        Transform model_view = model_translation;
        model_view = compose( camera_translation, model_view );
        GLint proj_loc = glGetUniformLocation( point_cloud_shader_program, "projection" );
        GL_WARN_IF_ERROR();
        GLint mv_loc = glGetUniformLocation( point_cloud_shader_program, "model_view" );
        GL_WARN_IF_ERROR();
        glUniformMatrix4fv( proj_loc, 1, GL_TRUE, projection.data );
        GL_WARN_IF_ERROR();
        glUniformMatrix4fv( mv_loc, 1, GL_TRUE, model_view.fwd.data );
        GL_WARN_IF_ERROR();
        gpu_point_cloud.bind();
        gpu_point_cloud.draw();
    }

    glDisable( GL_DEPTH_TEST );
    glutSwapBuffers();
    glutTimerFunc( anim_timeout_millis, animTimerCallback, 0 );
}

GLuint createShaders( const char * vs, const char * fs ) 
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

int main (int argc, char * const argv[]) 
{
    const char * vertex_shader_filename = "shaders/basic.vs";
    const char * fragment_shader_filename = "shaders/basic.fs";

    printf("FastRender UI\n");

    glutInit( &argc, const_cast<char **>(argv) );
    glutInitDisplayMode( GLUT_DOUBLE              // Double buffered
                         | GLUT_RGBA | GLUT_DEPTH
                         | GLUT_3_2_CORE_PROFILE  // Core profile context
                         );
    glutInitWindowSize( window_width, window_height );
    glutInitWindowPosition( 0, 0 );
    glutCreateWindow("FastRender UI");

    printf( "Renderer: %s\n", glGetString( GL_RENDERER ) );
    printf( "GL Version: %s\n", glGetString( GL_VERSION ) );
    printf( "GLSL Version: %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

    //
    // Parse command line arguments
    //
    const char * options = "v:f:";
    int opt = 0;

    while( (opt = getopt( argc, argv, options )) > -1 ) {
        if( opt == '?' ) {
            exit(-1);
        }
        switch( opt ) {
            case 'v': vertex_shader_filename = optarg; break;
            case 'f': fragment_shader_filename = optarg; break;
            default:
                fprintf( stderr, "Unsupported option '-%c'\n", opt );
        }
    }

    mesh_shader_program = createShaders( vertex_shader_filename, fragment_shader_filename );
    point_cloud_shader_program = createShaders( "shaders/points.vs", "shaders/points.fs" );
    ground_shader_program = createShaders( "shaders/ground.vs", "shaders/ground.fs" );

    if( !mesh_shader_program || !point_cloud_shader_program || !ground_shader_program ) {
        return -1;
    }

    glutReshapeFunc( viewportReshaped );
    glutDisplayFunc( repaintViewport );
    glutKeyboardFunc( keyPressed );
    glutMouseFunc( mouseButton );
    glutMotionFunc( mouseMotionWhileButtonPressed );

    AssetLoader loader;
    std::string modelPath = "models";

    printf("Loading game objects\n");
    std::string dragonPath = modelPath + "/stanford/dragon/reconstruction";
    std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";
    hero = new GameObject( bunnyPath + "/bun_zipper_res2.ply" );
    enemy = new GameObject( dragonPath + "/dragon_vrip_res2.ply" );

    TriangleMesh * ground_mesh = new TriangleMesh();
    makeTriangleMeshGroundPlatform( *ground_mesh, 30.0 );
    ground = new GameObject( ground_mesh );

    GL_WARN_IF_ERROR();
    start_time = timeAsDouble();
    glutMainLoop();
    return EXIT_SUCCESS;
}

void Program::create()
{
    printf( "Creating program\n");
    id = glCreateProgram();
}

void Program::attach( Shader & shader )
{
    if( shader.id != 0 ) {
        glAttachShader( id, shader.id );
    }
}

void Program::link()
{
    GLint status = 0;
    printf( "Linking program\n");
    glLinkProgram( id );
    glGetProgramiv( id, GL_LINK_STATUS, &status ); 
    printf( "Link status: %d\n", status );

    if( !status ) {
        GLint len = 0;
        glGetProgramiv( id, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetProgramInfoLog( id, len, &len, &log[0] );
        printf( "Linker Error Message:\n%s", (char *) &log[0] );
        glDeleteProgram( id );
    }
}

