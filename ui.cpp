#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sstream>

#include "AssetLoader.h"
#include "Transform.h"
#include "TriangleMesh.h"
#include "OpenGLUtil.h"
#include "GPUMesh.h"
#include "GPUPointCloud.h"
#include "RandomNumberGenerator.h"
#include "BoundingVolume.h"

int window_width = 650;
int window_height = 650;

TriangleMesh * mesh = nullptr;
GPUMesh gpu_mesh;
GPUPointCloud gpu_point_cloud;

GLuint mesh_shader_program = 0;
GLuint point_cloud_shader_program = 0;

int anim_timeout_millis = 33;
float anim_rotation = 0.0f;
float anim_rot_step = 0.02;
float anim_time = 0.0f;
float anim_time_step = 0.1;

Vector4 cameraPosition( 0.0, 0.0, 0.0 );

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
    }
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

void animTimerCallback( int value )
{
    anim_rotation += anim_rot_step;
    anim_time += anim_time_step;
    glutPostRedisplay();
}

void repaintViewport( void ) 
{
    //printf("repaint\n");
    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    Matrix4x4 projection;
    Transform model_rotation;
    Transform model_translation;
    Transform model_view;

    projection.glProjectionSymmetric( 0.25, 0.2, 0.5, 100.0 );

    model_rotation = makeRotation( anim_rotation, Vector4( 0, 1, 0 ) );
    //model_rotation = makeRotation( 0.0, Vector4( 0, 1, 0 ) );
    // bunny and dragon
    //model_translation = makeTranslation( Vector4( 0, -1.0, -5.0 ) );
    // tetrahedron
    model_translation = makeTranslation( Vector4( 0.0, 0.0, -5.0 ) );
    model_view = compose( model_translation, model_rotation );
    Transform camera_translation = makeTranslation( Vector4( -cameraPosition.x,
                                                             -cameraPosition.y,
                                                             -cameraPosition.z ) );
    model_view = compose( camera_translation, model_view );

    Matrix4x4 model_view_projection;
    mult( projection, model_view.fwd, model_view_projection );
    //mult( model_view.fwd, projection, model_view_projection );
    //model_view_projection = projection;
    //model_view_projection.at( 0, 3 ) += 3.0;

    if( mesh ) {
        if( mesh_shader_program != 0 ) {
            glUseProgram( mesh_shader_program );
        }

        if( !gpu_mesh.uploaded() ) {
            gpu_mesh.upload( *mesh );
        }

        if( gpu_mesh.uploaded() ) {
            //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes
            //glFrontFace( GL_CCW );
            //glEnable(GL_CULL_FACE);

            GLint proj_loc = glGetUniformLocation( mesh_shader_program, "projection" );
            GL_WARN_IF_ERROR();
            GLint mv_loc = glGetUniformLocation( mesh_shader_program, "model_view" );
            GL_WARN_IF_ERROR();
            GLint anim_time_loc = glGetUniformLocation( mesh_shader_program, "anim_time" );
            GL_WARN_IF_ERROR();
            glUniformMatrix4fv( proj_loc, 1, GL_TRUE, projection.data );
            glUniformMatrix4fv( mv_loc, 1, GL_TRUE, model_view.fwd.data );
            glUniform1f( anim_time_loc, anim_time );
            GL_WARN_IF_ERROR();
            gpu_mesh.bind();
            gpu_mesh.draw();
        }
    }

    if( point_cloud_shader_program != 0 ) {
        glUseProgram( point_cloud_shader_program );
    }

    if( !gpu_point_cloud.uploaded() ) {
        buildPointCloud();
    }

    if( gpu_point_cloud.uploaded() ) {
        GLint proj_loc = glGetUniformLocation( point_cloud_shader_program, "projection" );
        GL_WARN_IF_ERROR();
        //GLint mv_loc = glGetUniformLocation( point_cloud_shader_program, "model_view" );
        GL_WARN_IF_ERROR();
        glUniformMatrix4fv( proj_loc, 1, GL_TRUE, projection.data );
        GL_WARN_IF_ERROR();
        //glUniformMatrix4fv( mv_loc, 1, GL_TRUE, model_view.fwd.data );
        GL_WARN_IF_ERROR();
        gpu_point_cloud.bind();
        gpu_point_cloud.draw();
    }

    glDisable( GL_DEPTH_TEST );
    glutSwapBuffers();
    glutTimerFunc( anim_timeout_millis, animTimerCallback, 0 );
}

