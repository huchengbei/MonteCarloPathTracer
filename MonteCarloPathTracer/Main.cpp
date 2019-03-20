#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "iostream"
#include <experimental/filesystem>
#include "string"
#include "PathTracer.h"
#include "Model.h"
#include "Camera.cpp"
#include "Log.cpp"
#include "chrono"
#include "iomanip"
using namespace std;

Log logs;
cv::Mat image;
int width;
int height;
bool saveImage = true; // saveImage
int savePreImage = 5;
bool log2file = true;
double  time_sum;
string path;
string windowName;
string resultDir;

int MaxRenderCnt = 100;
float fov;
PathTracer pathTracer;
Model*  model;

void loadImage();
void render(cv::Mat &image);

// cup model
void loadSceneCup()
{
	windowName = "Cup - Monte Carlo Path Tracer";
	cv::namedWindow(windowName);
	saveImage = true;
	savePreImage = 1;
	log2file = true;
	MaxRenderCnt = 100;

	path = "../models/Scene01/cup.obj";
	int pos = (int)(path.find_last_of('.'));
	resultDir = path.substr(0, pos) + "_results/";

	if (log2file)
		logs.setPath(path.substr(0, pos) + ".log");

	pathTracer = PathTracer();
	pathTracer.pxSampleNum = 1;
	pathTracer.lightSampleNum = 3;
	pathTracer.maxPathDepth = 5;
	pathTracer.maxRenderDepth = MaxRenderCnt;

	width = 512;
	height = 512;
	image = cv::Mat(height, width, CV_8UC3);
	logs.out("Load Model...");
	bool enableInternalLight = false;
	model = new Model(path, enableInternalLight);
	logs.out("Faces: " + to_string(model->facesNum) + "  Triangles: " + to_string(model->trianglesNum));
	logs.out("Load Model...finished");
	model->width = width;
	model->height = height;
	model->ambient = Color3f(0.2f, 0.2f, 0.2f);

	vector<Light>* lights = model->lights;
	Point3f lightCenter = Point3f(-2.758771896, 1.5246, 0);
	float radius = 0.5f;
	float area = 1.0f;
	Color3f emission = Color3f(40, 40, 40);
	Vec3f normal = Vec3f(1, 0, 0);
	// add external light
	lights->push_back(Light(Light::TYPE::POLYGON, lightCenter, radius, emission * 2.0f, area, normal));

	Point3f position = Point3f(0.0, 0.64, 0.52);
	Point3f center = Color3f(0.0, 0.40, 0.3);
	Vec3f up = Vec3f(0, 1, 0);
	fov = 60;

	model->camera = new Camera(position, center, up);
	Camera* &camera = model->camera;
	camera->setViewPort(fov, (float)height / (float)width);

	logs.out("Init Model And buildTree...");
	model->init();
	logs.out("Init Model And buildTree...finished");
	loadImage();
}

// room model
void loadSceneRoom()
{
	windowName = "Room - Monte Carlo Path Tracer";
	cv::namedWindow(windowName);
	saveImage = true;
	savePreImage = 1;
	log2file = true;
	MaxRenderCnt = 100;

	path = "../models/Scene02/room.obj";
	int pos = (int)(path.find_last_of('.'));
	resultDir = path.substr(0, pos) + "_results/";

	if (log2file)
		logs.setPath(path.substr(0, pos) + ".log");

	pathTracer = PathTracer();
	pathTracer.pxSampleNum = 1;
	pathTracer.lightSampleNum = 3;
	pathTracer.maxPathDepth = 5;
	pathTracer.maxRenderDepth = MaxRenderCnt;

	width = 512;
	height = 512;
	image = cv::Mat(height, width, CV_8UC3);
	logs.out("Load Model...");
	bool enableInternalLight = false;
	model = new Model(path, enableInternalLight);
	logs.out("Faces: " + to_string(model->facesNum) + "  Triangles: " + to_string(model->trianglesNum));
	logs.out("Load Model...finished");
	model->width = width;
	model->height = height;
	model->ambient = Color3f(0.2f, 0.2f, 0.2f);

	vector<Light>* lights = model->lights;
	Point3f lightCenter = Point3f(0.0, 1.589, -1.274);
	float radius = 0.2f;
	Color3f emission = Color3f(50, 50, 40);
	// add external light
	lights->push_back(Light(Light::TYPE::SPHERE, lightCenter, radius, emission * 2.0f));

	Point3f position = Point3f(0.0, 0.0, 0.4);
	Point3f center = Color3f(0.0, 0.0, 0.0);
	Vec3f up = Vec3f(0, 1, 0);
	fov = 50;

	model->camera = new Camera(position, center, up);
	Camera* &camera = model->camera;
	camera->setViewPort(fov, (float)height / (float)width);

	logs.out("Init Model And buildTree...");
	model->init();
	logs.out("Init Model And buildTree...finished");
	loadImage();
}

