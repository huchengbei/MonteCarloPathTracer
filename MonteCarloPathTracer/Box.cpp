#include "Box.h"
#include "algorithm"
#include "MetaClass.h"

using namespace std;

Box::Box(Point3f l, Point3f h)
{
	low = l;
	high = h;
}

Box Box::merge(const Box& box1, const Box& box2)
{
	Point3f l, h;
	l.x = min(box1.low.x, box2.low.x);
	l.y = min(box1.low.y, box2.low.y);
	l.z = min(box1.low.z, box2.low.z);

	h.x = max(box1.high.x, box2.high.x);
	h.y = max(box1.high.y, box2.high.y);
	h.z = max(box1.high.z, box2.high.z);

	return Box(l, h);
}

float Box::getMinCoord(int axis)
{
	return low.num[axis];
}

float Box::getMaxCoord(int axis)
{
	return high.num[axis];
}

void Box::changeBoard(Point3f & point)
{
	low.x = min(low.x, point.x);
	low.y = min(low.y, point.y);
	low.z = min(low.z, point.z);

	high.x = max(high.x, point.x);
	high.y = max(high.y, point.y);
	high.z = max(high.z, point.z);
}