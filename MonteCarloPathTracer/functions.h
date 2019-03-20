#pragma once
#include "MetaClass.h"
#include "Ray.cpp"
#include "Model.h"
#include "KDTree.h"
#include "Material.cpp"

// Judge Or Cal Intersection
enum JudgeOrCal{JUDGE, CAL};

bool intersect(Box& box, Ray& ray);

bool intersect(Triangle *trangle, Ray& ray, Point3f& point, Material* &material, Vec3f& normal);

bool intersect(Face *face, Ray& ray, Point3f& point, Material* &material, Vec3f& normal);

bool intersect(KDTree* kdTree, Ray& ray, Point3f& point, Material* &material, Vec3f& normal, JudgeOrCal jOC = JudgeOrCal::CAL);

bool intersect(Model& model, Ray& ray, Point3f& point, Material* &material, Vec3f& normal);

bool intersect(Model& model, Ray& ray, JudgeOrCal jOC = JudgeOrCal::CAL);
