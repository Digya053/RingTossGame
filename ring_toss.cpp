#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup.h"

using namespace std;

// Globals variables
static int view_state; // Flag to keep track of the type of view volume currently in display 
						// [Ortho view = 1, Perspective = 0]

static int success_count;
static int animationPeriod; // Time interval between frames

static float t; // Time parameter.
static int h; // Horizontal component of initial velocity.
static float v; // Vertical component of initial velocity.
static float g;  // Gravitational accelaration.
string h_string;

char canvas_Name[] = "Ring Toss"; // Name at the top of canvas
char message[] = "Enter Toss Velocity and then GO to Toss";
char velocity[] = "VELOCITY";
char go[] = "GO";

static char theStringBuffer[10];


// Sets width and height of canvas to 480 by 480.
#define canvas_Width 550
#define canvas_Height 550

void init() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	success_count = 0;
	animationPeriod = 50;
	t = 0.0;
	h = 0000;
	v = 4;
	g = 0.2;
}

void view_setup(void) {
	/*
	This function sets up the view volume.
	*/

	// Set the current matrix mode to projection
	glMatrixMode(GL_PROJECTION);
	// Load identity matrix
	glLoadIdentity();
	// Sets view volume to perspective if view state is 0. The aspect ratio of perspective projection
	//is computed as 116.
	if (view_state == 0)
	{
		gluPerspective(159.4, canvas_Width / canvas_Height, 1.0, 150.0);
	}
	// Sets view volume to orthographic if view state is 1. 
	else {
		glOrtho(- 275, 275, -275, 275, 1, 150);
	}
	// Sets the current matrix mode to modelview to apply the subsequent operations on modelview mode.
	glMatrixMode(GL_MODELVIEW);
}

void draw_single_post_stack(float x, float y, float z, GLdouble cube_size) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(30, 0.0, 1.0, 0.0);
	glutWireCube(cube_size);
	glPopMatrix();
}

void draw_post(float x, float y, float z, GLdouble cube_size) {
	draw_single_post_stack(x, y, z, cube_size);
	draw_single_post_stack(x, y + 10, z, cube_size);
	draw_single_post_stack(x, y + 20, z, cube_size);
	draw_single_post_stack(x, y + 30, z, cube_size);
}

void draw_ring(float x, float y, float z, GLdouble inner_radius, GLdouble torus_width) {
	GLdouble outer_radius = inner_radius + (torus_width / 2.0);
	GLint nsides = 25;
	GLint rings = 25;
	if (success_count == 1) {
		glColor3f(1.0, 0.647, 0.0);
	}
	else if (success_count == 2) {
		glColor3f(0.0, 1.0, 1.0);
	}
	else {
		glColor3f(1.0, 1.0, 0.0);
	}
	glPushMatrix();
	glTranslatef(-60, 200, -50);
	glTranslatef(h*t, v*t - (g / 2.0)*t*t, 0.0);
	glutWireTorus(inner_radius, outer_radius, nsides, rings);
	glPopMatrix();
}

// Routine to onvert floating point to char string.

void integerToString(char * destStr, int precision, int val)

{
	sprintf(destStr, "%d", val);
	destStr[precision] = '\0';
}



// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string) {
	char *c;
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void print_message(float x, float y, float z) {
	glPushMatrix();
	glRasterPos3f(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, message);
	glPopMatrix();
}

void draw_velocity_box(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x+45, y, z);
	glVertex3f(x + 45, y+30, z);
	glVertex3f(x, y+30, z);
	glEnd();
	integerToString(theStringBuffer, 4, h);
	glRasterPos3f(x + 5, y +10, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_18, theStringBuffer);
	glRasterPos3f(x-2, y-10, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_10, velocity);
	glPopMatrix();
}

void draw_start_box(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x + 25, y, z);
	glVertex3f(x + 25, y + 23, z);
	glVertex3f(x, y + 23, z);
	glEnd();
	glRasterPos3f(x + 5, y + 8, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_10, go);
	glPopMatrix();
}

// Timer function.
void animate(int value) {
	t += 1.0;
	glutPostRedisplay();
	glutTimerFunc(animationPeriod, animate, 1);
}

void keyboard_handler(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'V': case 'v':
		view_state = abs(view_state - 1);
		view_setup();
		glutPostRedisplay();
		break;
	default:
		break;
	}
	if (x >= 31 and x <= 63 and y >= 246 and y <= 264) {
		int key_in = int(key) - 48;
		if (key_in >= 0 and key_in <= 9) {
			h_string += key;
			if (h_string.length() <= 4) {
				h = stoi(h_string);
				glutPostRedisplay();
			}
		}
	}
}

void mouse_handler(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON, state == GLUT_UP)
	{
		if (x > 83 and x < 110 and y > 260 and y < 280) {
			t += 1.0;
			glutPostRedisplay();
			glutTimerFunc(animationPeriod, animate, 1);
		}

		if (x >= 31 and x <= 63 and y >= 246 and y <= 264) {
			h_string = "";
			glutKeyboardFunc(keyboard_handler);
		}
	}
}





void display_func() {
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	print_message(-100, 260, -50);
	draw_velocity_box(-125,0,-50);
	draw_start_box(-95, -5, -50);
	draw_post(210, -275, -50, 10);
	draw_ring(-60, 200, -50, 20, 15);
	glutSwapBuffers();
	glFlush();
}

/************** MAIN FUNCTION **************/
int main(int argc, char ** argv) {
	// Setup preliminaries
	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);
	// Register display callback handler
	glutDisplayFunc(display_func);
	glutMouseFunc(mouse_handler);
	glutKeyboardFunc(keyboard_handler);
	init();
	// Event processing loop
	glutMainLoop();
	return 0;
}
