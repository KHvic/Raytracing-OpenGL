// CS3241Lab5.cpp
#include <cmath>
#include <iostream>
#include "vector3D.h"
#include <chrono>


/* Include header files depending on platform */
#ifdef _WIN32
#include "GL\freeglut.h"
#define M_PI 3.14159
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif

using namespace std;

#define WINWIDTH 600
#define WINHEIGHT 400
#define NUM_OBJECTS 4
#define MAX_RT_LEVEL 50
#define NUM_SCENE 2

float* pixelBuffer = new float[WINWIDTH * WINHEIGHT * 3];

class Ray { // a ray that start with "start" and going in the direction "dir"
public:
	Vector3 start, dir;
};

class RtObject {

public:
	virtual double intersectWithRay(Ray, Vector3& pos, Vector3& normal) = 0; // return a -ve if there is no intersection. Otherwise, return the smallest postive value of t

																			 // Materials Properties
	double ambiantReflection[3];
	double diffusetReflection[3];
	double specularReflection[3];
	double speN = 300;


};
class Sphere : public RtObject {

	Vector3 center_;
	double r_;
public:
	Sphere(Vector3 c, double r) { center_ = c; r_ = r; };
	Sphere() {};
	void set(Vector3 c, double r) { center_ = c; r_ = r; };
	double intersectWithRay(Ray, Vector3& pos, Vector3& normal);
};

//Cylinder object
class Cylinder : public RtObject {

	Vector3 center_;
	//radius
	double r_;
	//height
	double h_;
public:
	Cylinder(Vector3 c, double r, double h) { center_ = c; r_ = r; h_ = h; };
	Cylinder() {};
	void set(Vector3 c, double r, double h) { center_ = c; r_ = r; h_ = h; };
	double intersectWithRay(Ray, Vector3& pos, Vector3& normal);
};

RtObject **objList; // The list of all objects in the scene
RtObject *tempObject;

					// Global Variables
					// Camera Settings
Vector3 cameraPos(0, 0, -500);

// assume the the following two vectors are normalised
Vector3 lookAtDir(0, 0, 1);
Vector3 upVector(0, 1, 0);
Vector3 leftVector(1, 0, 0);
float focalLen = 500;

// Light Settings

Vector3 lightPos(900, 1000, -1500);
double ambiantLight[3] = { 0.4,0.4,0.4 };
double diffusetLight[3] = { 0.7,0.7, 0.7 };
double specularLight[3] = { 0.5,0.5, 0.5 };


double bgColor[3] = { 0.1,0.1,0.4 };

int sceneNo = 0;
/*==============================
Modify Section Here
Hint: Add additional methods to help you in solving equations
==============================*/

//solve quadratic equation
double computeRoot(double a, double b, double c) {
	double root1, root2;

	double q = b*b - 4 * a*c;
	if (q <= 0)
		return -1;
	else {
		root1 = (-b - sqrt(q)) / (2 * a);
		root2 = (-b + sqrt(q)) / (2 * a);
		//return smaller root
		if (root1 * root2 < 0) return root2;
		else if (root1 > 0) return root1;
		else if (root1 == 0) return root2;
		else return -1;
	}

}

double Sphere::intersectWithRay(Ray r, Vector3& intersection, Vector3& normal)
// return a -ve if there is no intersection. Otherwise, return the smallest postive value of t
{// Step 1

	double alpha = dot_prod(r.dir, r.dir);
	double beta = 2 * dot_prod(r.dir, (r.start - center_));
	double gamma = dot_prod((r.start - center_), (r.start - center_))
		- r_ * r_;

	double root = computeRoot(alpha, beta, gamma);
	//update normal and intersection
	if (root != -1) {
		intersection = r.start + r.dir * root;
		normal = (*&intersection - center_) * 2;
		normal.normalize();
	}

	return root;
}

double Cylinder::intersectWithRay(Ray r, Vector3& intersection, Vector3& normal) {

	double alpha = pow(r.dir.x[0], 2) + pow(r.dir.x[2], 2);
	double beta = 2 * ((r.start.x[0] - center_.x[0])*r.dir.x[0]
		+ (r.start.x[2] - center_.x[2])*r.dir.x[2]);
	double gamma = pow((r.start.x[0] - center_.x[0]), 2)
		+ pow((r.start.x[2] - center_.x[2]), 2) - r_*r_;

	double root = computeRoot(alpha,beta,gamma);
	//update normal and intersection
	if (root != -1) {
		intersection = r.start + r.dir * root;
		if (intersection.x[1] > center_.x[1] + h_ || intersection.x[1] < center_.x[1] - h_) 
			return -1;
		normal = (intersection - center_) * 2;
		normal.normalize();
	}
	return root;
}

