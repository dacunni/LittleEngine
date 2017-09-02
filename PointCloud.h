#ifndef __POINT_CLOUD_H_
#define __POINT_CLOUD_H_

#include <vector>
#include "Vector.h"
#include "OpenGLUtil.h"
#include "Renderable.h"

class PointCloud : public Renderable
{
    public:
        PointCloud();
        ~PointCloud(); 

        void upload( std::vector<Vector4> & points );

        virtual void bind();
        virtual void draw();

};


#endif
