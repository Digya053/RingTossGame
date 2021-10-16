/*CS 445/545 Prog 3 for Digya Acharya*/
/***********************************************************************************************
ringtossgame.cpp
EXTRA CREDIT: All three extra credit features have been implemented, i.e, keeping SCORE on the upper left corner of the screen and incrementing it
by 25 if the ring lands on the post, displaying message of success on a successful ring toss and message of encouragement if it's a near miss and lastly,
viewing the objects from above the post triggered by the press of 'T'.
Apart from this, I have also displayed an extra message to encourage the player to try again on an unsuccessful ring toss.
Software Architecture Statement: This program implements a ring toss game that allows the  users to enter the velocity for tossing a ring into the peg by
registering four callback functions: display_func() which is a display callback handler, timer_func() which is a timer callback handler, keyboard_handler()
which is a keyboard callback handler and mouse_handler() which is a glut mouse callback handler. The program uses double buffered display mode.
After drawing all objects using the display_func(), the coordinates for enabling press of 'GO' button and typing in the velocity box has been found using the
mouse_handler(). Then, keyboard_handler() has been used to get the value of horizontal velocity typed in the obtained mouse coordinate position. The key
strings typed has been stored in 'hstring' global string variable, converted to global integer 'h' to use in gravitational equation and then, to character,
theStringBuffer (global variable) to display into the screen. The value of h which enables the ring to successfully land on post has been found to be
from 51 units/sec to 57 units/sec.
The view_setup function has been used to change the viewing model and to change the camera position using gluLookAt(). The global variable 'eyeY' has been
changed to adjust the camera position.
Other major global variables => x_pos and y_pos are the major float global variables used for making ring move following gravitational equation (implemented
in calc_gravity_based_ring_pos()). It has also been used to check the win position (implemented in check_win_position()). The 'play_count' variable has been
used to keep track of the current game number required for changing the color of the ring, 'score_val' has been used to keep track of the score, 'isAnimate'
has been used to stop animation after reaching the end position. The variables - win, near_win and not_win has been used to track the game status and print
messaage in the screen.
************************************************************************************************/

#define _CRT_SECURE_NO_WARNINGS	// used for sprintf function to change the integer typed in velocity box to char so as to display the text in the screen

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup.h"

// Globals variables
static float x_pos;			// Stores x position movement based on gravity
static float y_pos;			// Stores y position movement based on gravity

static int view_state;		// Flag to keep track of the type of view volume currently in display 
							// [Ortho view = 1, Perspective = 0]

static int animationPeriod; // Time interval between frames
static int delayPeriod;		// Delay time after which to restart the game with a different colored ring
static int isAnimate;		// Keeps track of animation status

static float t;				 // Time parameter.
static int h;				 // Horizontal component of initial velocity.

static int score_val;		 // Keeps score count
static int play_count;		 // Keeps track of current game number

static int win;				 // Keeps track of win, near win and not win status
static int near_win;
static int not_win;

static float eyeY;			 // for changing camera position

std::string h_string;		 // for storing the integers typed
static char theStringBuffer[10];	// Stores the character needed to display in the screen 

// For displaying text on canvas
char canvas_Name[] = "Ring Toss Game"; // Name at the top of canvas
char message[] = "Enter Toss Velocity and then GO to Toss";
char velocity[] = "VELOCITY";
char go[] = "GO";
char game_over[] = "GAME OVER";
char score[] = "SCORE:";
char success_msg[] = "CONGRATS! You have hit the POST";
char encouragement_msg[] = "VERY NEAR MISS! YOU CAN DO IT!!!";
char try_again_msg[] = "The RING didn't hit the POST. Please try again!!!";

// Sets width and height of canvas to 550 by 550.
#define canvas_Width 550
#define canvas_Height 550

