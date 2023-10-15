#ifndef Vector2DInt_H
#define Vector2DInt_H

#include<math.h>

class Vector2DInt
{
public:
	int x;
	int y;

	Vector2DInt() {};
	Vector2DInt(int x, int y) : x(x), y(y) {}
	int* getX() { return &x; }
	int* getY() { return &y; }

	void setX(int x) { x = x; }
	void setY(int y) { y = y; }

	int length() { return sqrt(x * x + y * y); }

	Vector2DInt operator+(const Vector2DInt& v2) const
	{
		return Vector2DInt(x + v2.x, y + v2.y);
	}

	friend Vector2DInt& operator+=(Vector2DInt& v1, const Vector2DInt& v2)
	{
		v1.x += v2.x;
		v1.y += v2.y;
		return v1;
	}

	Vector2DInt operator*(int scalar)
	{
		return Vector2DInt(x * scalar, y * scalar);
	}

	Vector2DInt& operator*=(int scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2DInt operator-(const Vector2DInt& v2) const
	{
		return Vector2DInt(x - v2.x, y - v2.y);
	}
	friend Vector2DInt& operator-=(Vector2DInt& v1, const Vector2DInt& v2)
	{
		v1.x -= v2.x;
		v1.y -= v2.y;
		return v1;
	}

	Vector2DInt operator/(int scalar)
	{
		return Vector2DInt(x / scalar, y / scalar);
	}
	Vector2DInt& operator/=(int scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	void normalize()
	{
		int l = length();
		if (l > 0)
		{
			(*this) *= 1 / l;
		}
	}
};

#endif
