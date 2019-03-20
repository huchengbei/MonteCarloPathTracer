#pragma once
#include "iostream"
#include "string"
#include "vector"
#include "KDTree.h"
#include "Model.h"

KDTree::KDTree(vector<Face *> &faces)
{
	buildTree(faces);
}

void KDTree::buildTree(vector<Face *> &faces)
{
	int size = (int)(faces.size());
	splitAxis = getSplitAxis(faces);

	if (size == 1)
	{
		isLeaf = true;
		node = faces[0];
		left = nullptr;
		right = nullptr;
	}
	else
	{
		vector<Face *> leftVec;
		vector<Face *> rightVec;

		float minVal = FLT_MAX;
		float maxVal = -FLT_MAX;

		minVal = box.getMinCoord(splitAxis);
		maxVal = box.getMaxCoord(splitAxis);
		float pivot = (maxVal + minVal) / 2;

		Box bbox;
		for (int i = 0; i < faces.size(); i++)
		{
			bbox = faces[i]->getBox();
			float center = (bbox.getMaxCoord(splitAxis) + bbox.getMinCoord(splitAxis)) / 2;
			if (center < pivot)
				leftVec.push_back(faces[i]);
			else
				rightVec.push_back(faces[i]);
		}

		int index;
		if (leftVec.size() == faces.size())
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
		else if (rightVec.size() == faces.size())
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
AXIS KDTree::getSplitAxis(vector<Face*> &faces)
{
	for (Face* &face : faces)
	{
		box = Box::merge(box, face->getBox());
	}

	Vec3f diff = box.high - box.low;
	if (diff.x > diff.y && diff.x > diff.z)
		return X;
	else if (diff.y > diff.x && diff.y > diff.z)
		return Y;
	else
		return Z;
}
