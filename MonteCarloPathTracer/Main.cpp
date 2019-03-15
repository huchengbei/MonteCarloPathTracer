#include "GL/freeglut.h"
#include "iostream"
#include "string"
#include "PathTracer.h"
#include "Scene.cpp"
using namespace std;

int width;
int height;
float fov;
PathTracer pathTracer;
Scene*  scene;
void initWindow();

void render()
{
	static int cnt = 1;
	cout << cnt++ << endl;
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	vector<float> colors = pathTracer.render((*scene));

	glRasterPos2i(0, 0);
	glDrawPixels(width, height, GL_RGB, GL_FLOAT, (GLvoid *)colors.data());

	glFlush();
}

void loadScene(string path)
{
	width = 100;
	height = 100;
	fov = 70;

	scene = new Scene(width, height);
	scene->model = new Model(path);
	Model* &model = scene->model;

	Point3f center = (model->box.high + model->box.low) / 2;
	float scale = length(model->box.high - model->box.low) / 2;
	Point3f pos = Point3f(center.x, center.y, center.z + 1.5 * scale);
	scene->camera = new Camera(pos, center, Vec3f(0, 1, 0));
	Camera* &camera = scene->camera;

	camera->setViewPort(fov, (float)height / (float)width);

	pos = Point3f(-1, 9.8, 1);
	Vec3f dy = Vec3f(2, 0, 0);
	Vec3f dx = Vec3f(0, 0, -2);
	Color3f emission = Color3f(50, 50, 50);
	scene->lights.push_back(Light(pos, dx, dy, emission));
	scene->lights.push_back(Light(pos + dx + dy, -dx, -dy, emission));

	scene->init();
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
	glutInitWindowPosition(50, 80);
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
