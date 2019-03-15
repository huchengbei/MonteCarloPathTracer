#pragma once
#include "algorithm"
#include "MetaClass.h"

class Box {
public:
	Point3f low, high;

	Box() {};
	Box(Point3f l, Point3f h);

	static Box merge(const Box& box1, const Box& box2);

	float getMinCoord(int axis);

	float getMaxCoord(int axis);

	void changeBoard(Point3f & point);
};