void init(void) {
	/* This function sets the background color and initializes all the global variables.*/
	glClearColor(0.0, 0.0, 0.0, 1.0);
	play_count = 1;
	animationPeriod = 50;	// Set the time to display new frame to 50
	delayPeriod = 1000;		// Time to start a new game
	t = 0.0;
	h = 0;
	isAnimate = 0;			// Animation is disabled at the start
	view_state = 1;			// Set the view volume to orthographic

	x_pos = 0;
	y_pos = 0;

	score_val = 0;
	// As the camera position is at the origin in the beginning, set eyeY(position of camera y) to 0
	eyeY = 0;

}

void writeBitmapString(void *font, char *string) {
	/*
	This function writes a bitmap text, one character at a time.
	Parameters:
	----------
		font: void pointer
			The font to use to write a text.
		string: char pointer
			The text to write.
	*/
	char *c;
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void writeStrokeString(void *font, char *string) {
	/*
	This function writes a stroke text, one character at a time.
	Parameters:
	----------
		font: void pointer
			The font to use to write a text.
		string: char pointer
			The text to write.
	*/
	char *c;
	for (c = string; *c != '\0'; c++) {
		glutStrokeCharacter(font, *c);
	}
}

void integerToString(char * destStr, int precision, int val) {
	/*
	This function convert a integer to char string.
	Parameters:
	----------
		destStr: char pointer
			The name of the string to store the converted integer
		precision: Integer
			The number of characters
		val: Integer
			The value to write
	*/
	sprintf(destStr, "%d", val);
	destStr[precision] = '\0';
}

void draw_single_post_stack(float x, float y, float z, double cube_size) {
	/*
	This function draws a single cube of a peg using glutWireCube.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where a cube should be drawn
		y: Float
			The y-coordinate of the position where a cube should be drawn
		z: Float
			The z-coordinate of the position where a cube should be drawn
		cube_size: Double
			The size of the cube
	*/
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(30, 0.0, 1.0, 0.0);
	glutWireCube(cube_size);
	glPopMatrix();
}

void draw_post(float x, float y, float z, double cube_size) {
	/*
	This function draws entire post using draw_single_post_stack() four times.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where a post should be drawn
		y: Float
			The y-coordinate of the position where a post should be drawn
		z: Float
			The z-coordinate of the position where a post should be drawn
		cube_size: Double
			The size of the cube
	*/
	draw_single_post_stack(x, y, z, cube_size);
	draw_single_post_stack(x, y + 10, z, cube_size);
	draw_single_post_stack(x, y + 20, z, cube_size);
	draw_single_post_stack(x, y + 30, z, cube_size);
}

void draw_ring(float x, float y, float z, double width, double diameter) {
	/*
	This function draws a ring using glutWireTorus. The innerRadius and outerRadius are calculated using width and diameter. The color of the ring is
	changed in each new game start. The variable play_count is used to keep trak of the game number.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where the ring should be drawn
		y: Float
			The y-coordinate of the position where the ring should be drawn
		z: Float
			The z-coordinate of the position where the ring should be drawn
		width: Double
			The width of the torus
		diameter: Double
			The diameter of the inner circle of torus
	*/
	int nsides = 25;
	int rings = 25;
	// calculate innerRadius and outerRadius
	double innerRadius = width / 2;
	double outerRadius = innerRadius + diameter / 2;

	// change color
	if (play_count == 1) {
		glColor3d(1.0, 1.0, 0.0);
	}
	else if (play_count == 2) {
		glColor3d(1.0, 0.647, 0.0);
	}
	else if (play_count == 3) {
		glColor3d(0.0, 1.0, 1.0);
	}

	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(90, 1, 0, 0);
	glutWireTorus(innerRadius, outerRadius, nsides, rings);
	glPopMatrix();
}

void draw_velocity_box(float x, float y, float z) {
	/*
	This function draws the velocity box using GL_LINE_LOOP, convert the text inside the box to character and use glutStrokeCharacter to display it.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where a box should be drawn
		y: Float
			The y-coordinate of the position where a box should be drawn
		z: Float
			The z-coordinate of the position where a box should be drawn
	*/

	glPushMatrix();
	glTranslatef(x, y, z);
	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x + 75, y, z);
	glVertex3f(x + 75, y + 25, z);
	glVertex3f(x, y + 25, z);
	glEnd();

	// converts the value of horizontal velocity to character and displays it in the canvas
	integerToString(theStringBuffer, 7, h);
	glTranslatef(x + 8, y - 15, z);
	glScaled(0.07, 0.07, 0);
	writeStrokeString(GLUT_STROKE_MONO_ROMAN, velocity);

	glTranslatef(x - 750, y + 320, z);
	glScaled(1.8, 1.8, 0);
	writeStrokeString(GLUT_STROKE_ROMAN, theStringBuffer);
	glPopMatrix();
}

