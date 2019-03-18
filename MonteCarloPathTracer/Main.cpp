#include "GL/freeglut.h"
#include "iostream"
#include "string"
#include "PathTracer.h"
#include "Model.h"
#include "Camera.cpp"
#include "Log.cpp"
#include "chrono"
#include "iomanip"
using namespace std;

Log logs;
int width;
int height;
string path;
int MaxRenderCnt = 100;
float fov;
PathTracer pathTracer;
Model*  model;
double  time_sum;

void initWindow();
void loadScene(string path);
void render();
void redisplay();

int main(int argc, char *argv[])
{
	width = 400;
	height =400;
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

	lights.push_back(Light(pos, dx, dy, emission));
	lights.push_back(Light(pos + dx + dy, -dx, -dy, emission));
	model->lights = &lights;

	logs.out("Init Model And buildTree...");
	model->init();
	logs.out("Init Model And buildTree...finished");
	initWindow();
}

void initWindow()
{
	pathTracer = PathTracer();
	pathTracer.pxSampleNum = 1;
	pathTracer.lightSampleNum = 3;
	pathTracer.maxPathDepth = 5;
	pathTracer.maxRenderDepth = 100;

	logs.out("width: " + to_string(width) + ", height: " + to_string(height));
	int argc = 1;
	char** argv= NULL;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Monte Carlo Path Tracer");
	glutDisplayFunc(render);
	glutIdleFunc(redisplay);
	glutMainLoop();
}

void render()
{
	static int cnt = 1;
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	if (cnt <= MaxRenderCnt)
	{
		chrono::time_point<chrono::steady_clock> t_start = std::chrono::high_resolution_clock::now();
		logs.out("start " + to_string(cnt) + " iterate");
		pathTracer.render(*model);
		logs.out("end " + to_string(cnt++) + " iterate");
		chrono::time_point<chrono::steady_clock> t_end = chrono::high_resolution_clock::now();
		double time = chrono::duration_cast<chrono::duration<float>>
			(t_end - t_start).count();
		time_sum += time;
		char content[50];
		sprintf_s(content, "Time: %.2fs Total: %.2fs Avg: %.2fs", time, time_sum, time_sum / (float)(cnt - 1));
		logs.out(content);
	}

	glRasterPos2i(0, 0);
	glDrawPixels(width, height, GL_RGB, GL_FLOAT, (GLvoid *)model->colors.data());

	glFlush();
}

void redisplay()
{
	glutPostRedisplay();
}