// Veach MIS model
void loadSceneVeach()
{
	windowName = "Veach MIS - Monte Carlo Path Tracer";
	cv::namedWindow(windowName);
	saveImage = true;
	savePreImage = 1;
	log2file = true;
	MaxRenderCnt = 100;

	path = "../models/Scene03/VeachMIS.obj";
	int pos = (int)(path.find_last_of('.'));
	resultDir = path.substr(0, pos) + "_results/";

	if (log2file)
		logs.setPath(path.substr(0, pos) + ".log");

	pathTracer = PathTracer();
	pathTracer.pxSampleNum = 1;
	pathTracer.lightSampleNum = 3;
	pathTracer.maxPathDepth = 5;
	pathTracer.maxRenderDepth = MaxRenderCnt;

	width = 1152;
	height = 864;
	image = cv::Mat(height, width, CV_8UC3);
	logs.out("Load Model...");
	bool enableInternalLight = false;
	model = new Model(path, enableInternalLight);
	logs.out("Faces: " + to_string(model->facesNum) + "  Triangles: " + to_string(model->trianglesNum));
	logs.out("Load Model...finished");
	model->width = width;
	model->height = height;
	model->ambient = Color3f(0.2f, 0.2f, 0.2f);

	vector<Light>* lights = model->lights;
	Point3f lightCenter = Point3f(-10, 10, 4);
	float radius = 0.5f;
	Color3f emission = Color3f(800, 800, 800);
	// add external light
	lights->push_back(Light(Light::TYPE::SPHERE, lightCenter, radius, emission * 2.0f));

	lightCenter = Point3f(3.75, 0, 0);
	radius = 0.033f;
	emission = Color3f(901.803, 901.803, 901.803);
	lights->push_back(Light(Light::TYPE::SPHERE, lightCenter, radius, emission * 2.0f));

	lightCenter = Point3f(1.25, 0, 0);
	radius = 0.1f;
	emission = Color3f(100, 100, 100);
	lights->push_back(Light(Light::TYPE::SPHERE, lightCenter, radius, emission * 2.0f));

	lightCenter = Point3f(-1.25, 0, 0);
	radius = 0.3f;
	emission = Color3f(11.1111, 11.1111, 11.1111);
	lights->push_back(Light(Light::TYPE::SPHERE, lightCenter, radius, emission * 2.0f));

	lightCenter = Point3f(-3.75, 0, 0);
	radius = 0.9f;
	emission = Color3f(1.23457, 1.23457, 1.23457);
	lights->push_back(Light(Light::TYPE::SPHERE, lightCenter, radius, emission * 2.0f));

	Point3f position = Point3f(0.0, 2.0, 15.0);
	Point3f center = Color3f(0.0, 1.69521, 14.0476);
	Vec3f up = Vec3f(0.0, 0.952421, -0.304787);
	fov = 38;

	model->camera = new Camera(position, center, up);
	Camera* &camera = model->camera;
	camera->setViewPort(fov, (float)height / (float)width);


	logs.out("Init Model And buildTree...");
	model->init();
	logs.out("Init Model And buildTree...finished");
	loadImage();
}

