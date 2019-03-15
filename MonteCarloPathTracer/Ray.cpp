#pragma once
#include "MetaClass.h"

class Ray {
public:
	enum SOURCE{NONE, DIRECT, DIFFUSE_REFLECT, SPECULAR_REFLECT, TRANSMISSION};

	Point3f position;
	Point3f direction;
	Vec3f inverseDirection;
	SOURCE source;
	float tmin, tmax;

	Ray(){}

	Ray(Point3f pos, Vec3f dir)
	{
		position = pos;
		direction = normalize(dir);
		source = SOURCE::NONE;
		inverseDirection = 1.0 / direction;
		tmin = EPS;
		tmax = FLT_MAX;
	}

	Ray(Point3f pos, Vec3f dir, SOURCE s)
	{
		position = pos;
		direction = dir;
		source = s;
		inverseDirection = 1.0 / direction;
		tmin = EPS;
		tmax = FLT_MAX;
	}

	bool reflect(const Vec3f &normal, Vec3f &reflectDirection)
	{
		reflectDirection = direction - 2.0f * normal * dot(normal, direction);
		return true;
	}

	bool refract(const Vec3f &normal, float nit, Vec3f &refractDirection)
	{
		float cosIn = dot(normal, direction);
		float cosT = 1.0f - nit * nit * (1.0f - cosIn * cosIn);
		if (cosT >= 0.0f)
		{
			refractDirection = nit * direction - normal * (nit * cosIn + sqrt(cosT));
			return true;
		}
		return false;
	}


};