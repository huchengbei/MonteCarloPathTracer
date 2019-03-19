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
double  time_sum;
string path;
string windowName; // saveImage
string resultDir;

int MaxRenderCnt = 100;
float fov;
PathTracer pathTracer;
Model*  model;

void loadImage();
void loadScene(string path);
void render(cv::Mat &image);

int main(int argc, char *argv[])
{
	windowName = "Monte Carlo Path Tracer";
	cv::namedWindow(windowName);
	width = 200;
	height =200;
	saveImage = true;
	MaxRenderCnt = 100;
	image = cv::Mat(width, height, CV_8UC3);
	path = "../models/scene01.obj";
	loadScene(path);
	system("pause");
	return 0;
}


void loadScene(string path)
{
	fov = 70;
	logs.out("Load Model...");
	model = new Model(path);
	logs.out("Load Model...finished");
	model->width = width;
	model->height = height;
	model->ambient = Color3f(0.2f, 0.2f, 0.2f);

	Point3f center = (model->box.high + model->box.low) / 2;
	float scale = length(model->box.high - model->box.low) / 2;
	Point3f pos = Point3f(center.x, center.y, center.z + 1.5 * scale);

	model->camera = new Camera(pos, center, Vec3f(0, 1, 0));
	Camera* &camera = model->camera;
	camera->setViewPort(fov, (float)height / (float)width);

	vector<Light> lights;
	pos = Point3f(-1, 9.8, 1);
	Vec3f dy = Vec3f(2, 0, 0);
	Vec3f dx = Vec3f(0, 0, -2);
	Color3f emission = Color3f(50, 50, 50);

	// lights.push_back(Light(pos, dx, dy, emission));
	// lights.push_back(Light(pos + dx + dy, -dx, -dy, emission));
	lights.push_back(Light(Light::TYPE::POLYGON, Point3f(0.0f, 9.8f, 0.0f), 1, emission * 1.75, 4, Vec3f(0, -1, 0)));
	model->lights = &lights;

	logs.out("Init Model And buildTree...");
	model->init();
	logs.out("Init Model And buildTree...finished");
	loadImage();
}

void loadImage()
{
	static int cnt = 1;
	pathTracer = PathTracer();
	pathTracer.pxSampleNum = 1;
	pathTracer.lightSampleNum = 3;
	pathTracer.maxPathDepth = 3;
	pathTracer.maxRenderDepth = MaxRenderCnt;

	logs.out("width: " + to_string(width) + ", height: " + to_string(height));
	while (true)
	{
		chrono::time_point<chrono::steady_clock> t_start = std::chrono::high_resolution_clock::now();
		logs.out("start " + to_string(cnt) + " iterate");

		render(image);
		
		logs.out("end " + to_string(cnt++) + " iterate");
		chrono::time_point<chrono::steady_clock> t_end = chrono::high_resolution_clock::now();
		double time = chrono::duration_cast<chrono::duration<float>>
			(t_end - t_start).count();
		time_sum += time;
		char content[50];
		sprintf_s(content, "Time: %.2fs Total: %.2fs Avg: %.2fs", time, time_sum, time_sum / (float)(cnt - 1));
		logs.out(content);

		if (saveImage && (cnt - 1) % 5 == 0)
		{
			if (!std::experimental::filesystem::exists(resultDir)) {
				int pos = (int)(path.find_last_of('/'));
				resultDir = path.substr(0, pos + 1) + "results/";
				logs.out("Can't find result directory. It will be create in the obj's directory and named result");
				std::experimental::filesystem::create_directories(resultDir);
			}
			char imageName[10];
			sprintf_s(imageName, "%03d.jpg", cnt - 1);
			string fileName = resultDir + imageName;
			cv::imwrite(fileName, image);
			logs.out("Save image " + string(imageName) + " in the directory: " + resultDir);
		}

		cv::imshow(windowName, image);
		cv::waitKey(1);
	}
}

void render(cv::Mat &image)
{
	vector<float> colors = pathTracer.render(*model);

	// it can be remove
	image.resize(width, height);
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
