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
static int isAnimate;


static float t; // Time parameter.
static int h; // Horizontal component of initial velocity.
static float v; // Vertical component of initial velocity.
static float g;  // Gravitational accelaration.
string h_string;
static int score_val;

static float x_t;
static float y_t;

static float x_pos;
static float y_pos;

static int win;
static int near_win;

static float eyeY;
static float centerY;
static float centerZ;

char canvas_Name[] = "Ring Toss"; // Name at the top of canvas
char message[] = "Enter Toss Velocity and then GO to Toss";
char velocity[] = "VELOCITY";
char go[] = "GO";
char game_over[] = "GAME OVER";
char score[] = "SCORE:";
char success_msg[] = "CONGRATS! You have hit the POST";
char encouragement_msg[] = "VERY NEAR MISS! YOU CAN DO IT!!!";


static char theStringBuffer[10];
static char scoreBuffer[3];

static float square_color[3] = { 1.0, 0.0, 0.0 }; // Color of the square
static float eyeX = 0;
static float eyeZ = 0;
static float centerX = 0;

void animate(int value);


// Sets width and height of canvas to 480 by 480.
#define canvas_Width 550
#define canvas_Height 550

void init() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	success_count = 0;
	animationPeriod = 50;
	t = 0.0;
	h = 0;
	v = 10;
	g = 32;
	isAnimate = 0;
	view_state = 1;

	x_pos = 0;
	y_pos = 0;

	score_val = 0;
	win = 0;
	near_win = 0;
	eyeY = 0;
	centerY = 0;
	centerZ = -50;

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
		//gluLookAt(0, eyeY, 0, centerY, 0, -50, 0, 1, 0);
	}
	// Sets view volume to orthographic if view state is 1. 
	else {
		glOrtho(-275, 275, -275, 275, 1, 150);
	//	gluLookAt(0, 0, 0, 0, -275, -50, 0, 1, 0);
		//gluLookAt(0, 200, 0, 0, 255, -50, 0, 1, 0);
	}
	// Sets the current matrix mode to modelview to apply the subsequent operations on modelview mode.
	//gluLookAt(0,eyeY,0,0,0,-50,0,1,0);
	gluLookAt(eyeX, eyeY, 1, centerX, centerY, -50.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
}

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string) {
	char *c;
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void writeStrokeString(void *font, char *string) {
	char *c;
	for (c = string; *c != '\0'; c++) {
		glutStrokeCharacter(font, *c);
	}
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

void display_game_over(float x, float y, float z) {
	glPushMatrix();
	glRasterPos3f(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, game_over);
	glPopMatrix();
}

void draw_ring(float x, float y, float z, GLdouble inner_radius, GLdouble torus_width) {
	//if (x == 190 and y == -270) {
		//win = 1;
	//}
	GLdouble outer_radius = inner_radius + (torus_width / 2.0);
	GLint nsides = 25;
	GLint rings = 25;
	if (success_count == 1) {
		glColor3f(1.0, 0.647, 0.0);
	}
	else if (success_count == 2) {
		glColor3f(0.0, 1.0, 1.0);
	}

	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(90, 1, 0, 0);
	glutWireTorus(7.5, 27.5, nsides, rings);
	glPopMatrix();

	if (x >= 210 and x <= 220) {
		cout << "won:" << win << endl;
		win = 1;
	}
	cout << "win is : " << win << endl;
	if (win == 0) {
		cout << "winder uder: " << win << endl;
		if (x >= 140 and x <= 300 and win == 0) {
			near_win = 1;
		}
	}
	
}

// Routine to onvert floating point to char string.

void integerToString(char * destStr, int precision, int val) {
	sprintf(destStr, "%d", val);
	destStr[precision] = '\0';
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
	glVertex3f(x + 75, y, z);
	glVertex3f(x + 75, y + 25, z);
	glVertex3f(x, y + 25, z);
	glEnd();

	integerToString(theStringBuffer, 7, h);
	glTranslatef(x + 8, y - 15, z);
	glScalef(0.07, 0.07, 0);
	writeStrokeString(GLUT_STROKE_MONO_ROMAN, velocity);

	glTranslatef(x - 750, y + 320, z);
	glScalef(1.8, 1.8, 0);
	writeStrokeString(GLUT_STROKE_ROMAN, theStringBuffer);
	glPopMatrix();
}

void draw_go_box(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x + 25, y, z);
	glVertex3f(x + 25, y + 23, z);
	glVertex3f(x, y + 23, z);
	glEnd();
	glTranslatef(x + 5, y + 8, z);
	glScalef(0.08, 0.08, 0);
	writeStrokeString(GLUT_STROKE_MONO_ROMAN, go);
	glPopMatrix();
}

void display_score(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glTranslatef(x, y, z);
	glScalef(0.08, 0.08, 0);
	writeStrokeString(GLUT_STROKE_MONO_ROMAN, score);

	integerToString(theStringBuffer, 3, score_val);
	writeStrokeString(GLUT_STROKE_MONO_ROMAN, theStringBuffer);
	glPopMatrix();
}

void display_success_msg(float x, float y, float z) {
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glRasterPos3f(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, success_msg);
	glPopMatrix();
}

void display_encouragement_msg(float x, float y, float z) {
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glRasterPos3f(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, encouragement_msg);
	glPopMatrix();
}

// Timer function.
void animate(int value) {
	switch (value) {

	case 1:
		if (isAnimate == 1) {
			t += 1;
			glutPostRedisplay();
			glutTimerFunc(animationPeriod, animate, 1);
			//glutTimerFunc(2000, animate, 1);

		}
		break;
	case 2:
		success_count += 1;
		if (success_count <= 3 and win) {
			score_val += 25;
		}
		t = 0.0;
		//win = 0;
		//near_win = 0;
		//glutPostRedisplay();
		glutTimerFunc(1000, animate, 3);
		//win = 0;
		//near_win = 0;
		break;
	case 3:
		glutPostRedisplay();
		break;
	}

}

void keyboard_handler(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'V': case 'v':
		view_state = abs(view_state - 1);
		view_setup();
		glutPostRedisplay();
		break;
	case 'T': case 't':
		if (eyeY == 0) {
			eyeY = -270;
			view_setup();
			centerX = 0;
			centerY = 0;
			centerZ = -50;
			eyeX = 0;
			eyeZ = 0;
			
		}
		else {
			eyeY = 0;
			view_setup();
			centerX = 0;
			centerY = 0;
			eyeX = 0;
			eyeZ = 0;
			centerZ = -50;
		}
		
		view_setup();
		glutPostRedisplay();
		break;
	case 'r':
		isAnimate = 0;
		t = 0.0;
		glutPostRedisplay();
		break;
	default:
		break;
	}
	if (view_state == 1) {
		if (x >= 15 and x <= 87 and y >= 250 and y <= 274) {
			int key_in = int(key) - 48;
			if (key_in >= 0 and key_in <= 9) {
				h_string += key;
				if (h_string.length() <= 7) {
					h = stoi(h_string);
					glutPostRedisplay();
				}
			}
		}
		
	}
	else if (view_state == 0) {
		if (x >= 145 and x <= 161 and y >= 260 and y <= 272) {
			int key_in = int(key) - 48;
			if (key_in >= 0 and key_in <= 9) {
				h_string += key;
				if (h_string.length() <= 7) {
					h = stoi(h_string);
					glutPostRedisplay();
				}
			}
		}
	}
}

