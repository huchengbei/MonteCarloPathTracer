#pragma once
#include "functions.h"

bool intersect(Box& box, Ray& ray)
{
	Vec3f &invDirection = ray.inverseDirection;

	float t1 = (box.low.x - ray.position.x) * invDirection.x;
	float t2 = (box.high.x - ray.position.x) * invDirection.x;

	float t3 = (box.low.y - ray.position.y) * invDirection.y;
	float t4 = (box.high.y - ray.position.y) * invDirection.y;

	float t5 = (box.low.z - ray.position.z) * invDirection.z;
	float t6 = (box.high.z - ray.position.z) * invDirection.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	return !(tmax < 0 || tmin > tmax || tmin >= ray.tmax || tmax <= ray.tmin);
}

bool intersect(Triangle *trangle, Ray& ray, Point3f& point, Material* &material, Vec3f& normal)
{
	if (abs(dot(trangle->normal, ray.direction)) < EPS)
		return false;

	Vec3f v = trangle->point - ray.position;

	Vec3f tmp = cross(trangle->edge2, ray.direction);
	float beta = -dot(tmp, v) / dot(tmp, trangle->edge1);

	tmp = cross(trangle->edge1, ray.direction);
	float gamma = -dot(tmp, v) / dot(tmp, trangle->edge2);

	float t = dot(trangle->normal, v) / dot(trangle->normal, ray.direction);

	if (beta > 0 && gamma > 0 && beta + gamma < 1 && t <= ray.tmax && t >= ray.tmin)
	{
		point = ray.position + t * ray.direction;
		material = trangle->material;
		normal = trangle->getIntersectioNormal(point);

		// may change
		ray.tmax = t;
		return true;
	}

	return false;
}

bool intersect(Face * face, Ray & ray, Point3f & point, Material *& material, Vec3f & normal)
{
	bool ok = false;
	for (Triangle* &tri : face->triangles)
	{
		ok = intersect(tri, ray, point, material, normal);
		if (ok)
			return ok;
	}
	return false;
}

bool intersect(KDTree* kdTree, Ray& ray, Point3f& point, Material* &material, Vec3f& normal, JudgeOrCal jOC)
{
	if (!intersect(kdTree->box, ray))
		return false;

	bool hit = false;

	if (kdTree->isLeaf)
		return intersect(kdTree->node, ray, point, material, normal);

	if (ray.direction.num[kdTree->splitAxis] >= 0)
	{
		switch (jOC)
		{
		case JUDGE:
			if (kdTree->right != nullptr)
				hit |= intersect(kdTree->right, ray, point, material, normal, jOC);
			if (!hit && kdTree->left != nullptr)
				hit |= intersect(kdTree->left, ray, point, material, normal, jOC);
			break;
		case CAL:
			if (kdTree->left != nullptr)
				hit |= intersect(kdTree->left, ray, point, material, normal);
			if (kdTree->right != nullptr)
				hit |= intersect(kdTree->right, ray, point, material, normal);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (jOC)
		{
		case JUDGE:
			if (kdTree->left != nullptr)
				hit |= intersect(kdTree->left, ray, point, material, normal, jOC);
			if (!hit && !hit && kdTree->right != nullptr)
				hit |= intersect(kdTree->right, ray, point, material, normal, jOC);
			break;
		case CAL:
			if (kdTree->right != nullptr)
				hit |= intersect(kdTree->right, ray, point, material, normal);
			if (kdTree->left != nullptr)
				hit |= intersect(kdTree->left, ray, point, material, normal);
			break;
		default:
			break;
		}
	}

	return hit;
}

bool intersect(Model& model, Ray& ray, Point3f& point, Material* &material, Vec3f& normal)
{
	return intersect(model.kdTree, ray, point, material, normal);
}

bool intersect(Model& model, Ray& ray, JudgeOrCal jOC)
{
	Point3f point;
	Material* material;
	Vec3f normal;
	return intersect(model.kdTree, ray, point, material, normal, jOC);
}

