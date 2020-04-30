#include <stdlib.h>
#include <sys/time.h>

#include "OpenGLUtil.h"
#include "GameObject.h"
#include "Timer.h"
#include "Engine.h"

Engine & Engine::instance() {
    static Engine * _instance = nullptr;
    if(!_instance) { _instance = new Engine(); }
    return *_instance;
}

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
    bool shiftPressed = keyState[GLFW_KEY_LEFT_SHIFT] || keyState[GLFW_KEY_RIGHT_SHIFT];
    bool controlPressed = keyState[GLFW_KEY_LEFT_CONTROL] || keyState[GLFW_KEY_RIGHT_CONTROL];
    bool altPressed = keyState[GLFW_KEY_LEFT_ALT] || keyState[GLFW_KEY_RIGHT_ALT];

    bool anyMod = shiftPressed || controlPressed || altPressed;

    // Camera controls
    //   Translation
    if( keyState[GLFW_KEY_W] && !anyMod) { cameraPosition += cameraForward() * cameraKeyboardSpeed * deltaTime; }
    if( keyState[GLFW_KEY_S] && !anyMod) { cameraPosition -= cameraForward() * cameraKeyboardSpeed * deltaTime; }
    if( keyState[GLFW_KEY_A] && !anyMod) { cameraPosition -= cameraRight() * cameraKeyboardSpeed * deltaTime; }
    if( keyState[GLFW_KEY_D] && !anyMod) { cameraPosition += cameraRight() * cameraKeyboardSpeed * deltaTime; }
    //   Rotation
    if( keyState[GLFW_KEY_Q] && !anyMod ) { cameraYRotation += cameraKeyboardRotationSpeed * deltaTime; }
    if( keyState[GLFW_KEY_E] && !anyMod ) { cameraYRotation -= cameraKeyboardRotationSpeed * deltaTime; }
    // Hero controls
    if( keyState[GLFW_KEY_I] && !anyMod) { hero->position.z -= heroSpeed * deltaTime; }
    if( keyState[GLFW_KEY_K] && !anyMod) { hero->position.z += heroSpeed * deltaTime; }
    if( keyState[GLFW_KEY_J] && !anyMod) { hero->position.x -= heroSpeed * deltaTime; }
    if( keyState[GLFW_KEY_L] && !anyMod) { hero->position.x += heroSpeed * deltaTime; }

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

    window = glfwCreateWindow(800, 400, "LittleEngine", NULL, NULL);
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
        gameTime = now;
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
void Engine::sScrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{ instance().scrollCallback(window, xoffset, yoffset); }
void Engine::sFramebufferSizeCallback(GLFWwindow * window, int width, int height)
{ instance().framebufferSizeCallback(window, width, height); }

void Engine::registerCallbacks() {
    glfwSetKeyCallback(window, sKeyCallback);
    glfwSetCursorPosCallback(window, sCursorPositionCallback);
    glfwSetMouseButtonCallback(window, sMouseButtonCallback);
    glfwSetScrollCallback(window, sScrollCallback);
    glfwSetFramebufferSizeCallback(window, sFramebufferSizeCallback);
}

void Engine::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    //printf("window %p key %d scan %d action %d mods %d\n",
    //       window, key, scancode, action, mods);
    keyState[key] = action;
    if(action == GLFW_PRESS) {
        keyMods[key] = mods;
    }

    if(key == GLFW_KEY_W && mods & GLFW_MOD_SHIFT && action == GLFW_PRESS) {
        draw_wireframes = !draw_wireframes;
    }
    if(key == GLFW_KEY_SPACE && !mods && action == GLFW_PRESS) {
        if( hero->position.y < 0.01 ) {
            hero->velocity.y = 5.0;
        }
    }
}

void Engine::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
}

void Engine::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    bool shiftPressed = keyState[GLFW_KEY_LEFT_SHIFT] || keyState[GLFW_KEY_RIGHT_SHIFT];

    if(shiftPressed) {
        // Rotation
        cameraYRotation += cameraScrollRotationSpeed * xoffset;
    }
    else {
        // Translation
        cameraPosition += cameraForward() * cameraScrollSpeed * yoffset
                       -cameraRight() * cameraScrollSpeed * xoffset;
    }
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

void Engine::framebufferSizeCallback(GLFWwindow * window, int width, int height)
{
}

void Engine::drawGameObjects( const Matrix4x4 & projection, const Matrix4x4 & view )
{
    float lightPositions[][3] = { 
        { 7.0, 3.0, 3.0 },
        { -7.0, 3.0, 3.0 }
    };

    float lightColors[][3] = { 
        { 1.0, 0.8, 0.8 },
        { 0.8, 0.8, 1.0 }
    };

    for(auto obj : game_objects ) {
        obj->renderable->useProgram();
        obj->renderable->setWorldMatrix( obj->worldTransform.fwd );
        obj->renderable->setViewMatrix( view );
        obj->renderable->setProjection( projection );
        obj->renderable->setCameraPosition( cameraPosition );
        obj->renderable->setAnimTime( gameTime );
        obj->renderable->setLights( lightPositions, lightColors, 2 );
        obj->draw();
    }
}

void Engine::repaintViewport() 
{
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    Matrix4x4 projection;
    projection.glProjectionSymmetric( 0.20 * (float) fbWidth / fbHeight, 0.20, 0.25, 200.0 );
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




