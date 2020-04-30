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
    if( keyState[GLFW_KEY_W] ) { cameraPosition = cameraPosition + cameraForward() * cameraSpeed * deltaTime; }
    if( keyState[GLFW_KEY_S] ) { cameraPosition = cameraPosition - cameraForward() * cameraSpeed * deltaTime; }
    if( keyState[GLFW_KEY_A] ) { cameraPosition = cameraPosition - cameraRight() * cameraSpeed * deltaTime; }
    if( keyState[GLFW_KEY_D] ) { cameraPosition = cameraPosition + cameraRight() * cameraSpeed * deltaTime; }
    //   Rotation
    if( keyState[GLFW_KEY_Q] ) { cameraYRotation += cameraKeyboardRotationSpeed * deltaTime; }
    if( keyState[GLFW_KEY_E] ) { cameraYRotation -= cameraKeyboardRotationSpeed * deltaTime; }
    // Hero controls
    if( keyState[GLFW_KEY_I] ) { hero->position.z -= heroSpeed * deltaTime; }
    if( keyState[GLFW_KEY_K] ) { hero->position.z += heroSpeed * deltaTime; }
    if( keyState[GLFW_KEY_J] ) { hero->position.x -= heroSpeed * deltaTime; }
    if( keyState[GLFW_KEY_L] ) { hero->position.x += heroSpeed * deltaTime; }

    for(auto obj : game_objects ) {
        obj->updateAnimation(timeNow, deltaTime);
    }
}

void Engine::createWindow(int & argc, char ** argv )
{
    // TODO
    //glfwSetErrorCallback(error_callback);

    if(!glfwInit()) {
        fprintf(stderr, "Error from glfwInit()\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(640, 480, "LittleEngine", NULL, NULL);
    if(!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    printf( "Renderer: %s\n", glGetString( GL_RENDERER ) );
    printf( "GL Version: %s\n", glGetString( GL_VERSION ) );
    printf( "GLSL Version: %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

    registerCallbacks();

    setViewport( window_width, window_height );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void Engine::start()
{
    GL_WARN_IF_ERROR();
    gameTimer.start();
    double prevTime = gameTimer.elapsed();

    while (!glfwWindowShouldClose(window)) {
        int width, height;

        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        repaintViewport();

        double now = gameTimer.elapsed();
        double dt = now - prevTime;
        double fps = 1.0 / dt;
        userTimerUpdate(now, dt);

        glfwPollEvents();
        prevTime = now;
        glfwSetWindowTitle(window, std::to_string(fps).c_str());
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}

void Engine::sKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{ instance().keyCallback(window, key, scancode, action, mods); }
void Engine::sCursorPositionCallback(GLFWwindow * window, double x, double y)
{ instance().cursorPositionCallback(window, x, y); }
void Engine::sMouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{ instance().mouseButtonCallback(window, button, action, mods); }


void Engine::registerCallbacks() {
#ifdef USE_GLFW
    printf("IMPLEMENT ME - register callbacks\n");
    glfwSetKeyCallback(window, sKeyCallback);
    glfwSetCursorPosCallback(window, sCursorPositionCallback);
    glfwSetMouseButtonCallback(window, sMouseButtonCallback);
#else
    glutReshapeFunc( sViewportReshaped );
    glutDisplayFunc( sRepaintViewport );
    glutKeyboardFunc( sKeyPressed );
    glutKeyboardUpFunc( sKeyReleased );
    glutMouseFunc( sMouseButton );
    glutMotionFunc( sMouseMotionWhileButtonPressed );
#endif
}

void Engine::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    //printf("window %p key %d scan %d action %d mods %d\n",
    //       window, key, scancode, action, mods);
    keyState[key] = action;

    if(key == GLFW_KEY_W && mods & GLFW_MOD_SHIFT && action == GLFW_PRESS) {
        draw_wireframes = !draw_wireframes;
    }
}

void Engine::keyPressed( unsigned char key, int x, int y ) 
{
#ifndef USE_GLFW
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
#endif
    keyState[key] = 1;
}

void Engine::keyReleased( unsigned char key, int x, int y ) 
{
    keyState[key] = 0;
}

void Engine::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
}

void Engine::cursorPositionCallback(GLFWwindow * window, double x, double y)
{
    double dx = x - mouse_last_x;
    double dy = y - mouse_last_y;

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS) {
        cameraXRotation -= 0.01 * dy;
        cameraYRotation -= 0.01 * dx;
    }

    mouse_last_x = x;
    mouse_last_y = y;
}

void Engine::viewportReshaped( int width, int height ) 
{
    window_width = width;
    window_height = height;
    setViewport( window_width, window_height );
    GL_WARN_IF_ERROR();
    //glutPostRedisplay();
}

void Engine::setViewport( int width, int height )
{
#ifdef __APPLE__
    // WAR for retina displays
    glViewport( 0, 0, width * 2, height * 2 );
#else
    glViewport( 0, 0, width, height );
#endif
}

void Engine::drawGameObjects( const Matrix4x4 & projection, const Matrix4x4 & view )
{
    for(auto obj : game_objects ) {
        obj->renderable->useProgram();
        obj->renderable->setWorldMatrix( obj->worldTransform.fwd );
        obj->renderable->setViewMatrix( view );
        obj->renderable->setProjection( projection );
        obj->renderable->setCameraPosition( cameraPosition );
        obj->renderable->setAnimTime( anim_time );
        obj->draw();
    }
}

void Engine::repaintViewport() 
{
    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    Matrix4x4 projection;
    projection.glProjectionSymmetric( 0.20 * (float) window_width / window_height, 0.20, 0.25, 200.0 );
    Transform camera = cameraTransform();
    Matrix4x4 & view = camera.rev;

    if( draw_wireframes ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes
        glFrontFace( GL_CCW );
        glEnable( GL_CULL_FACE );
        drawGameObjects(projection, view);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  // Draw polygons filled
        glDisable( GL_CULL_FACE );
    }
    else {
        drawGameObjects(projection, view);
    }

    glDisable( GL_DEPTH_TEST );

    GL_WARN_IF_ERROR();

    glfwSwapBuffers(window);
}




