#ifndef __AXISALIGNEDSLAB_H__
#define __AXISALIGNEDSLAB_H__

#include "Traceable.h"

class AxisAlignedSlab : public Traceable
{
public:
    AxisAlignedSlab();
    AxisAlignedSlab( float xmin_default, float ymin_default, float zmin_default,
                     float xmax_default, float ymax_default, float zmax_default );
    virtual ~AxisAlignedSlab() {}
    
    void correctMinMax( void );
    
    virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;

    float xdim() const { return xmax - xmin; }
    float ydim() const { return ymax - ymin; }
    float zdim() const { return zmax - zmin; }

    float xmin, ymin, zmin;
    float xmax, ymax, zmax;

    static unsigned long intersection_test_count;       // Counts the number of intersection tests against
                                                        // objects of this class
    virtual void print( FILE * file = stdout ) const;
};

#endif
