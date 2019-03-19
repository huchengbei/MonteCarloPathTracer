#pragma once
#include "vector"
#include "MetaClass.h"
using namespace std;

class Light
{
public:
	enum TYPE{POLYGON, SPHERE};
	Point3f center;
	Vec3f normal;
	float area;
	float radius = 1;
	Vec3f Le;
	TYPE type;

	Vec3f dx{ 1, 0, 0 }, dy{0, 1, 0 }, dz{ 0, 0, 1 };

	Light(Point3f pos, Vec3f emission)
	{
		center = pos;
		Le = emission;
	}

	Light(Point3f pos, Vec3f dX, Vec3f dY, Vec3f emission)
	{
		center = pos;
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
		float fixRadius = radius * 1.414;
		float randX = (((float)rand() / RAND_MAX) * 2 - 1) * fixRadius;
		float randY = (((float)rand() / RAND_MAX) * 2 - 1) * fixRadius;
		float randZ = (((float)rand() / RAND_MAX) * 2 - 1) * fixRadius;
		return center + randX * dx + randY * dy+ randZ * dz;
	}

	vector<Point3f> getPoints(int n)
	{
		vector<Point3f> res;
		for (int i =0; i< n;i++)
			res.push_back(getRandomLightPoint());
		return res;

	}


};