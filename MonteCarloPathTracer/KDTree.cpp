#pragma once
#include "iostream"
#include "string"
#include "vector"
#include "KDTree.h"
#include "Model.h"

KDTree::KDTree(vector<Triangle *> &triangles)
{
	buildTree(triangles);
}

void KDTree::buildTree(vector<Triangle *> &triangles)
{
	int size = (int)(triangles.size());

	splitAxis = getSplitAxis(triangles);

	if (size == 1)
	{
		isLeaf = true;
		node = triangles[0];
		left = nullptr;
		right = nullptr;
	}
	else
	{
		vector<Triangle *> leftVec;
		vector<Triangle *> rightVec;

		float minVal = FLT_MAX;
		float maxVal = -FLT_MAX;

		minVal = box.getMinCoord(splitAxis);
		maxVal = box.getMaxCoord(splitAxis);
		float pivot = (maxVal + minVal) / 2;

		Box bbox;
		for (int i = 0; i < triangles.size(); i++)
		{
			bbox = triangles[i]->getBox();
			float center = (bbox.getMaxCoord(splitAxis) + bbox.getMinCoord(splitAxis)) / 2;
			if (center < pivot)
				leftVec.push_back(triangles[i]);
			else
				rightVec.push_back(triangles[i]);
		}

		int index;
		if (leftVec.size() == triangles.size())
		{
			float maxCenter = -FLT_MAX;
			for (int i = 0; i < leftVec.size(); i++)
			{
				bbox = leftVec[i]->getBox();
				float center = (bbox.getMaxCoord(splitAxis) + bbox.getMinCoord(splitAxis)) / 2;
				if (center > maxCenter)
				{
					maxCenter = center;
					index = i;
				}
			}
			rightVec.push_back(leftVec[index]);
			leftVec.erase(leftVec.begin() + index);
		}
		else if (rightVec.size() == triangles.size())
		{
			float minCenter = FLT_MAX;
			for (int i = 0; i < rightVec.size(); i++)
			{
				bbox = rightVec[i]->getBox();
				float center = (bbox.getMaxCoord(splitAxis) + bbox.getMinCoord(splitAxis)) / 2;
				if (center < minCenter)
				{
					minCenter = center;
					index = i;
				}
			}
			leftVec.push_back(rightVec[index]);
			rightVec.erase(rightVec.begin() + index);
		}

		left = leftVec.size() > 0 ? new KDTree(leftVec) : nullptr;
		right = rightVec.size() > 0 ? new KDTree(rightVec) : nullptr;

	}
}

//cal box of this level & get split axis
AXIS KDTree::getSplitAxis(vector<Triangle*> &triangles)
{
	for (Triangle* &tri : triangles)
	{
		box = Box::merge(box, tri->getBox());
	}

	Vec3f diff = box.high - box.low;
	if (diff.x > diff.y && diff.x > diff.z)
		return X;
	else if (diff.y > diff.x && diff.y > diff.z)
		return Y;
	else
		return Z;
}
