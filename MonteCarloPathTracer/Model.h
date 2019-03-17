#pragma once
#include "vector"
#include "string"
#include "map"

#include "Box.h"
#include "Light.cpp"
#include "Material.cpp"
#include "MetaClass.h"

using namespace std;

class Triangle;

class Model
{
public:
	vector<Vec3f> vertexs;
	vector<Vec3f> normals;	
	vector<Light> lights;
	map<string, Material> materialTable;

	Box box;
	vector<Triangle*> triangles;

	Model() {}
	Model(string path);

	bool LoadMaterial(string path);

	bool Load(string path);
};

class Triangle
{
public:
	Vec3f  vertex[3];
	Vec3f  vertexNormal[3];

	Point3f point;
	Vec3f edge1, edge2;

	Model* model;
	Material* material;
	Box box;

	Vec3f normal;

	Mat4 barycentric;

	Triangle(Model* m, int v[3], int n[3]);

	void setBox();

	Box getBox();

	Vec3f getIntersectioNormal(Vec3f &point);
};
