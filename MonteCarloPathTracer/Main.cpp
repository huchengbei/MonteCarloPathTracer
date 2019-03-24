#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "yaml-cpp/yaml.h"
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
bool enableInternalLight = false;
double  time_sum;
string path;
string windowName;
string resultDir;
string logPath;

int MaxRenderCnt = 100;
float fov;
PathTracer pathTracer;
Model*  model;

void loadImage();
void LoadScene(string path);
void render(cv::Mat &image);

int main(int argc, char *argv[])
{
// 	string path("models_configs/classical.yaml");
	if (argc < 2)
	{
		cout << "You should use this pram by this format:" << endl;
		cout << "MonteCarloPathTracer.exe example.yaml" << endl;
		system("pause");
		return 0;
	}
	string path(argv[1]);
	LoadScene(path);
	system("pause");
	return 0;
}

void LoadScene(string path)
{
	YAML::Node config = YAML::LoadFile(path);

	windowName = config["windowName"].as<string>();
	cv::namedWindow(windowName);
	saveImage = config["saveImage"].as<bool>();
	savePreImage = config["savePreImage"].as<int>();
	log2file = config["log2file"].as<bool>();
	MaxRenderCnt = config["MaxRenderCnt"].as<int>();

	path = config["objPath"].as<string>();
	resultDir = config["resultDir"].as<string>();
	logPath = config["logPath"].as<string>();

	if (log2file)
		logs.setPath(logPath);

	YAML::Node pTConfig = config["pathTracer"];
	pathTracer = PathTracer();
	pathTracer.pxSampleNum = pTConfig["pxSampleNum"].as<int>();
	pathTracer.lightSampleNum = pTConfig["lightSampleNum"].as<int>();
	pathTracer.maxPathDepth = pTConfig["maxPathDepth"].as<int>();
	// pathTracer.maxRenderDepth = pTConfig["maxRenderDepth"].as<int>();
	pathTracer.maxRenderDepth = MaxRenderCnt;

	YAML::Node modelConfig = config["model"];
	width = modelConfig["width"].as<int>();
	height = modelConfig["height"].as<int>();
	image = cv::Mat(height, width, CV_8UC3);
	logs.out("Load Model...");
	enableInternalLight = config["enableInternalLight"].as<bool>();
	model = new Model(path, enableInternalLight);
	logs.out("Faces: " + to_string(model->facesNum) + "  Triangles: " + to_string(model->trianglesNum));
	logs.out("Load Model...finished");
	model->width = width;
	model->height = height;
	YAML::Node ambientConfig = modelConfig["ambient"];
	model->ambient = Color3f(ambientConfig[0].as<float>(), ambientConfig[1].as<float>(), ambientConfig[2].as<float>());

	YAML::Node cameraConfig = config["camera"];
	YAML::Node cameraCenterConfig = cameraConfig["lookat"];
	Point3f cameraCenter = Point3f(cameraCenterConfig[0].as<float>(), cameraCenterConfig[1].as<float>(), cameraCenterConfig[2].as<float>());
	YAML::Node positionConfig = cameraConfig["position"];
	Point3f position = Point3f(positionConfig[0].as<float>(), positionConfig[1].as<float>(), positionConfig[2].as<float>());
	YAML::Node upConfig = cameraConfig["up"];
	Vec3f up = Vec3f(upConfig[0].as<float>(), upConfig[1].as<float>(), upConfig[2].as<float>());
	fov = cameraConfig["fov"].as<float>();

	model->camera = new Camera(position, cameraCenter, up);
	Camera* &camera = model->camera;
	camera->setViewPort(fov, (float)height / (float)width);

	vector<Light>* lights = model->lights;
	// add external light
	YAML::Node lightsConfig = config["lights"];
	for (int i = 0; i < lightsConfig.size(); i++)
	{
		YAML::Node lightConfig = lightsConfig[i];
		YAML::Node lightCenterConfig = lightConfig["center"];
		Point3f lightCenter = Point3f(lightCenterConfig[0].as<float>(), lightCenterConfig[1].as<float>(), lightCenterConfig[2].as<float>());

		float radius = lightConfig["radius"].as<float>();

		YAML::Node emissionConfig = lightConfig["emission"];
		Color3f emission = Point3f(emissionConfig[0].as<float>(), emissionConfig[1].as<float>(), emissionConfig[2].as<float>());

		string type = lightConfig["type"].as<string>();
		if (type == "POLYGON")
		{
			float area = lightConfig["area"].as<float>();
			YAML::Node normalConfig = lightConfig["normal"];
			Vec3f normal = Vec3f(normalConfig[0].as<float>(), normalConfig[1].as<float>(), normalConfig[2].as<float>());
			lights->push_back(Light(Light::TYPE::POLYGON, lightCenter, radius, emission * 2.0f, area, normal));
		}
		else
		{
			lights->push_back(Light(Light::TYPE::SPHERE, lightCenter, radius, emission * 2.0f));
		}
	}

	logs.out("Init Model And buildTree...");
	model->init();
	logs.out("Init Model And buildTree...finished");
	loadImage();

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
