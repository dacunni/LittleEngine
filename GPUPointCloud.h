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

class GPUPointCloud
{
    public:
        GPUPointCloud();
        ~GPUPointCloud(); 

        void upload( std::vector<Vector4> & points );
        bool uploaded();

        void bind();
        void draw();

        void setPointSize( float s );

        const GLuint POSITION_ATTRIB_INDEX = 0;
        const GLuint NORMAL_ATTRIB_INDEX   = 1;

        GLuint vao = 0;
        GLuint vbo = 0;
        unsigned long long num_vertices = 0;
        float point_size = 1;
};


#endif
