#include "GL/freeglut.h"
#include "iostream"
#include "string"
#include "PathTracer.h"
#include "Scene.cpp"
#include "chrono"
#include "iomanip"
using namespace std;

int width;
int height;
int MaxRenderCnt = 100;
float fov;
PathTracer pathTracer;
Model*  model;
double  time_sum;
void initWindow();

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
		auto t_start = std::chrono::high_resolution_clock::now();
		cout <<"start "  << cnt << " itera" << endl;
		pathTracer.render(*model);
		cout <<"end "  << cnt++ << " itera" << endl;
		auto t_end = std::chrono::high_resolution_clock::now();
		double tloop = std::chrono::duration_cast<std::chrono::duration<double>>
			(t_end - t_start).count();
		time_sum += tloop;
		cout << std::fixed << std::setprecision(4) << "Time: " << tloop << "s Sum: " <<
			time_sum << "s" << endl;
	}

	glRasterPos2i(0, 0);
	glDrawPixels(width, height, GL_RGB, GL_FLOAT, (GLvoid *)model->colors.data());

	glFlush();
}

void loadScene(string path)
{
	cout << "Load Scene..." << endl;
	width = 400;
	height =400;
	fov = 70;

	model = new Model(path);
	model->width = width;
	model->height = height;
	cout << "Load Model..." << endl;
	// scene->model = new Model(path);
	cout << "Load Model...finished" << endl;
	//Model* &model = scene->model;

	Point3f center = (model->box.high + model->box.low) / 2;
	float scale = length(model->box.high - model->box.low) / 2;
	Point3f pos = Point3f(center.x, center.y, center.z + 1.5 * scale);
	model->camera = new Camera(pos, center, Vec3f(0, 1, 0));
	Camera* &camera = model->camera;
	camera->setViewPort(fov, (float)height / (float)width);

	pos = Point3f(-1, 9.8, 1);
	Vec3f dy = Vec3f(2, 0, 0);
	Vec3f dx = Vec3f(0, 0, -2);
	Color3f emission = Color3f(50, 50, 50);

	vector<Light> lights;
	lights.push_back(Light(pos, dx, dy, emission));
	lights.push_back(Light(pos + dx + dy, -dx, -dy, emission));
	model->lights = &lights;

	cout << "Init Scene & buildTree..." << endl;
	model->init();
	cout << "Init Scene & buildTree...finished" << endl;
	initWindow();
}
void update()
{
	glutPostRedisplay();
}

void initWindow()
{
	pathTracer = PathTracer();
	cout << "width:" << width << ",height:" << height << endl;
	int argc = 1;
	char** argv= NULL;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Monte Carlo Path Tracer");
	glutDisplayFunc(render);
	glutIdleFunc(update);
	glutMainLoop();
}


int main(int argc, char *argv[])
{
	// freopen("data.out", "w", stdout);
	loadScene("../models/scene01.obj");
	/*
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("HelloTriangle");

	glutDisplayFunc(render);
	glutMainLoop();
	*/
	system("pause");
	return 0;
}
