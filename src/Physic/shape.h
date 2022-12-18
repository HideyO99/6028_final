#pragma once
#include "Vector3.h"

typedef Vector3 Point;

enum ShapreType
{
	TYPE_SPHERE,
	TYPE_AABB,
	TYPE_TRIANGLE
};

class Ray 
{
public:
	Ray(const Point& p, const Vector3& d)
		: origin(p), direction(d) {
		direction.Normalize();
	}
	Point origin;
	Vector3 direction;
};

class cShape 
{
public:
	virtual ShapreType getShapeType() const = 0;
};

class Sphere : public cShape
{
public:
	Sphere(const Point& center, float radius)
		: Center(center)
		, Radius(radius)
	{ }

	virtual ~Sphere()
	{ }

	virtual ShapreType getShapeType() const override
	{
		return TYPE_SPHERE;
	}

	Point Center;
	float Radius;
};

class AABB : public cShape
{
public:
	AABB(float min[3], float max[3])
	{
		memcpy(&(Min[0]), &(min[0]), 3 * sizeof(float));
		memcpy(&(Max[0]), &(max[0]), 3 * sizeof(float));
	}

	virtual ~AABB()
	{ }

	virtual ShapreType getShapeType() const override
	{
		return TYPE_AABB;
	}

	float Min[3];
	float Max[3];
};

class Triangle : public cShape
{
public:
	Triangle(Point a, Point b, Point c)
		: A(a), B(b), C(c) 
	{ }

	virtual ~Triangle()
	{ }

	virtual ShapreType getShapeType() const override
	{
		return TYPE_TRIANGLE;
	}

	Point A;
	Point B;
	Point C;
};
