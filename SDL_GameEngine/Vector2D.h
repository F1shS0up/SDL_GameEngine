#ifndef __VECTOR_2D_H__
#define __VECTOR_2D_H__
#include<math.h>
#include <algorithm>
#include <iostream> 
#include <ostream> 

class Vector2D
{
public:
	double x;
	double y;

	Vector2D() {};
	Vector2D(double x, double y) : x(x), y(y) {}
	double* getX() { return &x; }
	double* getY() { return &y; }

	void setX(double x) { x = x; }
	void setY(double y) { y = y; }

	double length() { return sqrt(x * x + y * y); }

	Vector2D operator+(const Vector2D& v2) const
	{
		return Vector2D(x + v2.x, y + v2.y);
	}

	bool operator== (const Vector2D& v2)
	{
		return (x == v2.x &&
			y == v2.y);
	}

	friend Vector2D& operator+=(Vector2D& v1, const Vector2D& v2)
	{
		v1.x += v2.x;
		v1.y += v2.y;
		return v1;
	}

	Vector2D operator*(double scalar)
	{
		return Vector2D(x * scalar, y * scalar);
	}

	Vector2D& operator*=(double scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2D operator-(const Vector2D& v2) const
	{
		return Vector2D(x - v2.x, y - v2.y);
	}
	friend Vector2D& operator-=(Vector2D& v1, const Vector2D& v2)
	{
		v1.x -= v2.x;
		v1.y -= v2.y;
		return v1;
	}

	Vector2D operator/(double scalar)
	{
		return Vector2D(x / scalar, y / scalar);
	}
	Vector2D& operator/=(double scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	Vector2D normalize()
	{
		double l = length();
		if (l > 0)
		{
			(*this) *= 1 / l;
		}
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector2D& v)
	{
		os << "X:" << v.x << " " << "Y:" << v.y;
		return os;
	}


	static Vector2D Clamp(const Vector2D& v, const Vector2D& min, const Vector2D& max)
	{
		return Vector2D(std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y));
	}
	static double DotProduct(Vector2D v1, Vector2D v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}
	static double CrossDotProduct(Vector2D v1, Vector2D v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}
	static double PerpDotProduct(Vector2D v1, Vector2D v2)
	{
		return (v1.y * v2.x) - (v1.x * v2.y);
	}
};

#endif