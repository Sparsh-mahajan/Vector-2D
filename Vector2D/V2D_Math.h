#pragma once

#include<cmath>
#include<algorithm>
#include<cassert>

typedef char int8;
typedef short int16;
typedef int int32;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef float real;
typedef double real64;

const real PI = 3.141592741f;
const real EPSILON = 0.0001f;

struct vec2
{
	union
	{
		real v[2];
		struct
		{
			real x, y;
		};
	};
	vec2() {}
	vec2(real x_ , real y_) : x{ x_ }, y{ y_ }
	{
		//constructor : do something
	}

	void Set(real x_, real y_)
	{
		x = x_;
		y = y_;
	}
	vec2 operator- () const
	{
		return vec2(-x, -y);
	}

	vec2 operator* (real s) const
	{
		return vec2(x * s, y * s);
	}
	void operator*= (real s)
	{
		x *= s;
		y *= s;
	}

	vec2 operator/ (real s) const
	{
		return vec2(x / s, y / s);
	}
	void operator/= (real s) // not in the original code
	{
		x /= s;
		y /= s;
	}

	vec2 operator+ (const vec2& rhs) const
	{
		return vec2(x + rhs.x, y + rhs.y);
	}
	vec2 operator+ (real s) const
	{
		return vec2(x + s, y + s);
	}
	void operator+= (const vec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}
	void operator+= (real s)  // not in the original code
	{
		x += s;
		y += s;
	}

	vec2 operator- (const vec2& rhs) const 
	{
		return vec2(x - rhs.x, y - rhs.y);
	}
	vec2 operator- (real s) const // not in the original code
	{
		return vec2(x - s, y - s);
	}
	void operator-= (const vec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
	}
	void operator-= (real s) // not in the original code
	{
		x -= s;
		y -= s;
	}

	real LenSqr() const
	{
		return (x * x + y * y);
	}
	real Len() const
	{
		return sqrt(x * x + y * y);
	}
	void Normalize()
	{
		real len = Len();
		if (len > EPSILON)
		{
			real invLen = 1.0f / len;
			x *= invLen;
			y *= invLen;
		}
	}
	void Rotate(real radians)
	{
		real c = std::cos(radians);
		real s = std::sin(radians);

		real xNew = x * c - y * s;
		real yNew = x * s + y * c;

		x = xNew;
		y = yNew;
	}
};

struct mat22
{
	union 
	{
		struct
		{
			real m00, m01;
			real m10, m11;
		};
		real m[2][2];
		real v[4];
	};
	mat22() {}
	mat22(real radians)  //constructor to initialize the object as a rotation matrix.
	{
		real s = std::sin(radians);
		real c = std::cos(radians);

		m00 = c; m01 = -s; 
		m10 = s; m11 = c;
	}
	mat22(real a, real b, real c, real d) : // constructor to initialize the object as a matrix containing 4 floats(real).
		m00{ a }, m01{ b },
		m10{ c }, m11{ d }
	{
		// do something
	}
	void set(real radians)
	{
		real s = std::sin(radians);
		real c = std::cos(radians);

		m00 = c; m01 = -s;
		m10 = s; m11 = c;
	}

	mat22 Abs() const
	{
		return mat22(std::abs(m00), std::abs(m01), std::abs(m10), std::abs(m11));
	}
	mat22 Transpose() const
	{
		return mat22(m00, m10, m01, m11);
	}
	const vec2 operator* (const vec2& rhs) const
	{
		return vec2(m00 * rhs.x + m01 * rhs.y, m10 * rhs.x + m11 * rhs.y);
	}
	const mat22 operator* (const mat22& rhs) const
	{
		return mat22(
			m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0],
			m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1],
			m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0],
			m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1]
		);
	}
};

inline vec2 Min(const vec2& a, const vec2& b) 
{
	return vec2(std::min(a.x, b.x), std::min(a.y, b.y));
}
inline vec2 Max(const vec2& a, const vec2& b)
{
	return vec2(std::max(a.x, b.x), std::max(a.y, b.y));
}
inline real Dot(const vec2& a, const vec2& b)
{
	return (a.x * b.x + a.y * b.y);
}
inline real Cross(const vec2& a, const vec2& b)
{
	return (a.x * b.y - b.x * a.y);
}
inline real DistSqr( const vec2& a, const vec2& b)
{
	vec2 c = a - b;
	return Dot(c, c);
}
inline bool Equal(real a, real b)
{
	return std::abs(a - b) <= EPSILON; //we use Epsilon (least possible 32 bit float) to counter floating point math
}									   // i.e. 0.2 + 0.1 would not always return 0.3 and thats why we are using approximation based									 //comparison
inline real Square(real a)
{
	return (a * a);
}
inline real Clamp(real min, real max, real a)
{
	if (a < min) return min;
	if (a > max) return max;
	return a;
}
inline int32 Round(real a)
{
	return (int32)(a + 0.5f);
}
inline real Random(real upper_lim,  real lower_lim)
{
	real a = (real)rand();
	a /= RAND_MAX;
	a = (upper_lim - lower_lim) * a + lower_lim;
	return a;
}
inline bool BiasGreaterThan(real a, real b) //function that is realted to collission detection.. needs work!
{
	const real relative = 0.95f;
	const real absolute = 0.01f;
	if ( relative * a > absolute + b)
	{
		return true;
	}
	return false;
}

const real Gravity_Scale = 5.0f;
const vec2 Gravity(0.0f, 10.0f * Gravity_Scale);
const real dt = 1.0f / 60.0f; // interval for very small change in time ( about 16 milliseconds )

//end of V2D math.