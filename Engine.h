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

        int anim_timeout_millis = 33;
        float anim_rotation = 0.0f;
        float anim_time = 0.0f;

        bool draw_wireframes = false;

        Vector4 cameraPosition = Vector4( 0.0, 0.6, 0.0 );
        float cameraXRotation = 0.0;
        float cameraYRotation = 0.0;
        float cameraSpeed = 5.0;
        float cameraKeyboardRotationSpeed = 2.0;

        float heroSpeed =  5.0;

        int window_width = 640;
        int window_height = 480;

        unsigned char keyState[GLFW_KEY_LAST+1] = {0};

#ifdef USE_GLFW
#else
        int mouse_button_state[3] = { 
            GLUT_UP, // left
            GLUT_UP, // right
            GLUT_UP  // middle
        };
#endif
        int mouse_last_x = -1;
        int mouse_last_y = -1;

        static Engine * _instance;

        static Engine & instance() {
            if(!_instance) { _instance = new Engine(); }
            return *_instance;
        }

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
        void keyPressed( unsigned char key, int x, int y );
        void keyReleased( unsigned char key, int x, int y );
        void mouseButton( int button, int state, int x, int y );
        void mouseMotionWhileButtonPressed( int x, int y );
        void viewportReshaped( int width, int height );

        void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
        void cursorPositionCallback(GLFWwindow * window, double xpos, double ypos);
        void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);

        // Static GLFW callbacks - delegate to instance callbacks
        static void sKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
        static void sCursorPositionCallback(GLFWwindow * window, double xpos, double ypos);
        static void sMouseButtonCallback(GLFWwindow * window, int button, int action, int mods);

    protected:
        void setViewport( int width, int height );
        void drawGameObjects( const Matrix4x4 & projection, const Matrix4x4 & view );

        Timer gameTimer;

        GLFWwindow * window = nullptr;
};

#endif

