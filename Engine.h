#ifndef _ENGINE_H_
#define _ENGINE_H_

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

        double start_time = -1.0;
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

        int window_width = 1200;
        int window_height = 650;

        unsigned char keyState[256] = {0};

        int mouse_button_state[3] = { 
            GLUT_UP, // left
            GLUT_UP, // right
            GLUT_UP  // middle
        };
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

        // Callbacks
        void registerCallbacks();
        void keyPressed( unsigned char key, int x, int y );
        void keyReleased( unsigned char key, int x, int y );
        void mouseButton( int button, int state, int x, int y );
        void mouseMotionWhileButtonPressed( int x, int y );
        void viewportReshaped( int width, int height );
        void animTimerCallback( int value );
        void repaintViewport();

        // Static callbacks - delegate to instance callbacks
        static void sKeyPressed( unsigned char key, int x, int y );
        static void sKeyReleased( unsigned char key, int x, int y );
        static void sMouseButton( int button, int state, int x, int y );
        static void sMouseMotionWhileButtonPressed( int x, int y );
        static void sViewportReshaped( int width, int height );
        static void sRepaintViewport(void);
        static void sAnimTimerCallback( int value );

    protected:
        void drawGameObjects( const Matrix4x4 & projection, const Matrix4x4 & view );
};

#endif

