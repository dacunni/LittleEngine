/*
 *  Vector2.h
 *  FastRender
 *
 *  Created by David Cunningham on 7/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#include <fstream>
#include <stdio.h>

//
// 2D Vector class
//
class Vector2 
{
    public:
        inline Vector2();
        inline Vector2( const Vector2 & a );
        inline Vector2( float x, float y );
        inline ~Vector2() {}

        inline float & operator[]( int i ) 
        { 
            return data[i]; 
        }

        inline const float & operator[]( int i ) const
        { 
            return data[i]; 
        }

        inline void set( float x, float y );

        union {
            float data[2];
            struct {
                float x, y;
            };
        };
};

inline void dot( const Vector2 & a, const Vector2 & b, float & r );
inline void add( const Vector2 & a, const Vector2 & b, Vector2 & r );
inline void subtract( const Vector2 & a, const Vector2 & b, Vector2 & r );
inline void scale( const Vector2 & a, float s, Vector2 & r );
inline void interp( const Vector2 & a, const Vector2 & b, const float alpha, Vector2 & r);

#include "Vector2.hpp"

#endif
