#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <functional>

#include "Mesh.h"
#include "Transform.h"

class GameObject
{
    public:
        GameObject( const std::string & path,
                    bool normalizeScale = true,
                    float normScaleFactor = 1.0f );
        GameObject();
        ~GameObject();

        void draw();

        // Callback for updating animation state.
        //   Params: this, gameTime, deltaTime
        typedef std::function<void(GameObject *, float, float)> AnimationFunction;

        void setAnimationFunction( AnimationFunction func );
        void updateAnimation(float gameTime, float deltaTime);

        Mesh mesh;
        Transform worldTransform;
        Vector4 position;

        AnimationFunction animFunc;
};



#endif
