/*
 *  Vector.h
 *  FastRender
 *
 *  Created by David Cunningham on 7/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <fstream>
#include <stdio.h>

template< typename T > inline T sq( const T & t ) { return t * t; }

//
// 3D Vector class for projective geometry. Has typical x, y, z coordinates, plus
// the w coordinate for homogenous transforms.
//
class Vector4 
{
    public:
        inline Vector4();
        inline Vector4( const Vector4 & a );
        inline Vector4( float x, float y, float z, float w = 1.0f );
        inline ~Vector4() {}

        inline float & operator[]( int i ) 
        { 
            return data[i]; 
        }

        inline const float & operator[]( int i ) const
        { 
            return data[i]; 
        }

        inline void set( float x, float y, float z, float w = 1.0f );

        float magnitude_sq();
        float magnitude();
        void normalize();
        void negate();

        void print() const;
        void fprintCSV( FILE * file ) const;

        union {
            float data[4];
            struct {
                float x, y, z, w;
            };
        };
};

inline void dot( const Vector4 & a, const Vector4 & b, float & r );
inline float dot( const Vector4 & a, const Vector4 & b );
inline void cross( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline void add( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline void subtract( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline void scale( const Vector4 & a, float s, Vector4 & r );
inline void perspective_scale( const Vector4 & a, Vector4 & r );
inline void mirror( const Vector4 & a, const Vector4 & n, Vector4 & r );
inline void interp( const Vector4 & a, const Vector4 & b, const float alpha, Vector4 & r);

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

#include "Vector4.hpp"
#include "Vector2.hpp"

#endif