void draw_go_box(float x, float y, float z) {
	/*
	This function draws the go box using GL_LINE_LOOP and writes 'GO' inside the box using stroke.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where a box should be drawn
		y: Float
			The y-coordinate of the position where a box should be drawn
		z: Float
			The z-coordinate of the position where a box should be drawn
	*/
	glPushMatrix();
	glTranslatef(x, y, z);
	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x + 25, y, z);
	glVertex3f(x + 25, y + 23, z);
	glVertex3f(x, y + 23, z);
	glEnd();
	glTranslatef(x + 5, y + 8, z);
	glScaled(0.08, 0.08, 0);
	writeStrokeString(GLUT_STROKE_MONO_ROMAN, go);
	glPopMatrix();
}

void display_score(float x, float y, float z) {
	/*
	This function writes 'SCORE' on the upper left corner, convertes the integer value of score to character and displays the score using
	glutStrokeCharacter.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where a score should be written
		y: Float
			The y-coordinate of the position where a score should be written
		z: Float
			The z-coordinate of the position where a score should be written
	*/
	glPushMatrix();
	glTranslatef(x, y, z);
	glTranslatef(x, y, z);
	glScaled(0.08, 0.08, 0);
	writeStrokeString(GLUT_STROKE_MONO_ROMAN, score);

	integerToString(theStringBuffer, 3, score_val);
	writeStrokeString(GLUT_STROKE_MONO_ROMAN, theStringBuffer);
	glPopMatrix();
}

void display_game_over(float x, float y, float z) {
	/*
	This function writes game over on the same position where a ring is drawn using glutBitmapCharacter.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where a text should be written
		y: Float
			The y-coordinate of the position where a text should be written
		z: Float
			The z-coordinate of the position where a text should be written
	*/
	glColor3f(1.0, 0.0, 0.0);
	glPushMatrix();
	glRasterPos3f(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, game_over);
	glPopMatrix();
}

void print_game_instruction(float x, float y, float z) {
	/*
	This function displays game instruction at the top position of the screen using glutBitmapCharacter.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where a text should be written
		y: Float
			The y-coordinate of the position where a text should be written
		z: Float
			The z-coordinate of the position where a text should be written
	*/
	glPushMatrix();
	glRasterPos3f(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, message);
	glPopMatrix();
}

void display_success_msg(float x, float y, float z) {
	/*
	This function displays success message at the bottom left of the screen using glutBitmapCharacter.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where a text should be written
		y: Float
			The y-coordinate of the position where a text should be written
		z: Float
			The z-coordinate of the position where a text should be written
	*/
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glRasterPos3f(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, success_msg);
	glPopMatrix();
}

void display_encouragement_msg(float x, float y, float z) {
	/*
	This function displays encouragement message at the bottom left of the screen using glutBitmapCharacter.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where a text should be written
		y: Float
			The y-coordinate of the position where a text should be written
		z: Float
			The z-coordinate of the position where a text should be written
	*/
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glRasterPos3f(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, encouragement_msg);
	glPopMatrix();
}

