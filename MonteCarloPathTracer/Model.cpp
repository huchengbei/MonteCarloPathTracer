#pragma once
#include "Model.h"
#include "iostream"
#include "fstream"

using namespace std;

Triangle::Triangle(Model* model, int vertex_ids[3], int normal_ids[3])
{
	for (int i = 0; i < 3; i++)
	{
		vertex[i] = model->vertexs[vertex_ids[i]];
		vertexNormal[i] = model->normals[normal_ids[i]];
	}
	edge1 = vertex[1] - vertex[0];
	edge2 = vertex[2] - vertex[0];
	normal = normalize(cross(edge1, edge2));
	point = vertex[0];

	barycentric = Mat4(vertex[0], vertex[1], vertex[2]);
	barycentric.inverse();
	setBox();
}

void Triangle::setBox()
{
	Point3f low, high;
	low.x = min(vertex[0].x, min(vertex[1].x, vertex[2].x));
	low.y = min(vertex[0].y, min(vertex[1].y, vertex[2].y));
	low.z = min(vertex[0].z, min(vertex[1].z, vertex[2].z));

	high.x = max(vertex[0].x, max(vertex[1].x, vertex[2].x));
	high.y = max(vertex[0].y, max(vertex[1].y, vertex[2].y));
	high.z = max(vertex[0].z, max(vertex[1].z, vertex[2].z));

	box = Box(low, high);
}

Box Triangle::getBox()
{
	return box;
}

Vec3f Triangle::getIntersectioNormal(Vec3f &point)
{
	Vec3f abg = barycentric * point;
	abg.abs();

	return normalize(abg.x * vertexNormal[0] + abg.y * vertexNormal[1] + abg.z * vertexNormal[2]);
}

Model::Model(string path)
{
	if (Load(path))
	{
		cout << "Load " << path << "successful" << endl << endl;
		cout << "Triangles: " << triangles.size() << endl;
	}
	else
	{
		cout << "Load " << path << "error." << endl;
	}
}

bool Model::LoadMaterial(string path, map<string, Material> &materialTable)
{
	ifstream file(path);
	if (!file.is_open())
	{
		cerr << "open mtl file error. file path: " << path << endl;
		return false;
	}

	bool flag = false;
	string materialName;
	Material material;
	int illum;

	string type;

	while (file >> type)
	{
		if (type == "newmtl")
		{
			if (flag)
			{
				materialTable.insert(make_pair(materialName, material));
				material = Material();
			}
			else
				flag = true;
			file >> materialName;
			material.name = materialName;
		}
		else if (type == "Kd")
			file >> material.Kd.x >> material.Kd.y >> material.Kd.z;
		else if (type == "Ka")
			file >> material.Ka.x >> material.Ka.y >> material.Ka.z;
		else if (type == "Ks")
			file >> material.Ks.x >> material.Ks.y >> material.Ks.z;
		else if (type == "Ke")
			file >> material.Le.r >> material.Le.g >> material.Le.b;
		else if (type == "Ns")
			file >> material.Ns;
		else if (type == "Ni")
			file >> material.Ni;
		else if (type == "Tf")
			file >> material.Tf.x >> material.Tf.y >> material.Tf.z;
		else if (type == "illum")
			file >> material.illum;
		else
			file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
	}

	if (flag)
		materialTable.insert(make_pair(materialName, material));

	file.close();
	return true;
}

bool Model::Load(string path)
{
	ifstream file(path);
	if (!file.is_open())
	{
		cerr << "open obj file error. file path: " << path << endl;
		return false;
	}

	bool returnValue = true;
	map<string, Material> materialTable;

	string type;
	Point3f v;
	Vec3f vn;
	string materialName;
	Material material;
	int vertex_ids[3];
	int vertex_normal_ids[3];

	while (file >> type)
	{
		if (type == "mtllib")
		{
			string mtlFileName;
			file >> mtlFileName;
			int pos = (int)(path.find_last_of('/'));
			string mtlPath = path.substr(0, pos + 1) + mtlFileName;
			if (!LoadMaterial(mtlPath, materialTable))
			{
				returnValue = false;
				cerr << "load mtl file error. file path: " << path << endl;
				break;
			}
		}
		else if (type == "usemtl")
		{
			file >> materialName;
			map<string, Material>::const_iterator it = materialTable.find(materialName);
			if (it != materialTable.end())
			{
				material = it->second;
			}
			else
			{
				returnValue = false;
				cerr << "mtl name: " << materialName << "not find in mtl file" << endl;
				break;
			}
		}
		else if (type == "v")
		{
			file >> v.x >> v.y >> v.z;
			box.changeBoard(v);
			vertexs.push_back(v);
		}
		else if (type == "vn")
		{
			file >> vn.x >> vn.y >> vn.z;
			normals.push_back(vn);
		}
		else if (type == "f")
		{
			int index = 0;
			while (true)
			{
				char ch = file.get();
				if (ch == ' ') continue;
				else if (ch == '\n' || ch == EOF) break;
				else file.putback(ch);

				file >> vertex_ids[index];
				vertex_ids[index] --;

				char split = file.get();

				if (split == '/')
				{
					split = file.get();
					if (split == '/')
					{
						file >> vertex_normal_ids[index];
						vertex_normal_ids[index]--;
					}
					else
					{
						int tIndex;
						file.putback(split);
						file >> tIndex;
						split = file.get();
						if (split == '/')
						{
							file >> vertex_normal_ids[index];
							vertex_normal_ids[index]--;
						}
						else file.putback(split);
					}
				}
				else file.putback(split);

				index++;
				if (index >= 3)
				{
					Triangle * tri = new Triangle(this, vertex_ids, vertex_normal_ids);
					tri->material = material;
					triangles.push_back(tri);
					
					if (material.Le != Color3f(0, 0, 0))
					{
						// lights.push_back(Light(tri->point, tri->edge1, tri->edge2, material.Le));
					}

					vertex_ids[1] = vertex_ids[2];
					vertex_normal_ids[1] = vertex_normal_ids[2];
					index = 2;
				}
			}
		}
	}
	file.close();
	return returnValue;
}