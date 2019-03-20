#pragma once
#include "Model.h"
#include "Camera.cpp"
#include "iostream"
#include "fstream"

using namespace std;

Model::Model(string path, bool isEnableInternalLight)
{
	enableInternalLight = isEnableInternalLight;
	kdTree = new KDTree();
	lights = new vector<Light>;
	if (Load(path))
	{
		cout << "\tLoad " << path << " successful" << endl;
	}
	else
	{
		cout << "\tLoad " << path << "error." << endl;
	}
}

void Model::init()
{
	colors.clear();
	colors.resize(3 * width * height);
	kdTree->buildTree(faces);
}

bool Model::LoadMaterial(string path)
{
	ifstream file(path);
	if (!file.is_open())
	{
		cerr << "\topen mtl file error. file path: " << path << endl;
		return false;
	}

	bool flag = false;
	string materialName;
	Material material;

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
		cerr << "\topen obj file error. file path: " << path << endl;
		return false;
	}

	bool returnValue = true;

	string type;
	Point3f v;
	Vec3f vn;
	string materialName;
	Material* material = nullptr;
	vector<int> vertex_ids;
	vector<int> vertex_normal_ids;

	while (file >> type)
	{
		if (type == "mtllib")
		{
			string mtlFileName;
			file >> mtlFileName;
			int pos = (int)(path.find_last_of('/'));
			string mtlPath = path.substr(0, pos + 1) + mtlFileName;
			if (!LoadMaterial(mtlPath))
			{
				returnValue = false;
				cerr << "\tload mtl file error. file path: " << path << endl;
				break;
			}
		}
		else if (type == "usemtl")
		{
			file >> materialName;
			map<string, Material>::const_iterator it = materialTable.find(materialName);
			if (it != materialTable.end())
			{
				material = (Material*)(&(it->second));
			}
			else
			{
				returnValue = false;
				cerr << "\tmtl name: " << materialName << " not find in mtl file" << endl;
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
			facesNum++;
			int index = 0;
			vertex_ids.clear();
			vertex_normal_ids.clear();
			while (true)
			{
				char ch = file.get();
				if (ch == ' ') continue;
				else if (ch == '\n' || ch == EOF) break;
				else file.putback(ch);

				vertex_ids.resize(index + 1);
				file >> vertex_ids[index];
				vertex_ids[index] --;

				char split = file.get();

				if (split == '/')
				{
					split = file.get();
					if (split == '/')
					{
						vertex_normal_ids.resize(index + 1);
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
							vertex_normal_ids.resize(index + 1);
							file >> vertex_normal_ids[index];
							vertex_normal_ids[index]--;
						}
						else file.putback(split);
					}
				}
				else file.putback(split);

				index++;
			}
			// move to face
			Face * face = new Face(this, vertex_ids, vertex_normal_ids, material);
			faces.push_back(face);
			trianglesNum += face->triangles.size();
		}
	}
	file.close();
	return returnValue;
}

vector<Ray> Model::getRays(int x, int y, int px_sample_num)
{
	vector<Ray> rays;
	rays.resize(px_sample_num);

	for (Ray &ray : rays)
	{
		float randX = (float)rand() / RAND_MAX;
		float randY = (float)rand() / RAND_MAX;
		ray = camera->getRay((x + randX) / width, (y + randY) / height);
	}

	return rays;
}

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

Face::Face(Model * m, vector<int>& vs_i, vector<int>& ns_i, Material* mater)
{
	int v_index[3];
	int n_index[3];
	v_index[0] = vs_i[0];
	v_index[1] = vs_i[1];
	n_index[0] = ns_i[0];
	n_index[1] = ns_i[1];

	for (int i = 2; i < vs_i.size(); i++)
	{
		v_index[2] = vs_i[i];
		n_index[2] = ns_i[i];
		Triangle* tri = new Triangle(m, v_index, n_index);
		tri->material = mater;
		box = Box::merge(box,tri->box);
		triangles.push_back(tri);
		v_index[1] = v_index[2];
		n_index[1] = n_index[2];
	}
}

Box Face::getBox()
{
	return box;
}