void display_tryagain_msg(float x, float y, float z) {
	/*
	This function displays try again message at the bottom left of the screen using glutBitmapCharacter.
	Parameters:
	----------
		x: Float
			The x-coordinate of the position where a text should be written
		y: Float
			The y-coordinate of the position where a text should be written
		z: Float
			The z-coordinate of the position where a text should be written
	*/
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glRasterPos3f(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, try_again_msg);
	glPopMatrix();
}

void check_win_position(float x, float y) {
	/*
	This function checks if the post coordinates lies inside the ring coordinates.
	Parameters:
	----------
		x: Float
			The x coordinate determined by the equation of gravity
		y: Float
			The y coordinate determined by the equation of gravity
	*/
	win = 0;
	near_win = 0;
	not_win = 0;
	// if the post is inside the ring
	if (x >= 255 and x <= 285 and y == -470) {
		win = 1;
	}
	// if the post is near to the ring
	else if (x >= 218 and x <= 325 and y == -470 and !win) {
		near_win = 1;
	}
	// if the post is far away from the ring
	else if (y == -470 and !win and !near_win) {
		not_win = 1;
	}
}

void view_setup(void) {
	/*
	This function sets up the view volume and reposition the camera.
	*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Sets view volume to perspective if view state is 0.
	if (view_state == 0)
	{
		gluPerspective(159.4, canvas_Width / canvas_Height, 1.0, 150.0);
	}
	// Sets view volume to orthographic if view state is 1. 
	else {
		glOrtho(-275, 275, -275, 275, 1, 150);
	}
	// Set the camera position (position is at the origin at the start)
	gluLookAt(0.0, eyeY, 0.0, 0.0, 0.0, -50.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
}

void timer_func(int value) {
	/*
	This function is a glut timer callback function which displays each new frame 55 ms from start to enable ring movement,
	displays new score value and different colored ring at the start position based on the play_count and score_val.
	Parameters:
	-----------
		value: Integer
			This is the timerID.
	*/
	switch (value) {

	case 1:
		// enables ring movement
		if (isAnimate == 1) {
			t += 1;
			glutPostRedisplay();
			glutTimerFunc(animationPeriod, timer_func, 1);

		}
		break;
	case 2:
		// only increase play_count if ring s at the end position
		if (y_pos == -470) {
			play_count += 1;
		}
		// only increase score if it's the win position and value of score is less than 75
		if (win and score_val < 75) {
			score_val += 25;
		}
		t = 0.0;
		glutPostRedisplay();
		break;
	}

}

void display_typed_velocity(unsigned char key) {
	/*
	This function displays the key typed in the velocity box.
	Parameters:
	-----------
		key: unsigned char
			This is the key typed.
	*/
	int key_in = int(key) - 48;
	//only enable integer
	if (key_in >= 0 and key_in <= 9) {
		h_string += key;
		if (h_string.length() <= 7) {
			// use stoi to convert string to integer
			h = stoi(h_string);
			glutPostRedisplay();
		}
		else {
			h_string = "";
		}
	}
}

void keyboard_handler(unsigned char key, int x, int y) {
	/*
	This function is a glut keyboard callback handler. Whenever a key is pressed, it gets stored in a key
	variable. The switch condition matches the corresponding key condition and perform necessary
	computation.
	Parameters:
	-----------
		key: unsigned char
			The key input received
		x: Integer
			Mouse x-coordinate
		y: Integer
			Mouse y-coordinate
	*/
	switch (key)
	{
	case 'V': case 'v':
		// Toggle the view volume
		view_state = abs(view_state - 1);
		view_setup();
		glutPostRedisplay();
		break;
	case 'T': case 't':
		// Toggle the camera position
		if (eyeY == 0) {
			// change the value of eyeY for positioning the camera in upper direction
			eyeY = -275;

		}
		else {
			eyeY = 0;
		}
		view_setup();
		glutPostRedisplay();
		break;
	default:
		break;
	}
	if (view_state == 1) {
		// detect if the mouse position is inside the velocity box
		if (x >= 15 and x <= 87 and y >= 250 and y <= 274) {
			display_typed_velocity(key);
		}

	}
	else if (view_state == 0) {
		if (x >= 145 and x <= 161 and y >= 260 and y <= 272) {
			display_typed_velocity(key);
		}
	}
}

