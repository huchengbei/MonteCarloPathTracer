#pragma once
#include "iostream"
#include "string"
#include "vector"
#include "Model.h"
using namespace std;

class Face;
enum AXIS { X, Y, Z };
class KDTree
{
public:

	Box box;
	AXIS splitAxis;
	KDTree* left;
	KDTree* right;

	Face* node;
	bool isLeaf = false;

	KDTree() {}

	KDTree(vector<Face *> &triangles);

	void buildTree(vector<Face *> &triangles);

	AXIS getSplitAxis(vector<Face*> &triangles);
};
