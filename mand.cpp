#include <stdlib.h>
#include <stdio.h>
#include <glut.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <complex>
#include <list>
#include <iostream>
#include<math.h>

//------------------------------DATASEGMENT------------------------------//
const double PI = 3.14159265358979323846264338;
float XWinSize = 980, YWinSize = 980;
const int maxlen = 2;

float XLeft = -1 * maxlen, XRight = maxlen, YDown = -1 * maxlen, YUp = maxlen, ZNear = -1 * maxlen, ZFar = maxlen;
float x0 = 4, y00 = -3, z0 = 2,
xe = 0, ye = 0, ze = 0, xp = 0, yp = 0, zp = 10,
dx = 0, dy = 1, dz = 0,
//dx = xe - x0, dy = ye - y00, dz = ze - z0,
x, y, z, t = 0, s = 0, scale = 1;

const float init_shape_factor = 2;

float shape_factor = init_shape_factor;// the multiplication in the itterative complex equation (the usual one in mandelbrot is z = z^2+c)

int efact = 5.0;// exponent factor( the amount we will want to increase/decrease the exponent in the itirrated equation )

float alfa = 10;// the maximum value of alfa (the display angle).

GLuint initialmandel;


float initmandarr[1121][1121][5]; // A 3D array that it's fundemental component is an array of length 5
							  // which is built this way :      =======>{x axis position, y axis position, R, G, B}<=======
float mandarr[1121][1121][5];


//------------------------------DATASEGMENT------------------------------//

//colors
typedef struct {
	double r;       // a fraction between 0 and 1
	double g;       // a fraction between 0 and 1
	double b;       // a fraction between 0 and 1
} rgb;


typedef struct {
	double h;       // angle in degrees
	double s;       // a fraction between 0 and 1
	double v;       // a fraction between 0 and 1
} hsv;

rgb hsv2rgb(hsv in)
{
	double      hh, p, q, t, ff;
	long        i;
	rgb         out;

	if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch (i) {
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;
}

//std::complex<float> powc(std::complex<float> comp1, float n) {// complex number to the power of n.
//	float r = abs(comp1);
//	float cTeta = 0;
//	if (comp1.imag() != 0)
//		float cTeta = atan(deg(comp1.imag()) / comp1.real());
//
//	comp1.real(powf(r, n) * cos(deg(cTeta * n)));
//	comp1.imag(powf(r, n) * sin(deg(cTeta * n)));
//	return comp1;
//}


void draw_string(void* font, const char* str);

//int color_set[][3] = { {0, 153, 255},{255, 51, 0},{255, 153, 255} ,{255, 255, 0}};
int color_set[][3] = { {0, 64, 255}, {230, 153, 255} };
float opopo = 0.25;
int max_itter = 80;
int max_itter_lim = 20;
std::complex<float> mycomplex(0.0, 0.0);


//----------------------------------------------------------------------//

void keyboard(unsigned char key, int x, int y);

int mandelbrot(std::complex<double> c) {// z(n+1)=z(n)^2+c
	std::complex<double> z(0, 0);
	int n = 0;
	while ((fabs(z.real()) < 2 && fabs(z.imag()) < 2) && n < max_itter) {
		z = pow(z, shape_factor) + c;
		n++;
	}
	return n;
}


void plan_mandel_dots(std::complex<float> c, float parmandarr[1121][1121][5]) {// z(n+1)=z(n)+
	std::complex<float> z(0.0, 0.0);//z is always supposed to be zero
	int m = mandelbrot(c);
	z = z * z + c;
	int len = sizeof(color_set) / sizeof(color_set[0]);


	//coloring
	hsv hsv_color;
	hsv_color.h = 360.0 * m / (double)max_itter;
	//std::cout << hsv_color.h << std::endl;
	hsv_color.s = 1;
	hsv_color.v = 0;
	if (m < max_itter)
		hsv_color.v = 1;

	//converting hsv to rgb
	rgb rgb_color = hsv2rgb(hsv_color);


	int arrplace1 = (int)(280.1121 * z.real()) + 560;
	int arrplace2 = (int)(280.1121 * z.imag()) + 560;

	/*if (arrplace1 >= 1200)
		printf("%i, %i\n", arrplace1, arrplace2);*/
	parmandarr[arrplace1][arrplace2][0] = z.real();
	parmandarr[arrplace1][arrplace2][1] = z.imag();
	parmandarr[arrplace1][arrplace2][2] = rgb_color.r;
	parmandarr[arrplace1][arrplace2][3] = rgb_color.g;
	parmandarr[arrplace1][arrplace2][4] = rgb_color.b;

	//printf("%i, %i\n", arrplace1, arrplace2);
	//glColor3f(rgb_color.r, rgb_color.g, rgb_color.b);


	//glBegin(GL_POINTS);
	//glVertex3f(z.real(), z.imag(), 0);
	//glEnd();
}



//--------------------------DRAWING_DOTS_FUNCTION---------------------------//
void draw_mandel_dots(std::complex<float> c, float parmandarr[1121][1121][5]) {
	std::complex<float> z(0.0, 0.0);//z is always supposed to be zero
	int m = mandelbrot(c);
	z = z * z + c;


	//==================================================================//
	int arrplace1 = z.real() + 560;
	int arrplace2 = z.imag() + 560;

	//printf("%i, %i\n",arrplace1, arrplace2);
	glColor3f(parmandarr[arrplace1][arrplace2][2], parmandarr[arrplace1][arrplace2][3], parmandarr[arrplace1][arrplace2][4]);
	glBegin(GL_POINTS);
	glVertex3f(parmandarr[arrplace1][arrplace2][0], parmandarr[arrplace1][arrplace2][1], 0);
	glEnd();
}
//--------------------------------------------------------------------------//


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glClearColor(0.0, 0, 0.0, 0);
	gluPerspective(alfa, scale, 0.1, fabs(ZFar - ZNear));
	gluLookAt(xp, yp, zp, xe, ye, ze, dx, dy, dz);


	if(shape_factor == init_shape_factor)
		glCallList(initialmandel);
	else {
		for (float k = -560; k < 560; k++) {
			mycomplex.real(k);
			for (float j = -560; j < 560; j++) {
				draw_mandel_dots(mycomplex, mandarr);
				mycomplex.imag(j);
			}
		}
	}

	
	//mycomplex.real(0);
	//mycomplex.imag(0);
	//std::complex<float> check1 = mycomplex;
	//check1 = powc(check1, 2);
	//printf("%f+%f*i\n", check1.real(),check1.imag());

	glutPostRedisplay();
	glutSwapBuffers();
	glFlush();
}