void enable_go_button(void) {
	/*
	This function enables ring movement when go button in clicked.
	*/
	isAnimate = 1;
	glutTimerFunc(animationPeriod, timer_func, 1);
}

void enable_velocity_type(void) {
	/*
	This function enables the keyboard callback handler to activate typing within the velocity box.
	*/
	h_string = "";
	glutKeyboardFunc(keyboard_handler);
}

void mouse_handler(int button, int state, int x, int y) {
	/*
	This function is a glut mouse callback handler which activates the ring movement after clicking go button and enables typing
	in the velocity box within certain mouse pointer coordinates. Default values of button and state has been used in this function.
	Parameters:
	-----------
		button: Integer
			Stores the button pressed in the mouse
		state: Integer
			Stores the state of the press
		x: Integer
			Stores the x-coordinate of the mouse pointer
		y: Integer
			Stores the y-coordinate of the mouse pointer
	*/
	if (view_state == 1) {
		if (x >= 95 and x <= 119 and y >= 250 and y <= 272 and h != 0) {
			// enable ring movement
			enable_go_button();
		}

		if (x >= 15 and x <= 87 and y >= 250 and y <= 274) {
			// enable typing in the velocity box
			enable_velocity_type();
		}
	}
	else if (view_state == 0) {
		if (x >= 185 and x <= 195 and y >= 262 and y <= 270 and h != 0) {
			enable_go_button();
		}

		if (x >= 145 and x <= 161 and y >= 260 and y <= 272) {
			enable_velocity_type();
		}
	}
}

void calc_gravity_based_ring_pos(void) {
	/*
	This function calculates the x_pos and y_pos value based on gravitational equation. This also make ring fall straight down
	when y = -210 and pauses the ring for 1000 ms when y =-270.
	*/
	float v = 10;
	float g = -32;
	y_pos = v * t + (g / 2)*t*t;
	if (200 + y_pos > -210) {
		x_pos = h * t;

	};
	
	if (200 + y_pos <= -270) {
		y_pos = -470;
		isAnimate = 0;
		glutTimerFunc(delayPeriod, timer_func, 2);
	}
}
void display_func(void) {
	/*
	This is a glut display callback handler which is called whenever a window needs to be displayed or redisplayed. It clears the
	canvas screen and reloads all the objects of the scene when called. This function also displays different messages on the canvas
	based on the game status (win, near_win, not_win), enable ring movement, displays new ring after the game ends until game number
	is 4 then, displays game over message.
	*/
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	// the game instruction printed at the top of the screen
	print_game_instruction(-100, 260, -50);
	draw_velocity_box(-130, 0, -50);
	draw_go_box(-90, 0, -50);
	draw_post(210, -275, -50, 10);
	display_score(-130, 130, -50);
	if (play_count < 4) {
		// get x_pos, y_pos value
		calc_gravity_based_ring_pos();
		draw_ring(-60 + x_pos, 200 + y_pos, -50, 15, 40);
		// check if the post is inside the ring
		check_win_position(x_pos, y_pos);
	}
	else {
		display_game_over(-60, 200, -50);
	}
	// display different messages based on current game status
	if (win) {
		display_success_msg(-270, -270, -50);
	}
	else if (near_win) {
		display_encouragement_msg(-270, -270, -50);
	}
	else if (not_win) {
		display_tryagain_msg(-270, -270, -50);
	}
	// At the end of each display callback, swap the back buffer drawn with the new position of the 
	// objects with the front buffer
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
	// To store the position of the mouse pointer 
	glutMouseFunc(mouse_handler);
	// To enable typing in the velocity and press keys for toggling
	glutKeyboardFunc(keyboard_handler);
	init();
	// Event processing loop
	glutMainLoop();
	return 0;
}