void mouse_handler(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON, state == GLUT_UP)
	{
		if (view_state == 1) {
			if (x >= 95 and x <= 107 and y >= 250 and y <= 272) {
				isAnimate = 1;
				glutPostRedisplay();
				glutTimerFunc(animationPeriod, animate, 1);
				
			}

			if (x >= 15 and x <= 87 and y >= 250 and y <= 274) {
				h_string = "";
				glutKeyboardFunc(keyboard_handler);
			}
		}
		else if (view_state == 0) {
			if (x >= 185 and x <= 195 and y >= 262 and y <= 270) {
				isAnimate = 1;
				glutTimerFunc(animationPeriod, animate, 1);
			}

			if (x >= 145 and x <= 161 and y >= 260 and y <= 272) {
				//cout << "here" << endl;
				h_string = "";
				glutKeyboardFunc(keyboard_handler);
			}
		}
	}
}


void display_func() {
	glLoadIdentity();
	
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	win = 0;
	near_win = 0;
	y_pos = v * t - (g / 2.0)*t*t;
	if (200 + y_pos >= -210) {
		x_pos = h * t;

	};

	if (200 + y_pos <= -270) {
		y_pos = -470;
		isAnimate = 0;
		glutTimerFunc(1000, animate, 2);
	}

	print_message(-100, 260, -50);
	draw_velocity_box(-130, 0, -50);
	draw_go_box(-90, 0, -50);
	draw_post(210, -275, -50, 10);
	display_score(-130, 130, -50);
	if (success_count < 3) {
		glColor3f(1.0, 1.0, 0.0);
		draw_ring(-60 + x_pos, 200 + y_pos, -50, 20, 15);
	}
	else {
		glColor3f(1.0, 0.0, 0.0);
		display_game_over(-60, 200, -50);
	}
	if (win) {
		display_success_msg(-270, -270, -50);
	}
	if (near_win) {
		display_encouragement_msg(-270, -270, -50);
	}
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