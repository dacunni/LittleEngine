#ifndef __VECTOR2_HPP__
#define __VECTOR2_HPP__

inline Vector2::Vector2()
    : x(0.0f), y(0.0f)
{

}

inline Vector2::Vector2( const Vector2 & a )
{
	data[0] = a.data[0];
	data[1] = a.data[1];
}

inline Vector2::Vector2( float xn, float yn )
{
	data[0] = xn;
	data[1] = yn;
}

inline void Vector2::set( float xn, float yn )
{
	data[0] = xn;
	data[1] = yn;
}

inline void add( const Vector2 & a, const Vector2 & b, Vector2 & r )
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
}

inline void subtract( const Vector2 & a, const Vector2 & b, Vector2 & r )
{
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
}

inline void scale( const Vector2 & a, float s, Vector2 & r )
{
	r[0] = a[0] * s;
	r[1] = a[1] * s;
}

// Linearly interpolate two vectors using parameter alpha, such that alpha=0 gives a and
// alpha=1 gives b
inline void interp( const Vector2 & a, const Vector2 & b, const float alpha, Vector2 & r)
{
    float oma = 1.0f - alpha;
    r.x = oma * a.x + alpha * b.x;
    r.y = oma * a.y + alpha * b.y;
}

#endif
