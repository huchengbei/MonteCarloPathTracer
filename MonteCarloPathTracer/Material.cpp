#pragma once
#include "MetaClass.h"
#include "string"

class Material
{
public:
	std::string name;
	int illum;

	Vec3f Ka;
	Vec3f Kd;
	Vec3f Ks;
	Vec3f Le;
	Vec3f Tf;
	float Ns = 60.0f;
	float Ni = 1.0f;
};