void addAnotherScene() {

	//Step 5: Modify objList here
	((Sphere*)objList[0])->set(Vector3(160, -40, 130), 180);
	((Sphere*)objList[1])->set(Vector3(140, 140, -50), 40);
	((Sphere*)objList[2])->set(Vector3(-70, 120, -80), 80);
	((Cylinder*)objList[3])->set(Vector3(-100, -80, 50), 60, 100);

	objList[0]->ambiantReflection[0] = 0.8;
	objList[0]->ambiantReflection[1] = 0.6;
	objList[0]->ambiantReflection[2] = 0.4;
	objList[0]->diffusetReflection[0] = 0.2;
	objList[0]->diffusetReflection[1] = 0.6;
	objList[0]->diffusetReflection[2] = 1;
	objList[0]->specularReflection[0] = 0.2;
	objList[0]->specularReflection[1] = 0.7;
	objList[0]->specularReflection[2] = 0.4;
	objList[0]->speN = 200;

	objList[1]->ambiantReflection[0] = 0.4;
	objList[1]->ambiantReflection[1] = 0.6;
	objList[1]->ambiantReflection[2] = 0.9;
	objList[1]->diffusetReflection[0] = 0.9;
	objList[1]->diffusetReflection[1] = 0.4;
	objList[1]->diffusetReflection[2] = 0.9;
	objList[1]->specularReflection[0] = 0.2;
	objList[1]->specularReflection[1] = 0.3;
	objList[1]->specularReflection[2] = 0.4;
	objList[1]->speN = 150;

	objList[2]->ambiantReflection[0] = 0.8;
	objList[2]->ambiantReflection[1] = 0.9;
	objList[2]->ambiantReflection[2] = 0.8;
	objList[2]->diffusetReflection[0] = 0.8;
	objList[2]->diffusetReflection[1] = 1;
	objList[2]->diffusetReflection[2] = 0.1;
	objList[2]->specularReflection[0] = 0.3;
	objList[2]->specularReflection[1] = 0.5;
	objList[2]->specularReflection[2] = 0.7;
	objList[2]->speN = 850;

	objList[3]->ambiantReflection[0] = 0.8;
	objList[3]->ambiantReflection[1] = 0.8;
	objList[3]->ambiantReflection[2] = 0.1;
	objList[3]->diffusetReflection[0] = 0.1;
	objList[3]->diffusetReflection[1] = 0.7;
	objList[3]->diffusetReflection[2] = 0.4;
	objList[3]->specularReflection[0] = 0.2;
	objList[3]->specularReflection[1] = 0.2;
	objList[3]->specularReflection[2] = 0.9;
	objList[3]->speN = 560;
}

//swap in/out obj 3 from list
void swapTempObject() {
	RtObject *temp = objList[3];
	objList[3] = tempObject;
	tempObject = temp;
}

