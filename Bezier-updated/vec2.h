#ifndef VEC2_H
#define VEC2_H

#include <QtGui/qopengl.h> // for GLfloat
#include <cmath>
#include "matrix4x4.h"
#include "vec3.h"

class Vec2
{
public:
    Vec2(GLfloat x_in=0.0, GLfloat y_in=0.0);

    const Vec2 &operator = (const Vec2& v);

    Vec2 operator + (const Vec2& v) const;
    Vec2 operator - (const Vec2& v) const;

    Vec2 operator ^ (const Vec2 &v) const;

    GLfloat getX() const;
    void setX(const GLfloat &value);

    GLfloat getY() const;
    void setY(const GLfloat &value);

    //Vec3 barycentricCoordinates(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3);

private:
    GLfloat x;
    GLfloat y;
};

#endif // VEC2_H
