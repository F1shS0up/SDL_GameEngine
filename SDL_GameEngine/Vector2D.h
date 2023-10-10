#ifndef __VECTOR_2D_H__
#define __VECTOR_2D_H__
#include<math.h>

class Vector2D
{
public:
    float x;
    float y;

    Vector2D() {};
    Vector2D(float x, float y) : x(x), y(y) {}
    float* getX() { return &x; }
    float* getY() { return &y; }

    void setX(float x) { x = x; }
    void setY(float y) { y = y; }

    float length() { return sqrt(x * x + y * y); }

    Vector2D operator+(const Vector2D& v2) const
    {
        return Vector2D(x + v2.x, y + v2.y);
    }

    friend Vector2D& operator+=(Vector2D& v1, const Vector2D& v2)
    {
        v1.x += v2.x;
        v1.y += v2.y;
        return v1;
    }

    Vector2D operator*(float scalar)
    {
        return Vector2D(x * scalar, y * scalar);
    }

    Vector2D& operator*=(float scalar)
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

    Vector2D operator/(float scalar)
    {
        return Vector2D(x / scalar, y / scalar);
    }
    Vector2D& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    Vector2D normalize()
    {
        float l = length();
        if (l > 0)
        {
            (*this) *= 1 / l;
        }
        return *this;
    }
};

#endif