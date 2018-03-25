#include <stdlib.h>
#include <sys/time.h>

#include "OpenGLUtil.h"
#include "GameObject.h"
#include "Timer.h"
#include "Engine.h"

Engine * Engine::_instance = nullptr;

Transform Engine::cameraTranslation()
{
    return makeTranslation( cameraPosition.x, cameraPosition.y, cameraPosition.z );
}

Transform Engine::cameraRotation()
{
    return compose( makeRotation( cameraYRotation, Vector4( 0, 1, 0 ) ),
                    makeRotation( cameraXRotation, Vector4( 1, 0, 0 ) ) );
}

Transform Engine::cameraTransform()
{
    return compose( cameraTranslation(), cameraRotation() );
}

Vector4 Engine::cameraForward()
{
    return cameraRotation().fwd * Vector4(0, 0, -1);
}

Vector4 Engine::cameraRight()
{
    return cameraRotation().fwd * Vector4(1, 0, 0);
}

void Engine::userTimerUpdate( double timeNow, double deltaTime )
{
    // Camera controls
    //   Translation
    if( keyState['w'] ) { cameraPosition = cameraPosition + cameraForward() * cameraSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['s'] ) { cameraPosition = cameraPosition - cameraForward() * cameraSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['a'] ) { cameraPosition = cameraPosition - cameraRight() * cameraSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['d'] ) { cameraPosition = cameraPosition + cameraRight() * cameraSpeed * deltaTime; glutPostRedisplay(); }
    //   Rotation
    if( keyState['q'] ) { cameraYRotation += cameraKeyboardRotationSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['e'] ) { cameraYRotation -= cameraKeyboardRotationSpeed * deltaTime; glutPostRedisplay(); }
    // Hero controls
    if( keyState['i'] ) { hero->position.z -= heroSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['k'] ) { hero->position.z += heroSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['j'] ) { hero->position.x -= heroSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['l'] ) { hero->position.x += heroSpeed * deltaTime; glutPostRedisplay(); }
}

void Engine::createWindow(int & argc, char ** argv )
{
    glutInit( &argc, argv );
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

    registerCallbacks();

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void Engine::start()
{
    GL_WARN_IF_ERROR();
    start_time = timeNowAsDouble();
    glutMainLoop();
}

void Engine::sKeyPressed( unsigned char key, int x, int y )      { instance().keyPressed(key, x, y); }
void Engine::sKeyReleased( unsigned char key, int x, int y )     { instance().keyReleased(key, x, y); }
void Engine::sMouseButton( int button, int state, int x, int y ) { instance().mouseButton(button, state, x, y); }
void Engine::sMouseMotionWhileButtonPressed( int x, int y )      { instance().mouseMotionWhileButtonPressed(x, y); }
void Engine::sViewportReshaped( int width, int height )          { instance().viewportReshaped(width, height); }
void Engine::sRepaintViewport(void)                              { instance().repaintViewport(); }
void Engine::sAnimTimerCallback( int value )                     { instance().animTimerCallback(value); }

void Engine::registerCallbacks() {
    glutReshapeFunc( sViewportReshaped );
    glutDisplayFunc( sRepaintViewport );
    glutKeyboardFunc( sKeyPressed );
    glutKeyboardUpFunc( sKeyReleased );
    glutMouseFunc( sMouseButton );
    glutMotionFunc( sMouseMotionWhileButtonPressed );
}

void Engine::keyPressed( unsigned char key, int x, int y ) 
{
    switch( key ) {
        case 'W':
            draw_wireframes = !draw_wireframes;
            glutPostRedisplay();
            break;
        case ' ':
            if( hero->position.y < 0.01 ) {
                hero->velocity.y = 5.0;
                glutPostRedisplay();
            }
            break;
    }
    keyState[key] = 1;
}

void Engine::keyReleased( unsigned char key, int x, int y ) 
{
    keyState[key] = 0;
}

void Engine::mouseButton( int button, int state, int x, int y )
{
    mouse_button_state[button] = state;
    mouse_last_x = x;
    mouse_last_y = y;
}

void Engine::mouseMotionWhileButtonPressed( int x, int y )
{
    int dx = x - mouse_last_x;
    int dy = y - mouse_last_y;

    //cameraPosition.x += 0.05 * dx;
    //cameraPosition.z += 0.05 * dy;
    cameraXRotation -= 0.01 * dy;
    cameraYRotation -= 0.01 * dx;
    //glutPostRedisplay();

    mouse_last_x = x;
    mouse_last_y = y;
}

void Engine::viewportReshaped( int width, int height ) 
{
    window_width = width;
    window_height = height;
    glViewport( 0, 0, window_width, window_height );
    GL_WARN_IF_ERROR();
}

void Engine::animTimerCallback( int value )
{
    double time_now = timeNowAsDouble() - start_time;
    double delta_time = time_now - anim_time;
    anim_time = (float) time_now;

    userTimerUpdate( time_now, delta_time );
    for(auto obj : game_objects ) {
        obj->updateAnimation( time_now, delta_time );
    }

    anim_rotation = anim_time * 0.4;
    glutPostRedisplay();
}

void Engine::repaintViewport( void ) 
{
    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    Matrix4x4 projection;
    projection.glProjectionSymmetric( 0.20 * (float) window_width / window_height, 0.20, 0.25, 200.0 );
    Transform camera = cameraTransform();

    if( draw_wireframes ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes
        glFrontFace( GL_CCW );
        glEnable( GL_CULL_FACE );
    }

    for(auto obj : game_objects ) {
        obj->renderable->useProgram();
        obj->renderable->setWorldMatrix( obj->worldTransform.fwd );
        obj->renderable->setViewMatrix( camera.rev );
        obj->renderable->setProjection( projection );
        obj->draw();
    }

    if( draw_wireframes ) {                           // revert to normal behavior
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  // Draw polygons filled
        glDisable( GL_CULL_FACE );
    }

    glDisable( GL_DEPTH_TEST );

    GL_WARN_IF_ERROR();

    glutSwapBuffers();
    glutTimerFunc( anim_timeout_millis, sAnimTimerCallback, 0 );
}




