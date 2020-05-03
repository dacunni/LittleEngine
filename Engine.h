#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "Timer.h"

class Engine
{
    public:
        Engine() {}
        ~Engine() {}

        GameObject * hero = nullptr;
        std::vector<GameObject*> game_objects;

        std::string modelPath = "models";
        std::string texturePath = "textures";
        std::string dragonPath = modelPath + "/stanford/dragon/reconstruction";
        std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";

        bool draw_wireframes = false;

        Vector4 cameraPosition = Vector4( 0.0, 0.6, 5.0 );
        float cameraXRotation = 0.0;
        float cameraYRotation = 0.0;
        float cameraKeyboardSpeed = 10.0;
        float cameraScrollSpeed = 1.0;
        float cameraKeyboardRotationSpeed = 2.0;
        float cameraScrollRotationSpeed = 2.0;

        float heroSpeed =  5.0;

        unsigned char keyState[GLFW_KEY_LAST+1] = {0};
        unsigned char keyMods[GLFW_KEY_LAST+1] = {0};

        static Engine & instance();

        Transform cameraTranslation();
        Transform cameraRotation();
        Transform cameraTransform();
        Vector4 cameraForward();
        Vector4 cameraRight();

        void userTimerUpdate( double timeNow, double deltaTime );

        // UI init and launch
        void createWindow(int & argc, char ** argv );
        void start();

        void repaintViewport();

        // Callbacks
        void registerCallbacks();
        void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
        void cursorPositionCallback(GLFWwindow * window, double xpos, double ypos);
        void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
        void scrollCallback(GLFWwindow * window, double xoffset, double yoffset);
        void framebufferSizeCallback(GLFWwindow * window, int width, int height);

        // Static GLFW callbacks - delegate to instance callbacks
        static void sKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
        static void sCursorPositionCallback(GLFWwindow * window, double xpos, double ypos);
        static void sMouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
        static void sScrollCallback(GLFWwindow * window, double xoffset, double yoffset);
        static void sFramebufferSizeCallback(GLFWwindow * window, int width, int height);

    protected:
        void setViewport( int width, int height );
        void drawGameObjects( const Matrix4x4 & projection, const Matrix4x4 & view );

        Timer gameTimer;
        float gameTime = 0.0f;

        GLFWwindow * window = nullptr;

        int mouse_last_x = -1;
        int mouse_last_y = -1;
};

#endif

