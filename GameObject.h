#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include "Mesh.h"
#include "Transform.h"

class GameObject
{
    public:
        GameObject( const std::string & path );
        GameObject();
        ~GameObject();

        void draw();

        Mesh mesh;
        Transform worldTransform;
};



#endif