const char * shaderTypeAsString( GLuint type )
{
    switch( type ) {
        case GL_VERTEX_SHADER:
            return "VERTEX SHADER";
        case GL_FRAGMENT_SHADER:
            return "FRAGMENT SHADER";
        default:
            return "UNKNOWN SHADER";
    }
}

GLuint loadShader( GLuint type, const std::string & filename )
{
    const char * type_string = shaderTypeAsString( type );
    int status = 0;

    std::ifstream ifs( filename );
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string src = ss.str();
    // FIXME - add error handling

    printf( ">>>> %s >>>>\n%s<<<< %s <<<<\n", type_string, src.c_str(), type_string );
    GLuint shader = glCreateShader( type );    

    // Make an array of pointers for GL
    const char * srcs[] = { src.c_str() };
    glShaderSource( shader, 1, srcs, NULL );

    // Compile shader
    glCompileShader( shader );
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );

    printf( "Shader Compile status: %d\n", status );

    if( !status ) {
        GLint len = 0;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetShaderInfoLog( shader, len, &len, &log[0] );
        printf( "Compiler Error Message:\n%s", (char *) &log[0] );
        glDeleteShader( shader );
        return 0;
    }

    // FIXME - add error handling

    return shader;
}

GLuint createShaders( const char * vs, const char * fs ) 
{
    GLuint status = 0;
    GLint program_status = 0;
    GLuint shader_program = 0;

    GLuint vertex_shader = loadShader( GL_VERTEX_SHADER, vs );
    GLuint fragment_shader = loadShader( GL_FRAGMENT_SHADER, fs );

    if( !vertex_shader || !fragment_shader )
        return 0;

    shader_program = glCreateProgram();
    if( vertex_shader != 0 )
        glAttachShader( shader_program, vertex_shader );
    if( fragment_shader != 0 ) 
        glAttachShader( shader_program, fragment_shader );
    glLinkProgram( shader_program );

    glGetProgramiv( shader_program, GL_LINK_STATUS, &program_status ); 
    printf( "Link status: %d\n", program_status );

    if( !program_status ) {
        GLint len = 0;
        glGetProgramiv( shader_program, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetProgramInfoLog( shader_program, len, &len, &log[0] );
        printf( "Compiler Error Message:\n%s", (char *) &log[0] );
        glDeleteProgram( shader_program );
        return 0; 
    }
    // FIXME - add error handling

    return shader_program;
}

int main (int argc, char * const argv[]) 
{
    const char * vertex_shader_filename = "basic.vs";
    const char * fragment_shader_filename = "basic.fs";

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
    point_cloud_shader_program = createShaders( "points.vs", "points.fs" );

    if( !mesh_shader_program || !point_cloud_shader_program ) {
        return -1;
    }

    glutReshapeFunc( viewportReshaped );
    glutDisplayFunc( repaintViewport );
    glutKeyboardFunc( keyPressed );

    AssetLoader loader;
    std::string modelPath = "models";

    // dragon
    std::string dragonPath = modelPath + "/stanford/dragon/reconstruction";
    //mesh = loader.load( dragonPath + "/dragon_vrip_res4.ply" );
    //mesh = loader.load( dragonPath + "/dragon_vrip_res3.ply" );
    //mesh = loader.load( dragonPath + "/dragon_vrip_res2.ply" );
    //mesh = loader.load( dragonPath + "/dragon_vrip.ply" );

    // bunnies
    std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";
    //mesh = loader.load( bunnyPath + "/bun_zipper_res4.ply" );
    //mesh = loader.load( bunnyPath + "/bun_zipper_res3.ply" );
    //mesh = loader.load( bunnyPath + "/bun_zipper_res2.ply" );
    //mesh = loader.load( bunnyPath + "/bun_zipper.ply" );

    mesh = new TriangleMesh();
    makeTriangleMeshTetrahedron( *mesh );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return EXIT_FAILURE;
    }

    BoundingVolume * meshBB = new BoundingVolume();
    meshBB->buildAxisAligned( mesh );
    meshBB->print();
    delete meshBB;

    GL_WARN_IF_ERROR();
    glutMainLoop();
    return EXIT_SUCCESS;
}

