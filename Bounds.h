#ifndef __BOUNDS_H_
#define __BOUNDS_H_

class Bounds
{
    const float InitialMin = +std::numeric_limits<float>::max();
    const float InitialMax = -std::numeric_limits<float>::max();

    public:
        Bounds() = default;
        ~Bounds() = default;

        inline void add(float x, float y, float z);

        float minx() const { return min[0]; }
        float miny() const { return min[1]; }
        float minz() const { return min[2]; }
        float maxx() const { return max[0]; }
        float maxy() const { return max[1]; }
        float maxz() const { return max[2]; }

        inline float maxdim() const;

    protected:
        float min[3] = { InitialMin, InitialMin, InitialMin };
        float max[3] = { InitialMax, InitialMax, InitialMax };
};

// Implementations

inline void Bounds::add(float x, float y, float z)
{
    min[0] = std::min(x, min[0]);
    min[1] = std::min(y, min[1]);
    min[2] = std::min(z, min[2]);
    max[0] = std::max(x, max[0]);
    max[1] = std::max(y, max[1]);
    max[2] = std::max(z, max[2]);
}

inline float Bounds::maxdim() const
{
    return std::max(max[0] - min[0],
           std::max(max[1] - min[1],
                    max[2] - min[2]));
}

#endif

