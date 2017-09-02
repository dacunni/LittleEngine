#ifndef _GPU_POINT_CLOUD_H_
#define _GPU_POINT_CLOUD_H_

#include <vector>
#include "Vector.h"
#include "OpenGLUtil.h"
#include "GPURenderable.h"

class GPUPointCloud : public GPURenderable
{
    public:
        GPUPointCloud();
        ~GPUPointCloud(); 

        void upload( std::vector<Vector4> & points );

        virtual void bind();
        virtual void draw();

};


#endif
