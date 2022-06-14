#include <stdlib.h>
#include <stdio.h>
#include <glut.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <complex>
#include <list>
#include <iostream>
#include<string>

//------------------------------DATASEGMENT------------------------------//
const double PI = 3.14159265358979323846264338;
float XWinSize = 980, YWinSize = 980;
const int maxlen = 2;

float XLeft = -1 * maxlen, XRight = maxlen, YDown = -1 * maxlen, YUp = maxlen, ZNear = -1 * maxlen - 1, ZFar = maxlen + 1;
float x0 = 4, y00 = -3, z0 = 2,
xe = 0, ye = 0, ze = 0, xp = 0, yp = 0, zp = 10,
dx = 0, dy = 1, dz = 0,
x, y, z, t = 0, s = 0, scale = 1;
int Yscroll1 = -296;
int Yscroll2 = -296;
int Yscroll3 = -176;


const float init_shape_factor = 2;

float exponent_change = init_shape_factor;// A variable that --> changes the shape_factor variable once the "Enter" button has been pressed
int exponent_factor_int = 2;
float exponent_factor_float = 0;

float shape_factor = init_shape_factor;// the multiplication in the itterative complex equation (the usual one in mandelbrot is z = z^2+c)

int efact = 5.0;// exponent factor( the amount we will want to increase/decrease the exponent in the itirrated equation )

float alfa = 10;// the maximum value of alfa (the display angle).
int teta = 0;

GLuint initialmandel;
GLuint initialmandel_3D;
bool is_3D = false;

const int mand_arr_dim = 1121;

float initmandarr[mand_arr_dim][mand_arr_dim][5]; // A 3D array that it's fundemental component is an array of length 5
							  // which is built this way :      =======>{x axis position, y axis position, R, G, B}<=======
float mandarr[mand_arr_dim][mand_arr_dim][5];


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


void draw_string(void* font, const char* str);


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


void plan_mandel_dots(std::complex<float> c, float parmandarr[mand_arr_dim][mand_arr_dim][5]) {// z(n+1)=z(n)+
	std::complex<float> z(0.0, 0.0);//z is always supposed to be zero
	int m = mandelbrot(c);
	z = z * z + c;

	//coloring
	hsv hsv_color;
	hsv_color.h = 360.0 * m / (double)max_itter;
	hsv_color.s = 1;
	hsv_color.v = 0;
	if (m < max_itter)
		hsv_color.v = 1;

	//converting hsv to rgb
	rgb rgb_color = hsv2rgb(hsv_color);


	int arrplace1 = (int)(280.1121 * z.real()) + 560;
	int arrplace2 = (int)(280.1121 * z.imag()) + 560;
	parmandarr[arrplace1][arrplace2][0] = z.real();
	parmandarr[arrplace1][arrplace2][1] = z.imag();
	parmandarr[arrplace1][arrplace2][2] = rgb_color.r;
	parmandarr[arrplace1][arrplace2][3] = rgb_color.g;
	parmandarr[arrplace1][arrplace2][4] = rgb_color.b;
}



//--------------------------DRAWING_DOTS_FUNCTIONS---------------------------//
void draw_mandel_dots_3D(std::complex<float> c, float parmandarr[mand_arr_dim][mand_arr_dim][5]) {
	std::complex<float> z(0.0, 0.0);//z is always supposed to be zero
	int m = mandelbrot(c);
	z = z * z + c;


	//==================================================================//
	int arrplace1 = z.real() + 560;
	int arrplace2 = z.imag() + 560;

	glColor3f(parmandarr[arrplace1][arrplace2][2], parmandarr[arrplace1][arrplace2][3], parmandarr[arrplace1][arrplace2][4]);
	glBegin(GL_LINES);
	glVertex3f(parmandarr[arrplace1][arrplace2][0], parmandarr[arrplace1][arrplace2][1], 0);
	glVertex3f(parmandarr[arrplace1][arrplace2][0], parmandarr[arrplace1][arrplace2][1], parmandarr[arrplace1][arrplace2][2] / 10);
	glEnd();
}
//-------------------------------2D VERSION----------------------------------//
void draw_mandel_dots(std::complex<float> c, float parmandarr[mand_arr_dim][mand_arr_dim][5]) {
	std::complex<float> z(0.0, 0.0);//z is always supposed to be zero
	int m = mandelbrot(c);
	z = z * z + c;


	//==================================================================//
	int arrplace1 = z.real() + 560;
	int arrplace2 = z.imag() + 560;

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
	glutPostRedisplay();

	glClearColor(0.0, 0, 0.0, 0);
	gluPerspective(alfa, scale, ZNear, fabs(ZFar - ZNear));
	gluLookAt(xp, yp, zp, xe, ye, ze, dx, dy, dz);
	glRotatef(teta, 0, 1, 0);

	if (shape_factor == init_shape_factor && !is_3D)
		glCallList(initialmandel);
	else if (shape_factor == init_shape_factor && is_3D)
		glCallList(initialmandel_3D);

	else if (shape_factor != init_shape_factor && is_3D) {
		for (float k = -560; k < 560; k++) {
			mycomplex.real(k);
			for (float j = -560; j < 560; j++) {
				draw_mandel_dots_3D(mycomplex, mandarr);
				mycomplex.imag(j);
			}
		}
	}
	else {
		for (float k = -560; k < 560; k++) {
			mycomplex.real(k);
			for (float j = -560; j < 560; j++) {
				draw_mandel_dots(mycomplex, mandarr);
				mycomplex.imag(j);
			}
		}
	}
	glColor3f(1, 1, 1);


	glutSwapBuffers();
	glFlush();
}