void rayTrace(Ray ray, double& r, double& g, double& b, int fromObj = -1, int level = 0)
{
	// Step 4
	// Checks if it reaches MAX_RT_LEVEL
	if (level > MAX_RT_LEVEL)
	{
		r = 0;
		g = 0;
		b = 0;
		return;
	}
	int goBackGround = 1, i = 0;

	Vector3 intersection, normal;
	Vector3 rayVector;
	Vector3 lightVector;
	Vector3 viewVector;
	Vector3 reflectionVector;
	Vector3 rayReflectionVector;
	Vector3 transV;

	Ray reflectionRay;
	Ray shadowRay;

	//smallest t
	double mint = DBL_MAX;
	double t;
	//for ray tracing
	double r1, g1, b1;

	for (i = 0; i < NUM_OBJECTS; i++)
	{
		if ((t = objList[i]->intersectWithRay(ray, intersection, normal)) > 0)
		{
			//avoid reflecting itself
			if (fromObj == i) 
				continue;
			if (t >= mint)
				continue;

			//update min t
			mint = t;
			//to apply pie
			bool applyPIE = true;

			//Step 2 - apply ambiant
			r = objList[i]->ambiantReflection[0] * ambiantLight[0];
			g = objList[i]->ambiantReflection[1] * ambiantLight[1];
			b = objList[i]->ambiantReflection[2] * ambiantLight[2];

			//compute all vectors
			//L - direction unit vector from intersection to light source
			lightVector = lightPos - intersection;
			lightVector.normalize();
			//R = 2 (N*L)N-L
			reflectionVector = normal * dot_prod(normal, lightVector) * 2 - lightVector;
			reflectionVector.normalize();
			//V = direction unit vector to viewer
			viewVector = cameraPos - intersection;
			viewVector.normalize();

			//start of shadow is the intersection, facing toward light vector
			shadowRay.start = intersection;
			shadowRay.dir = lightVector;

			for (int j = 0; j < NUM_OBJECTS; j++)
				//shadow ray intersect
				if ((i != j && objList[j]->intersectWithRay(shadowRay, Vector3(), Vector3()) > 0) || (i == j && dot_prod(lightVector, normal) < 0)) 
					applyPIE = false;
			
			goBackGround = 0;

			if (applyPIE) {
				// Step 3 add diffuse and specular term of PIE
				r += objList[i]->diffusetReflection[0] * diffusetLight[0] * dot_prod(lightVector, normal)
					+ objList[i]->specularReflection[0] * specularLight[0] * pow(dot_prod(reflectionVector, viewVector), objList[i]->speN);
				g += objList[i]->diffusetReflection[1] * diffusetLight[1] * dot_prod(lightVector, normal)
					+ objList[i]->specularReflection[1] * specularLight[1] * pow(dot_prod(reflectionVector, viewVector), objList[i]->speN);
				b += objList[i]->diffusetReflection[2] * diffusetLight[2] * dot_prod(lightVector, normal)
					+ objList[i]->specularReflection[2] * specularLight[2] * pow(dot_prod(reflectionVector, viewVector), objList[i]->speN);
			}

			//compute reflection ray vector
			rayVector = ray.start - intersection;
			rayVector.normalize();
			rayReflectionVector = normal * dot_prod(normal, rayVector) * 2 - rayVector;
			rayReflectionVector.normalize();

			reflectionRay.start = intersection;
			reflectionRay.dir = rayReflectionVector;

			//recursively tracing ray
			r1 = g1 = b1 = 0;
			rayTrace(reflectionRay, r1, g1, b1, i, level + 1);
			//add reflection term
			r += r1 * objList[i]->specularReflection[0];
			g += g1 * objList[i]->specularReflection[1];
			b += b1 * objList[i]->specularReflection[2];

		}
	}

	if (goBackGround)
	{
		r = bgColor[0];
		g = bgColor[1];
		b = bgColor[2];
	}

}
/*==============================
End of Section,
IT IS NOT RECOMMENDED TO CHANGE ANYTHING BELOW
==============================*/



void drawInPixelBuffer(int x, int y, double r, double g, double b)
{
	pixelBuffer[(y*WINWIDTH + x) * 3] = (float)r;
	pixelBuffer[(y*WINWIDTH + x) * 3 + 1] = (float)g;
	pixelBuffer[(y*WINWIDTH + x) * 3 + 2] = (float)b;
}

