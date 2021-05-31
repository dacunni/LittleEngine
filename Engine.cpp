#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "OpenGLUtil.h"
#include "GameObject.h"
#include "Timer.h"
#include "Engine.h"
#include "Image.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

bool show_demo_window = true;

static Engine * _instance = nullptr;

Engine & Engine::instance() {
    if(!_instance) {
        _instance = new Engine();
    }
    return *_instance;
}

Engine::Engine()
{
    assert(!_instance);
    _instance = this;

    addLight(10.0, 6.0, 6.0, 50.0, 50.0, 50.0);
    addLight(-10.0, 6.0, 6.0, 50.0, 50.0, 50.0);
    addLight(0.0, 6.0, -6.0, 50.0, 50.0, 50.0);
}

Engine::~Engine()
{
    glDeleteFramebuffers(1, &mainCameraFBO);
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
    if( keyState[GLFW_KEY_I] && !anyMod) { if(hero) { hero->position.z -= heroSpeed * deltaTime; } }
    if( keyState[GLFW_KEY_K] && !anyMod) { if(hero) { hero->position.z += heroSpeed * deltaTime; } }
    if( keyState[GLFW_KEY_J] && !anyMod) { if(hero) { hero->position.x -= heroSpeed * deltaTime; } }
    if( keyState[GLFW_KEY_L] && !anyMod) { if(hero) { hero->position.x += heroSpeed * deltaTime; } }

    for(auto obj : gameObjects) {
        obj->updateAnimation(timeNow, deltaTime);
    }
}