// classical model
void loadScene0()
{
	windowName = "Classical - Monte Carlo Path Tracer";
	cv::namedWindow(windowName);
	saveImage = true;
	savePreImage = 1;
	log2file = true;
	MaxRenderCnt = 100;

	path = "../models/scene01.obj";
	int pos = (int)(path.find_last_of('.'));
	resultDir = path.substr(0, pos) + "_results/";

	if (log2file)
		logs.setPath(path.substr(0, pos) + ".log");

	pathTracer = PathTracer();
	pathTracer.pxSampleNum = 1;
	pathTracer.lightSampleNum = 3;
	pathTracer.maxPathDepth = 5;
	pathTracer.maxRenderDepth = MaxRenderCnt;

	width = 500;
	height = 500;
	image = cv::Mat(height, width, CV_8UC3);
	logs.out("Load Model...");
	bool enableInternalLight = false;
	model = new Model(path, enableInternalLight);
	logs.out("Faces: " + to_string(model->facesNum) + "  Triangles: " + to_string(model->trianglesNum));
	logs.out("Load Model...finished");
	model->width = width;
	model->height = height;
	model->ambient = Color3f(0.2f, 0.2f, 0.2f);

	vector<Light>* lights = model->lights;
	Point3f lightCenter = Point3f(0, 9.8, 0);
	float radius = 1.0f;
	float area = 4.0f;
	Color3f emission = Color3f(50, 50, 50);
	Vec3f normal = Vec3f(0, -1, 0);
	// add external light
	lights->push_back(Light(Light::TYPE::POLYGON, lightCenter, radius, emission * 2.0f, area, normal));

	Point3f center = (model->box.high + model->box.low) / 2;
	float scale = length(model->box.high - model->box.low) / 2;
	Point3f position = Point3f(center.x, center.y, center.z + 1.5 * scale);
	Vec3f up = Vec3f(0, 1, 0);
	fov = 70;

	model->camera = new Camera(position, center, up);
	Camera* &camera = model->camera;
	camera->setViewPort(fov, (float)height / (float)width);

	logs.out("Init Model And buildTree...");
	model->init();
	logs.out("Init Model And buildTree...finished");
	loadImage();
}

int main(int argc, char *argv[])
{
	// loadSceneCup();
	// loadSceneRoom();
	// loadSceneVeach();
	loadScene0();
	system("pause");
	return 0;
}

void loadImage()
{
	static int cnt = 1;

	logs.out("width: " + to_string(width) + ", height: " + to_string(height));

	while (cnt <= MaxRenderCnt)
	{
		chrono::time_point<chrono::steady_clock> t_start = std::chrono::high_resolution_clock::now();
		logs.out("start " + to_string(cnt) + " iterate");

		render(image);
		
		logs.out("end " + to_string(cnt) + " iterate");
		chrono::time_point<chrono::steady_clock> t_end = chrono::high_resolution_clock::now();
		double time = chrono::duration_cast<chrono::duration<float>>
			(t_end - t_start).count();
		time_sum += time;
		char content[50];
		sprintf_s(content, "Time: %.2fs Total: %.2fs Avg: %.2fs", time, time_sum, time_sum / (float)cnt);
		logs.out(content);

		if (saveImage && cnt % savePreImage == 0)
		{
			if (!std::experimental::filesystem::exists(resultDir)) {
				logs.out("Can't find result directory. It will be create in the obj's directory and named result");
				std::experimental::filesystem::create_directories(resultDir);
			}
			char imageName[10];
			sprintf_s(imageName, "%04d.jpg", cnt);
			string fileName = resultDir + imageName;
			cv::imwrite(fileName, image);
			logs.out("Save image " + string(imageName) + " in the directory: " + resultDir);
		}
		cnt++;

		cv::imshow(windowName, image);
		cv::waitKey(1);
	}
	cv::imshow(windowName, image);
	cv::waitKey(0);
}

void render(cv::Mat &image)
{
	vector<float> colors = pathTracer.render(*model);

	// it can be remove
	image.resize(height, width);
	for (size_t y = 0; y < height ; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			int i = y;
			int j = x;
			int index = (y * width + x) * 3;
			float r = colors[index];
			float g = colors[index + 1];
			float b = colors[index + 2];
			r = r > 1.0f ? 1.0f : r;
			r = r < 0.0f ? 0.0f : r;
			g = g > 1.0f ? 1.0f : g;
			g = g < 0.0f ? 0.0f : g;
			b = b > 1.0f ? 1.0f : b;
			b = b < 0.0f ? 0.0f : b;
			image.at<cv::Vec3b>(height - y - 1, x)[0] = (int)(b * 255);
			image.at<cv::Vec3b>(height - y - 1, x)[1] = (int)(g * 255);
			image.at<cv::Vec3b>(height - y - 1, x)[2] = (int)(r * 255);
		}
	}
}
