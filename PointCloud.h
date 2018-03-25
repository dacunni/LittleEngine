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

        virtual void upload();

        virtual void bind();
        virtual void draw();

        std::vector< Vector4 >  vertices;
};


#endif
