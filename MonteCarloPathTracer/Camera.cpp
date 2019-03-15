#include "MetaClass.h"
#include "Ray.cpp"

class Camera
{
public:
	Point3f position;

	float fov;
	Vec3f front, up, right;
	Vec3f view_x, view_y, view_z;

	Camera(){}

	Camera(Point3f pos, Point3f center, Vec3f up)
	{
		position = pos;
		front = normalize(center - pos);
		right = normalize(cross(front, up));
		this->up = normalize(cross(right, front));
	}

	void setViewPort(float _fov, float ratio)
	{
		fov = _fov;
		view_x = right * 2 * tan(fov * PI / 360);
		view_y = up * 2 * tan(fov * ratio * PI / 360);
		view_z = front;
	}

	Ray getRay(float x, float y)
	{
		Vec3f direction = view_z + (x - 0.5f) * view_x + (y - 0.5f) * view_y;

		return Ray(position, direction);
	}

};