void initial(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(XLeft, XRight, YDown, YUp, ZNear, ZFar);
	glMatrixMode(GL_MODELVIEW);
	initialmandel = glGenLists(1);
	glNewList(initialmandel, GL_COMPILE);

	for (float k = -560; k < 560; k++) {// remember : golden number = 0.00357 (golden number means (4/1120) and 1120 is the exact amount of dots you need in order to fill this entire thing)
		mycomplex.real(k * 0.00357);
		for (float j = -560; j < 560; j++) {
			plan_mandel_dots(mycomplex, initmandarr);
			mycomplex.imag(j * 0.00357);
		}
	}

	for (float k = -560; k < 560; k++) {
		mycomplex.real(k);
		for (float j = -560; j < 560; j++) {
			draw_mandel_dots(mycomplex, initmandarr);
			mycomplex.imag(j);
		}
	}
	glEndList();
}

void changeCoordinates(float Cx, float Cy)
{
	float newxp = (Cx - XWinSize / 2) / fabs(XWinSize / maxlen);
	float newyp = -(Cy - YWinSize / 2) / fabs(XWinSize / maxlen);
	xp, yp, xe, ye = 0;
}



void keyboard(unsigned char key, int x, int y) {
	if (tolower(key) == 'w')
	{
		printf("w");
		shape_factor++;
	}
	if (tolower(key) == 's')
	{
		printf("s");
		shape_factor--;
	}
	//----------RESETS THE VALUE TO 2----------//
	if (tolower(key) == 'r')
	{
		printf("%c",key);
		shape_factor = init_shape_factor;
	}
	//-----------------------------------------//
	printf("%f \n",shape_factor);
	if (tolower(key) == 's' || tolower(key) == 'w') {
		printf("s");
		for (float k = -560; k < 560; k++) {// remember : golden number = 0.00357 (golden number means (4/1120) and 1120 is the exact amount of dots you need to fill this entire thing)
			mycomplex.real(k * 0.00357);
			for (float j = -560; j < 560; j++) {
				plan_mandel_dots(mycomplex, mandarr);
				mycomplex.imag(j * 0.00357);
			}
		}
	}
}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(XWinSize / 2 - 140, 20);
	glutInitWindowSize(XWinSize, YWinSize);
	glutCreateWindow("Mandelbrot");
	initial();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);//<<===========
	//glutSpecialFunc(Special);
	//glutIdleFunc(idle);
	//glutMouseFunc(mouseClick);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
}

void draw_string(void* font, const char* str) {
	while (*str)
	{
		glutStrokeCharacter(font, *str);
		str++;
	}
}
