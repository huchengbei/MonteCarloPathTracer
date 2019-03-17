#pragma once
#pragma once
#include "iostream"
#include "string"
#include "vector"
#include "Model.h"

using namespace std;
class Triangle;

enum AXIS { X, Y, Z };
class KDTree
{
public:

	Box box;
	AXIS splitAxis;
	KDTree* left;
	KDTree* right;

	Triangle* node;
	bool isLeaf = false;

	KDTree() {}

	KDTree(vector<Triangle *> &triangles);

	void buildTree(vector<Triangle *> &triangles);

	AXIS getSplitAxis(vector<Triangle*> &triangles);
};
