#pragma once
#include "vector"
#include "MetaClass.h"
using namespace std;

class Light
{
public:
	enum TYPE{POLYGON, SPHERE, TRIANGLE};
	union{
		Point3f center;
		Point3f postion;
	};
	Vec3f normal;
	float area;
	float radius = 1;
	Vec3f Le;
	TYPE type;

	Vec3f dx{ 1, 0, 0 }, dy{0, 1, 0 }, dz{ 0, 0, 1 };

	// use by triangle
	Light(Point3f pos, Vec3f dX, Vec3f dY, Vec3f emission)
	{
		type = TRIANGLE;
		postion = pos;
		Le = emission;
		dx = dX;
		dy = dY;

		normal = cross(dx, dy);
		area = length(normal);
		normal = normalize(normal);
	}

	Light(TYPE t, Point3f c, float r, Vec3f emission, float a = 0.0f, Vec3f n = Vec3f())
	{
		type = t;
		center = c;
		radius = r;
		Le = emission;
		switch (t)
		{
		case TYPE::POLYGON:
			area = a;
			normal = n;
			dx = normalize(normal.getOneVerticalVec());
			dy = normalize(cross(normal, dx));
			dz = Vec3f();
			break;
		case TYPE::SPHERE:
			area = PI * radius * radius;
			break;
		default:
			break;
		}
	}

	Point3f getRandomLightPoint()
	{
		if (type == TYPE::TRIANGLE)
		{
			float randX = (float)rand() / RAND_MAX;
			float randY = (float)rand() / RAND_MAX;
			return postion + randX * dx + randY * dy;
		}

		float fixRadius = radius * 1.414f;
		float randX = (((float)rand() / RAND_MAX) * 2 - 1);
		float randY = (((float)rand() / RAND_MAX) * 2 - 1);
		float randZ = (((float)rand() / RAND_MAX) * 2 - 1);
		return center + (randX * dx + randY * dy+ randZ * dz) * fixRadius;
	}
};