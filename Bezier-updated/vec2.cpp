#include "vec2.h"

Vec2::Vec2(GLfloat x_in, GLfloat y_in) : x{x_in}, y{y_in} { }

const Vec2 &Vec2::operator =(const Vec2 &v)
{
    x = v.x;
    y = v.y;
    return *this;
}

Vec2 Vec2::operator +(const Vec2 &v) const
{
    Vec2 u;
    u.x = x + v.x;
    u.y = y + v.y;
    return u;
}

Vec2 Vec2::operator -(const Vec2 &v) const
{
    Vec2 u;
    u.x = x - v.x;
    u.y = y - v.y;
    return u;
}

Vec2 Vec2::operator ^(const Vec2 &v) const
{
    /*Vec2 w;
    w.x =  y*v.z - z*v.y;
    w.y =  z*v.x - x*v.z;
    return w;*/
}

GLfloat Vec2::getX() const
{
    return x;
}

void Vec2::setX(const GLfloat &value)
{
    x = value;
}

GLfloat Vec2::getY() const
{
    return y;
}

void Vec2::setY(const GLfloat &value)
{
    y = value;
}

/* Vec2::barycentricCoordinates(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3)
{
    Vec3 p12 = p2 - p1;
    Vec3 p13 = p3 - p1;

    Vec3 n = p12 ^ p13;

    std::cout << "N " << n << "N.length " << n.length() << std::endl;

    float areal_123 = n.length();

    Vec3 baryc;

    // u
    Vec3 p = p2 - *this;
    Vec3 q = p3 - *this;
    n = q ^ p;
    baryc.setX(n.getZ()/areal_123);

    // v
    p = Vec3(p3.getX(), p3.getY(), 0.f) - Vec3(x, y, 0.f);
    q = Vec3(p1.getX(), p1.getY(), 0.f) - Vec3(x, y, 0.f);
    n = q ^ p;
    baryc.setY(n.getZ()/areal_123);

    // w
    p = Vec3(p1.getX(), p1.getY(), 0.f) - Vec3(x, y, 0.f);
    q = Vec3(p2.getX(), p2.getY(), 0.f) - Vec3(x, y, 0.f);
    n = q ^ p;
    baryc.setZ(n.getZ()/areal_123);

    return baryc;
}*/