void initial(void)
{
	printf("-------------------------------------------------------------------------------\n");
	printf("*operations on the equation Z = Z^exp +c:\nR/r - resets the exponent to it's default value(= 2)\nW/w, S/s - increases(W/w) or decreases(S/s) the exponent by 1\n");
	printf("Up arrow, Down arrow - increases(Up) or decreases(Down) the exponent by 0.1\n");
	printf("-------------------------------------------------------------------------------\n");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(XLeft, XRight, YDown, YUp, ZNear, ZFar);
	glMatrixMode(GL_MODELVIEW);
	initialmandel = glGenLists(1);
	initialmandel_3D = glGenLists(2);

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

	glNewList(initialmandel_3D, GL_COMPILE);
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
			draw_mandel_dots_3D(mycomplex, initmandarr);
			mycomplex.imag(j);
		}
	}
	glEndList();
}


void keyboard(unsigned char key, int x, int y) {
	glutPostRedisplay();
}

void display2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glutPostRedisplay();

	exponent_change = exponent_factor_int + exponent_factor_float;

	glPushMatrix();
	glColor3f(1, 1, 1);
	std::string p = std::to_string(exponent_change);
	glTranslatef(-80, 32, 1);
	glScalef(.32, .32, 2);
	
	//--------------DIGIT PRINTING--------------//
	int len = 0;
	for (int i = 0; i < sizeof(p) && p[i] != '.'; i++)
		len++;
	p.resize(len + 3);
	draw_string(GLUT_STROKE_MONO_ROMAN, p.c_str());
	glPopMatrix();
	glColor3f(1, 1, 1);
	/**/glPushMatrix();
	glTranslatef(-185, -349, 1);
	glScalef(.10, .10, 2);
	draw_string(GLUT_STROKE_MONO_ROMAN, "0");
	glPopMatrix();
	/**/glPushMatrix();
	glTranslatef(165, -349, 1);
	glScalef(.10, .10, 2);
	draw_string(GLUT_STROKE_MONO_ROMAN, "360");
	glPopMatrix();
	/**/glPushMatrix();
	glTranslatef(150, -316, 1);
	glScalef(.10, .10, 2);
	draw_string(GLUT_STROKE_MONO_ROMAN, "1");
	glPopMatrix();
	/**/glPushMatrix();
	glTranslatef(140, -40, 1);
	glScalef(.10, .10, 2);
	draw_string(GLUT_STROKE_MONO_ROMAN, "100");
	glPopMatrix();
	/**/glPushMatrix();
	glTranslatef(-174, -40, 1);
	glScalef(.10, .10, 2);
	draw_string(GLUT_STROKE_MONO_ROMAN, "0.99");
	glPopMatrix();
	/**/glPushMatrix();
	glTranslatef(-174, -316, 1);
	glScalef(.10, .10, 2);
	draw_string(GLUT_STROKE_MONO_ROMAN, "0.01");
	glPopMatrix();

	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);//scroller1 background
		glColor3f(0, 0, 0);
		glVertex3f(150, -300, 0);
		glVertex3f(150, -52, 0);
		glVertex3f(160, -52, 0);
		glVertex3f(160, -300, 0);
	glEnd();
	

	glBegin(GL_POLYGON);//scroller1
		glColor3f(1, 0, 0);
		glVertex3f(148, Yscroll1 - 8, 1);
		glVertex3f(162, Yscroll1 - 8, 1);
		glVertex3f(162, Yscroll1, 1);
		glVertex3f(148, Yscroll1, 1);
	glEnd();
	glColor3f(0, 0, 0);

	glBegin(GL_POLYGON);//scroller2 background
		glColor3f(0, 0, 0);
		glVertex3f(-150, -300, 0);
		glVertex3f(-150, -53, 0);
		glVertex3f(-160, -53, 0);
		glVertex3f(-160, -300, 0);
	glEnd();
	

	glBegin(GL_POLYGON);//scroller2
		glColor3f(1, 0, 0);
		glVertex3f(-148, Yscroll2 - 8, 1);
		glVertex3f(-162, Yscroll2 - 8, 1);
		glVertex3f(-162, Yscroll2, 1);
		glVertex3f(-148, Yscroll2, 1);
	glEnd();
	
	
	glBegin(GL_POLYGON);// Apply Button
		glColor3f(.75, 0, 0);
		glVertex3f(-32, -276, 1);
		glVertex3f(32, -276, 1);
		glVertex3f(32, -232, 1);
		glVertex3f(-32, -232, 1);
	glEnd();
	
	glBegin(GL_POLYGON);// Reset Button
		glColor3f(0, .75, 0);
		glVertex3f(-32, -126, 1);
		glVertex3f(32, -126, 1);
		glVertex3f(32, -82, 1);
		glVertex3f(-32, -82, 1);
	glEnd();
	
	glBegin(GL_POLYGON);// Z Dimansion Button
		glColor3f(1, 1, 0);
		glVertex3f(-32, -157, 1);
		glVertex3f(32, -157, 1);
		glVertex3f(32, -201, 1);
		glVertex3f(-32, -201, 1);
	glEnd();
	
	glBegin(GL_POLYGON);//angle scroller background
	glColor3f(0, 0, 0);
	glVertex3f(180, -330, 0);
	glVertex3f(180, -320, 0);
	glVertex3f(-180, -320, 0);
	glVertex3f(-180, -330, 0);
	glEnd();

	glBegin(GL_POLYGON);//angle scroller
	glColor3f(1, 0, 0);
	glVertex3f(Yscroll3+4, -332, 1);
	glVertex3f(Yscroll3-4, -332, 1);
	glVertex3f(Yscroll3-4, -318, 1);
	glVertex3f(Yscroll3+4, -318, 1);
	glEnd();


	glutSwapBuffers();
	glFlush();
}


