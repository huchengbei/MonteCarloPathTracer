#pragma once
#include "Model.h"
#include "Ray.cpp"
#include "Material.cpp"
#include "functions.h"

class PathTracer
{
public:
	enum REFLECT_TYPE{SPECULAR_REFLECT, DIFFUSE_REFLECT};

	int iter_cnt = 0;
	int px_sample_num = 1;
	int light_sample_num = 3;
	int max_path_depth = 5;
	int maxRenderDepth = 100;

	Vec3f BREFImportanceSample(Vec3f &direction, float Ns, REFLECT_TYPE type);

	Ray monteCarloSample(Ray &ray, Point3f &point, Material* &material, Vec3f &normal);

	Color3f trace(Model &model, Ray &ray, int depth = 0);

	vector<float> render(Model& model);

	vector<float> getColors(int w, int h);

};