void Engine::createWindow(int & argc, char ** argv )
{
    // TODO
    //glfwSetErrorCallback(error_callback);

    // WAR: Save the working directory because GLFW changes it
    char * workingDirectory = getwd(nullptr);
    printf("Working directory: %s\n", workingDirectory);

    if(!glfwInit()) {
        fprintf(stderr, "Error from glfwInit()\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    fbWidth = 1200;
    fbHeight = 600;

    window = glfwCreateWindow(fbWidth, fbHeight, "LittleEngine", NULL, NULL);
    if(!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if(glewInit() != GLEW_OK) {
        printf("Error initializing GLEW\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    printf( "Renderer: %s\n", glGetString( GL_RENDERER ) );
    printf( "GL Version: %s\n", glGetString( GL_VERSION ) );
    printf( "GLSL Version: %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

    registerCallbacks();

    // Initialize Dear Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 150";
    ImGui_ImplOpenGL3_Init(glsl_version);

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    fbAspectRatio = (float) fbWidth / fbHeight;

    // Framebuffers

    // Main Framebuffer
    glGenFramebuffers(1, &mainCameraFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mainCameraFBO);

    //   Color Attachment
    glGenTextures(1, &mainCameraColorTexture);
    glBindTexture(GL_TEXTURE_2D, mainCameraColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbWidth, fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainCameraColorTexture, 0);

    //   Depth Attachment
    glGenTextures(1, &mainCameraDepthTexture);
    glBindTexture(GL_TEXTURE_2D, mainCameraDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fbWidth, fbHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mainCameraDepthTexture, 0);

    //   Validation
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if(status != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Main camera FBO is not complete: " + OpenGLErrorString(status));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Shadow Map Framebuffer
    glGenFramebuffers(1, &shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    //   Depth Attachment
    glGenTextures(1, &shadowMapDepthTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fbWidth, fbHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapDepthTexture, 0);

    //   Validation
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if(status != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Shadow map FBO is not complete: " + OpenGLErrorString(status));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // WAR: Restore working directory (see above)
    chdir(workingDirectory);
    free(workingDirectory);
}

void Engine::createWindow()
{
    int argc = 1;
    char * argv[] = { "le", nullptr };
    createWindow(argc, argv);
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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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
    // Relinquish control to Dear Imgui if it wants focus
    if(ImGui::GetIO().WantCaptureKeyboard) { return; }

    //printf("window %p key %d scan %d action %d mods %d\n",
    //       window, key, scancode, action, mods);
    keyState[key] = action;
    if(action == GLFW_PRESS) {
        keyMods[key] = mods;
    }

    if(key == GLFW_KEY_W && mods & GLFW_MOD_SHIFT && action == GLFW_PRESS) {
        drawWireframes = !drawWireframes;
    }
    if(key == GLFW_KEY_SPACE && !mods && action == GLFW_PRESS) {
        if( hero->position.y < 0.01 ) {
            hero->velocity.y = 5.0;
        }
    }
}

void Engine::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    // Relinquish control to Dear Imgui if it wants focus
    if(ImGui::GetIO().WantCaptureMouse) { return; }
}

void Engine::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    // Relinquish control to Dear Imgui if it wants focus
    if(ImGui::GetIO().WantCaptureMouse) { return; }

    bool shiftPressed = keyState[GLFW_KEY_LEFT_SHIFT] || keyState[GLFW_KEY_RIGHT_SHIFT];

    if(shiftPressed) {
        // Rotation
        cameraYRotation += cameraScrollRotationSpeed * xoffset;
    }
    else {
        // Translation
        auto forward = cameraForward();
        forward.y = 0.0f;
        forward.normalize();
        auto right = cameraRight();
        right.y = 0.0f;
        right.normalize();
        cameraPosition += forward * cameraScrollSpeed * yoffset
                       -right * cameraScrollSpeed * xoffset;
    }
}

void Engine::cursorPositionCallback(GLFWwindow * window, double x, double y)
{
    // Relinquish control to Dear Imgui if it wants focus
    if(ImGui::GetIO().WantCaptureMouse) { return; }

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
    for(auto & obj : gameObjects) {
        for(auto & renderable : obj->renderables) {
            drawRenderable(projection, view, obj, renderable);
        }
    }
}

void Engine::drawRenderable( const Matrix4x4 & projection, const Matrix4x4 & view,
                             std::shared_ptr<GameObject> & obj,
                             std::shared_ptr<Renderable> & renderable )
{
    if(!renderable->visible)
        return;

    bool isHighlighted = false;

    if(auto hi = highlightedGameObject.lock()) {
        isHighlighted = (hi == obj);
    }
    if(auto hi = highlightedRenderable.lock()) {
        isHighlighted |= (hi == renderable);
    }

    renderable->useProgram();
    renderable->setWorldMatrix( obj->worldTransform.fwd );
    renderable->setViewMatrix( view );
    renderable->setProjection( projection );
    renderable->setCameraPosition( cameraPosition );
    renderable->setAnimTime( gameTime );
    renderable->setHighlighted( isHighlighted );
    renderable->setLights( (float*)lightPositions.data(), (float*)lightColors.data(), lightPositions.size() );
    renderable->bind();
    renderable->draw();
}

void Engine::drawScene()
{
    glViewport(0, 0, fbWidth, fbHeight);

    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    Matrix4x4 projection;
    projection.glProjectionSymmetric( 0.20 * (float) fbWidth / fbHeight, 0.20, 0.25, 200.0 );
    Transform camera = cameraTransform();
    Matrix4x4 & view = camera.rev;

    if( drawWireframes ) {
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
}

void Engine::repaintViewport() 
{
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    fbAspectRatio = (float) fbWidth / fbHeight;

#if 1
    // Draw scene to main camera FBO
    glBindFramebuffer(GL_FRAMEBUFFER, mainCameraFBO);
    drawScene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blit main camera FBO to window
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mainCameraFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, fbWidth, fbHeight,
                      0, 0, fbWidth, fbHeight,
                      GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

#else
    drawScene();
#endif

    glfwPollEvents();

    drawUserInterface();

    glfwSwapBuffers(window);
}

void Engine::drawUserInterface()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    drawEngineWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Engine::drawEngineWindow()
{
    ImGui::Begin("Engine");

    highlightedGameObject.reset();
    highlightedRenderable.reset();

    if(ImGui::TreeNode("Game Objects")) {
        for(auto & obj : gameObjects) {
            ImGui::PushID(obj.get());
            bool gameObjectNodeOpen = ImGui::TreeNode("Game Object");
            if(ImGui::IsItemHovered()) { highlightedGameObject = obj; }
            if(gameObjectNodeOpen) {
                if(ImGui::TreeNode("Renderables")) {
                    for(auto & renderable : obj->renderables) {
                        ImGui::PushID(renderable.get());
                        bool renderableNodeOpen = ImGui::TreeNode("Renderable");
                        if(ImGui::IsItemHovered()) { highlightedRenderable = renderable; }
                        if(renderableNodeOpen) {
                            ImGui::PushItemWidth(100);
                            ImGui::Checkbox("Visible", &renderable->visible);
                            ImGui::LabelText("Num Vertices", "%llu", renderable->numVertices);
                            ImGui::LabelText("Has Texture", "%s", renderable->hasTexture ? "YES" : "NO");
                            if(renderable->hasTexture) {
                                ImGui::Image((void*)(intptr_t)renderable->textureId, ImVec2(100, 100));
                            }
                            ImGui::LabelText("Texture ID", "%u", renderable->textureId);
                            if(ImGui::TreeNode("Material")) {
                                ImGui::SliderFloat("Fresnel F0", &renderable->F0, 0.0f, 1.0f);
                                ImGui::SliderFloat("Roughness", &renderable->roughness, 0.0f, 1.0f);
                                ImGui::TreePop();
                            }
                            ImGui::PopItemWidth();
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    if(ImGui::TreeNode("Lights")) {
        unsigned int numLights = lightPositions.size();
        for(unsigned int lightIndex = 0; lightIndex < numLights; ++lightIndex) {
            ImGui::PushID(lightIndex);
            auto & lightColor = lightColors[lightIndex];
            if(ImGui::TreeNode("Point Light")) {
                ImGui::ColorEdit3("MyColor##1", (float*)&lightColor.r,
                                  ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    if(ImGui::TreeNode("Renderer")) {
        if(ImGui::TreeNode("Main Framebuffer")) {
            if(ImGui::TreeNode("Color Texture")) {
                ImGui::Image((void*)(intptr_t)mainCameraColorTexture, ImVec2(100 * fbAspectRatio, 100), ImVec2(0,1), ImVec2(1,0));
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Depth Texture")) {
                ImGui::Image((void*)(intptr_t)mainCameraDepthTexture, ImVec2(100 * fbAspectRatio, 100), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    if(ImGui::TreeNode("Options")) {
        ImGui::Checkbox("Draw Wireframes", &drawWireframes);
        ImGui::TreePop();
    }

    if(ImGui::TreeNode("Controls")) {
        if(ImGui::TreeNode("Hero")) {
            ImGui::SliderFloat("Hero Speed", &heroSpeed, 0.0f, 30.0f);
            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Camera")) {
            if(ImGui::TreeNode("Keyboard")) {
                ImGui::SliderFloat("Translate Speed", &cameraKeyboardSpeed, 0.0f, 100.0f);
                ImGui::SliderFloat("Rotate Speed", &cameraKeyboardRotationSpeed, 0.0f, 10.0f);
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Scroll")) {
                ImGui::SliderFloat("Translate Speed", &cameraScrollSpeed, 0.0f, 5.0f);
                ImGui::SliderFloat("Rotate Speed", &cameraScrollRotationSpeed, 0.0f, 2.0f);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    ImGui::End();
}

unsigned int Engine::loadTexture(const std::string & texturePath)
{
    std::shared_ptr<ImageBase> image = loadImage(texturePath);
    GLuint id = image->uploadGL();
    unsigned int index = textureIds.size();
    textureIds.push_back(id);
    return index;
}

void Engine::addLight(float x, float y, float z,
                      float r, float g, float b)
{
    lightPositions.push_back({x, y, z});
    lightColors.push_back({r, g, b});
}

void Engine::addGameObject(const std::shared_ptr<GameObject> & obj)
{
    gameObjects.push_back(obj);
}

