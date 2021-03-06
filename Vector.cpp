#include <math.h>
#include <stdio.h>

#include "Vector.h"

float Vector4::magnitude_sq()
{
	return sq(data[0]) + sq(data[1]) + sq(data[2]);
}

float Vector4::magnitude()
{
	return sqrt( sq(data[0]) + sq(data[1]) + sq(data[2]));
}

void Vector4::normalize()
{
	float mag = magnitude();
	
	if( mag != 0.0) {
		scale( *this, 1.0f / mag, *this);
	}
}

Vector4 Vector4::normalized() const
{
    Vector4 u( *this);
    u.normalize();
    return u;
}

void Vector4::negate()
{
    data[0] = -data[0];
    data[1] = -data[1];
    data[2] = -data[2];
}

Vector4 Vector4::negated() const
{
    return Vector4( -data[0], -data[1], -data[2], data[3]);
}


void Vector4::print() const
{
	printf( "V4( %f %f %f %f)\n", data[0], data[1], data[2], data[3]);
}

void Vector4::fprintCSV( FILE * file) const
{
	fprintf( file, "%f,%f,%f,%f\n", data[0], data[1], data[2], data[3]);
}



