#include "pch.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup.h"

static int success_count;
char canvas_Name[] = "Ring Toss"; // Name at the top of canvas
char message[] = "Enter Toss Velocity and then GO to Toss";
char velocity[] = "VELOCITY";
char go[] = "GO";


// Sets width and height of canvas to 480 by 480.
#define canvas_Width 550
#define canvas_Height 550

void init() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	success_count = 0;
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

	glutWireTorus(inner_radius, outer_radius, nsides, rings);
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


void display_func() {
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	print_message(-100, 260, -50);
	draw_velocity_box(-125,0,-50);
	draw_start_box(-95, -5, -50);
	draw_post(210, -275, -50, 10);
	draw_ring(-60, 200, -50, 20, 15);
	glFlush();
}

/************** MAIN FUNCTION **************/
int main(int argc, char ** argv) {
	// Setup preliminaries
	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);
	// Register display callback handler
	glutDisplayFunc(display_func);
	init();
	// Event processing loop
	glutMainLoop();
	return 0;
}