#pragma once
#include "iostream"
#include "vector"
#include "Ray.cpp"
#include "Light.cpp"
#include "Model.h"
#include "KDTree.cpp"
#include "Camera.cpp"

using namespace std;

class Scene
{
public:
	int width, height;
	Color3f ambient;
	Camera camera;
	Model* model;
	KDTree* kdTree = new KDTree();
	vector<float> colors;
	vector<Light> lights;

	Scene() {};

	Scene(int w, int h)
	{
		width = w;
		height = h;
	}

	void init()
	{
		colors.clear();
		colors.resize(3 * width * height);
		ambient = Color3f(0.2, 0.2, 0.2);
		kdTree->buildTree(model->triangles);
	}

	vector<Ray> getRays(int x, int y, int px_sample_num)
	{
		vector<Ray> rays;
		rays.resize(px_sample_num);

		for (Ray &ray : rays)
		{
			float randX = (float)rand() / RAND_MAX;
			float randY = (float)rand() / RAND_MAX;
			ray = camera.getRay((x + randX) / width, (y + randY) / height);
		}

		return rays;
	}


};
