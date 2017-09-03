#ifndef _TRACEABLE_H_
#define _TRACEABLE_H_

#include <stdio.h>
#include <memory>

#include "Transform.h"

class Material;
class Ray; 
class RayIntersection;

class Traceable {
public:
	Traceable();
    virtual ~Traceable();
	
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const = 0;
	virtual bool intersectsAny( const Ray & ray, float min_distance ) const;

    virtual void print( FILE * file = stdout ) const;

    // FIXME - we probably don't want all traceables to get a material, or we'll just end up wasting memory
    Material * material;
};



#endif

