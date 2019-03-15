#pragma once
#include "MetaClass.h"
#include "Scene.cpp"
#include "Ray.cpp"
#include "Model.h"
#include "KDTree.cpp"
#include "Material.cpp"

bool intersect(Box& box, Ray& ray);

bool intersect(Triangle *trangle, Ray& ray, Point3f& point, Material& material, Vec3f& normal);

bool intersect(KDTree* kdTree, Ray& ray, Point3f& point, Material& material, Vec3f& normal);

bool intersect(Scene& scene, Ray& ray, Point3f& point, Material& material, Vec3f& normal);

bool intersect(Scene& scene, Ray& ray);
