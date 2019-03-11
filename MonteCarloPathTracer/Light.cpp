#pragma once
#include "MetaClass.cpp"

class Light
{
public:
	Point3f position;
	Vec3f normal;
	float area;
	Vec3f Le;

	Vec3f dx{ 1, 0, 0 }, dy{0, 1, 0 }, dz{ 0, 0, 1 };

	Light(Point3f pos, Vec3f emission)
	{
		position = pos;
		Le = emission;
	}

	Light(Point3f pos, Vec3f dX, Vec3f dY, Vec3f emission)
	{
		position = pos;
		Le = emission;
		dx = dX;
		dy = dY;
	}
};