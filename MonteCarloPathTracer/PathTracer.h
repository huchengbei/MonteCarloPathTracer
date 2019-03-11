#pragma once
#include "Scene.cpp"
#include "Ray.cpp"
#include "Material.cpp"
#include "functions.h"

class PathTracer
{
public:
	int iter_cnt = 0;
	int px_sample_num = 1;
	int light_sample_num = 3;
	int max_path_depth = 5;

	Vec3f BREFImportanceSample(Vec3f &direction, float Ns);

	Ray monteCarloSample(Ray &ray, Point3f &point, Material &material, Vec3f &normal);

	Color3f trace(Scene &scene, Ray &ray, int depth = 0);

	vector<float> render(Scene& scene);

	vector<float> getColors(int w, int h);

};