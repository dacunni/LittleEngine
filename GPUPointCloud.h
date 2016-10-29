/*
 *  GPUPointCloud.h
 *  FastRender
 *
 *  Created by David Cunningham on 4/11/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
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