void renderScene()
{
	int x, y;
	Ray ray;
	double r, g, b;

	cout << "Rendering Scene " << sceneNo << " with resolution " << WINWIDTH << "x" << WINHEIGHT << "........... ";
	__int64 time1 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the starting time

	ray.start = cameraPos;

	Vector3 vpCenter = cameraPos + lookAtDir * focalLen;  // viewplane center
	Vector3 startingPt = vpCenter + leftVector * (-WINWIDTH / 2.0) + upVector * (-WINHEIGHT / 2.0);
	Vector3 currPt;

	for (x = 0; x<WINWIDTH; x++)
		for (y = 0; y < WINHEIGHT; y++)
		{
			currPt = startingPt + leftVector*x + upVector*y;
			ray.dir = currPt - cameraPos;
			ray.dir.normalize();
			rayTrace(ray, r, g, b);
			drawInPixelBuffer(x, y, r, g, b);
		}

	__int64 time2 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the ending time

	cout << "Done! \nRendering time = " << time2 - time1 << "ms" << endl << endl;
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_DOUBLEBUFFER);
	glDrawPixels(WINWIDTH, WINHEIGHT, GL_RGB, GL_FLOAT, pixelBuffer);
	glutSwapBuffers();
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void setScene(int i = 0)
{
	if (i > NUM_SCENE)
	{
		cout << "Warning: Invalid Scene Number" << endl;
		return;
	}
	swapTempObject();
	if (i == 0)
	{
		
		((Sphere*)objList[0])->set(Vector3(-130, 80, 120), 100);
		((Sphere*)objList[1])->set(Vector3(130, -80, -80), 100);
		((Sphere*)objList[2])->set(Vector3(-130, -80, -80), 100);
		((Sphere*)objList[3])->set(Vector3(130, 80, 120), 100);

		objList[0]->ambiantReflection[0] = 0.1;
		objList[0]->ambiantReflection[1] = 0.4;
		objList[0]->ambiantReflection[2] = 0.4;
		objList[0]->diffusetReflection[0] = 0;
		objList[0]->diffusetReflection[1] = 1;
		objList[0]->diffusetReflection[2] = 1;
		objList[0]->specularReflection[0] = 0.2;
		objList[0]->specularReflection[1] = 0.4;
		objList[0]->specularReflection[2] = 0.4;
		objList[0]->speN = 300;

		objList[1]->ambiantReflection[0] = 0.6;
		objList[1]->ambiantReflection[1] = 0.6;
		objList[1]->ambiantReflection[2] = 0.2;
		objList[1]->diffusetReflection[0] = 1;
		objList[1]->diffusetReflection[1] = 1;
		objList[1]->diffusetReflection[2] = 0;
		objList[1]->specularReflection[0] = 0.0;
		objList[1]->specularReflection[1] = 0.0;
		objList[1]->specularReflection[2] = 0.0;
		objList[1]->speN = 50;

		objList[2]->ambiantReflection[0] = 0.1;
		objList[2]->ambiantReflection[1] = 0.6;
		objList[2]->ambiantReflection[2] = 0.1;
		objList[2]->diffusetReflection[0] = 0.1;
		objList[2]->diffusetReflection[1] = 1;
		objList[2]->diffusetReflection[2] = 0.1;
		objList[2]->specularReflection[0] = 0.3;
		objList[2]->specularReflection[1] = 0.7;
		objList[2]->specularReflection[2] = 0.3;
		objList[2]->speN = 650;

		objList[3]->ambiantReflection[0] = 0.3;
		objList[3]->ambiantReflection[1] = 0.3;
		objList[3]->ambiantReflection[2] = 0.3;
		objList[3]->diffusetReflection[0] = 0.7;
		objList[3]->diffusetReflection[1] = 0.7;
		objList[3]->diffusetReflection[2] = 0.7;
		objList[3]->specularReflection[0] = 0.6;
		objList[3]->specularReflection[1] = 0.6;
		objList[3]->specularReflection[2] = 0.6;
		objList[3]->speN = 650;

	}

	if (i == 1)
	{
		// Step 5
		addAnotherScene();
	}
}

void keyboard(unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {
	case 's':
	case 'S':
		sceneNo = (sceneNo + 1) % NUM_SCENE;
		setScene(sceneNo);
		renderScene();
		glutPostRedisplay();
		break;
	case 'q':
	case 'Q':
		exit(0);

	default:
		break;
	}
}

int main(int argc, char **argv)
{


	cout << "<<CS3241 Lab 5>>\n\n" << endl;
	cout << "S to go to next scene" << endl;
	cout << "Q to quit" << endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WINWIDTH, WINHEIGHT);

	glutCreateWindow("CS3241 Lab 5: Ray Tracing");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutKeyboardFunc(keyboard);

	objList = new RtObject*[NUM_OBJECTS];

	// create four spheres
	objList[0] = new Sphere(Vector3(-130, 80, 120), 100);
	objList[1] = new Sphere(Vector3(130, -80, -80), 100);
	objList[2] = new Sphere(Vector3(-130, -80, -80), 100);
	objList[3] = new Sphere(Vector3(130, 80, 120), 100);
	tempObject = new Cylinder(Vector3(-100, -80, 50), 60, 100);
	setScene(0);

	setScene(sceneNo);
	renderScene();

	glutMainLoop();

	for (int i = 0; i < NUM_OBJECTS; i++)
		delete objList[i];
	delete[] objList;

	delete[] pixelBuffer;

	return 0;
}