void keyboard2(unsigned char key, int x, int y) {
	glutPostRedisplay();
}

void mouse2(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (x >= 437 && x <= 450 && y >= 564 && y <= 912)
		{
			Yscroll1 = (int)(494. - y) / 1.4;
			exponent_factor_int = (int)((Yscroll1 + 47.) / 2.5) + 101;
		}
		if (x >= 47 && x <= 58 && y >= 562 && y <= 912)
		{
			Yscroll2 = (int)(494. - y) / 1.4;
			exponent_factor_float = (((Yscroll2 + 47.) / 2.5) + 100) / 100;
		}if (x >= 25 && x <= 475 && y >= 938 && y <= 952)
		{
			Yscroll3 = (int)((x - 250.) / 1.25);
			teta = Yscroll3 + 180;
			printf("--teta = %d\n",teta);
		}
		//-------------------- scroll changers --------------------//

		if (/**/x >= 199 && x <= 290 && y >= 815 && y <= 875)
		{
			printf("Exponent has been reset\n");
			Yscroll1 = -296;
			Yscroll2 = -296;
			Yscroll3 = -176;
			teta = 0;
			shape_factor = init_shape_factor;
			exponent_factor_int = (int)init_shape_factor;
			exponent_factor_float = 0;
			is_3D = false;
		}
		if (/**/x >= 199 && x <= 290 && y >= 605 && y <= 665) 
		{
			shape_factor = exponent_change;
			printf("%f \n", shape_factor);
			for (float k = -560; k < 560; k++) {// remember : golden number = 0.00357 (golden number means (4/1120) and 1120 is the exact amount of dots you need to fill this entire thing)
				mycomplex.real(k * 0.00357);
				for (float j = -560; j < 560; j++) {
					plan_mandel_dots(mycomplex, mandarr);
					mycomplex.imag(j * 0.00357);
				}
			}
		}
		if (/**/x >= 199 && x <= 290 && y >= 710 && y <= 772) 
		{
			is_3D = !(is_3D);
		}


		printf("y = %d / %d\n", (int)((490.-y)/1.4), (int)y);
		printf("x = %d / ", (int)((x - 250.)/1.25));
		std::cout << x << "\n" << std::endl;
	}
	glutPostRedisplay();
}

void init2(void)
{
	glClearColor(0.266, 0.541, 0.717, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-200.0, 200.0, -350.0, 350.0, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	//---Mandelbrot Fractal:
	glutInitWindowPosition(XWinSize / 2 - 140, 20);
	glutInitWindowSize(XWinSize, YWinSize);
	glutCreateWindow("Mandelbrot Window");
	initial();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);//<<===========
	//glutIdleFunc(idle);
	//glutMouseFunc(mouseClick);

	glEnable(GL_DEPTH_TEST);

	//--------Control Panel:
	glutInitWindowPosition(XWinSize / 2 + 840, 20);
	glutInitWindowSize(500, 980);
	glutCreateWindow("Control Panel");
	glutDisplayFunc(display2);
	glutKeyboardFunc(keyboard);
	//glutSpecialFunc(specialKeys);
	glutMouseFunc(mouse2);

	init2();
	glutMainLoop();
}

void draw_string(void* font, const char* str) {
	while (*str)
	{
		glutStrokeCharacter(font, *str);
		str++;
	}
}
