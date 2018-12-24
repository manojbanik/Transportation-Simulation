//
//	Programming Assignment Three
//	Texture Mapping and Imaging Pipeline

//	
//	MSIM 541 (Computer Graphics & Visualization)
//	Assigned by: Dr. Yuzhong Shen, Associate Professor, MSVE, ODU.
//
//	Submitted by: Manoj Banik, e-mail: mbani003@odu.edu
//


#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

#include <gl/glew.h>
#include <gl/glut.h>

#include "ObjModel.h"
#include "PPMImage.h"
#include "BillBoard.h"
#include "TrafficLight.h"
#include "utility.h"

#define PI 3.141593
#define TotalBillBoards 6

using namespace std;


//int ff = 1;

// Counter for the traffic signal.  Only one is needed.
int counter = 0;

// 20 miliseconds
int updateInterval = 20;

int delay = 60;	//	Signal timing, 250 for 5 sec. For slow computer we can replace 250 by something small value, like 25.

//image for screenshots
PPMImage snapshot;

//Billboards
Billboard board[TotalBillBoards];
Vector3 boardloc0 = { 30.0, 3.0, -90.0 };		//	energy
Vector3 boardloc1 = { -40.0, 3.0, -120.0 };		//	nike
Vector3 boardloc2 = { -70.0, 4.0, -25.0 };		//	pepsi
Vector3 boardloc3 = { 45.0, 4.0, 25.0 };		//	macdonalds
Vector3 boardloc4 = { 40.0, 5.0, -280.0 };		//	cocacola
Vector3 boardloc5 = { -40.0, 5.0, -360.0 };		//	miami


// 3D models
ObjModel car, surveillanceCamera;


TrafficLight trafficLight;

// The display list IDs for car, surveillanceCamera, and terrain.
int carID, surveillanceCameraID, terrainID, treeID, boardID;

// Signals for North-South traffic and West-East traffic
Signal NS_Signal = Green, WE_Signal = Red;

//	tree position
Vector3 treePosition = { -15, 0, -20 };

// Car position and speed
Vector3 carPosition = { 2.5, 0, 70}, carInitialPos = { 2.5, 0, 70},
		localCarSpeed = {0, 0, 0}, worldCarSpeed;

// Car direction and heading
float carDirection = 180, carTurnAmount = 5.625;
string carHeading = "N";

// Third person camera offset in the car's local frame and world frame.
Vector3	localCameraOffset = {0, 0, -7};
Vector3	worldCameraOffset = {localCameraOffset.z*sin(carDirection*PI/180),0,localCameraOffset.z*cos(carDirection*PI/180)};

// Width and height for the window and three small viewports.
int winWidth, winHeight, sWidth, sHeight;


// Update the small viewports' size automatically.
void reshape(int w, int h)
{
	winWidth = w, winHeight = h;
	
	sWidth = 3 * winWidth / 10, sHeight = winHeight / 4;

	//printf("\n\nw=%d, h=%d", w, h);
}


void specialKey(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)
	{
		// Update car direction
				
		carDirection += carTurnAmount;
		if (carDirection == 360) carDirection = 0;

		worldCarSpeed.x = localCarSpeed.z*sin(carDirection*PI / 180.0);		//	x = z*sin*theta
		worldCarSpeed.y = 0;
		worldCarSpeed.z = localCarSpeed.z*cos(carDirection*PI / 180.0);		//	z = z*cos*theta

		// Update the third person camera offset in the world frame.
		worldCameraOffset.x = localCameraOffset.z*sin(carDirection*PI / 180);
		worldCameraOffset.z = localCameraOffset.z*cos(carDirection*PI / 180);

		// Compute the car heading.
		if (carDirection == 180)
			carHeading = "N";
		else if (carDirection == 0)
			carHeading = "S";
		else if (carDirection == 270)
			carHeading = "W";
		else if (carDirection == 90)
			carHeading = "E";
		else if (carDirection>0 && carDirection<90)
			carHeading = "SE";
		else if (carDirection>90 && carDirection<180)
			carHeading = "NE";
		else if (carDirection>180 && carDirection<270)
			carHeading = "NW";
		else
			carHeading = "SW";
	}
	if (key == GLUT_KEY_RIGHT)
	{

		// Handle the right turns.
		// Update car direction
		if (carDirection == 0)
			carDirection = 354.375;
		else
			carDirection -= carTurnAmount;		//	360/64 = 5.625
		worldCarSpeed.x = localCarSpeed.z*sin(carDirection*PI / 180);
		worldCarSpeed.y = 0;
		worldCarSpeed.z = localCarSpeed.z*cos(carDirection*PI / 180);

		// Update the third person camera offset in the world frame.
		worldCameraOffset.x = localCameraOffset.z*sin(carDirection*PI / 180);
		worldCameraOffset.z = localCameraOffset.z*cos(carDirection*PI / 180);
		// Compute the car heading.
		if (carDirection == 180)
			carHeading = "N";
		else if (carDirection == 0)
			carHeading = "S";
		else if (carDirection == 270)
			carHeading = "W";
		else if (carDirection == 90)
			carHeading = "E";
		else if (carDirection>0 && carDirection<90)
			carHeading = "SE";
		else if (carDirection>90 && carDirection<180)
			carHeading = "NE";
		else if (carDirection>180 && carDirection<270)
			carHeading = "NW";
		else
			carHeading = "SW";
	}
	
	if (key == GLUT_KEY_UP)
	{
		// acceleration
		
		localCarSpeed.z += .4;

		if (localCarSpeed.z>2) localCarSpeed.z = 2;
			
		
		worldCarSpeed.x = localCarSpeed.z*sin(carDirection*PI / 180);
		worldCarSpeed.y = 0;
		worldCarSpeed.z = localCarSpeed.z*cos(carDirection*PI / 180);
	}

	if (key == GLUT_KEY_DOWN)
	{
		// deceleration
		
		localCarSpeed.z -= .4;

		if (localCarSpeed.z<-2) localCarSpeed.z = -2;
		
		worldCarSpeed.x = localCarSpeed.z*sin(carDirection*PI / 180);
		worldCarSpeed.y = 0;
		worldCarSpeed.z = localCarSpeed.z*cos(carDirection*PI / 180);
	}

	//printf("\ncar pos: %f, %f", carPosition.x, carPosition.z);
}


void drawScene()
{
	// Draw terrain
	glCallList(terrainID);

	glEnable(GL_LIGHTING);

	// North-East (NS_Signal)
	
	glPushMatrix();
	glTranslatef(10, 0, -10.5);
	glScalef(0.0254, 0.0254, 0.0254);
	trafficLight.setSignal(NS_Signal);
	trafficLight.Draw();
	glPopMatrix();

	glPushMatrix();	
	glTranslatef(10, 0, -10);
	glRotatef(-45, 0, 1, 0);
	glCallList(surveillanceCameraID);
	glPopMatrix();
	
	
	// South-West (NS_Signal)
	glPushMatrix();
	glTranslatef(-10, 0, 10.5);
	glRotated(180, 0, 1, 0);
	glScalef(0.0254, 0.0254, 0.0254);
	trafficLight.setSignal(NS_Signal);
	trafficLight.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-10, 0, 10);
	glRotatef(135, 0, 1, 0);
	glCallList(surveillanceCameraID);
	glPopMatrix();
	
	// South-East (WE_Signal)
	glPushMatrix();
	glTranslatef(10, 0, 10.5);
	glRotated(270, 0, 1, 0);
	glScalef(0.0254, 0.0254, 0.0254);
	trafficLight.setSignal(WE_Signal);
	trafficLight.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10, 0, 10);
	glRotatef(225, 0, 1, 0);
	glCallList(surveillanceCameraID);
	glPopMatrix();

	// North-West (WE_Signal)
	glPushMatrix();
	glTranslatef(-10, 0, -10.5);
	glRotated(90, 0, 1, 0);
	glScalef(0.0254, 0.0254, 0.0254);
	trafficLight.setSignal(WE_Signal);
	trafficLight.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-10, 0, -10);
	glRotatef(45, 0, 1, 0);
	glCallList(surveillanceCameraID);
	glPopMatrix();
	
	// Draw the car.
	glPushMatrix();
	glTranslatef(carPosition.x, carPosition.y, carPosition.z);
	glRotated(carDirection, 0, 1, 0);
	//glScalef(1.0/3.28/12, 1/3.28/12, 1/3.28/12);
	//glScalef(0.0254, 0.0254, 0.0254);
	glCallList(carID);
	glPopMatrix();

	
	//	For billboards
	glPushMatrix();
	glCallList(boardID);
	glPopMatrix();
}

void init()
{
	glClearColor(0.5, 0.5, 1.0, 1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Note that the light is defined in the eye or camera frame.
	GLfloat light_position[] = {0.0, 0.0, 0.0, 1.0};

	GLfloat ambient[] = {0.3, 0.3, 0.3, 1};
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1};

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);	
	glEnable(GL_LIGHT0);

	// Generate display list for the board.
	boardID = glGenLists(1);
	
	glNewList(boardID, GL_COMPILE);
	for (int i = 0; i<TotalBillBoards; i++)
		board[i].Draw();
	glEndList();


	// Generate display list for the surveillance camera.
	surveillanceCameraID = glGenLists(1);
	
	glNewList(surveillanceCameraID, GL_COMPILE);
	surveillanceCamera.Draw();
	glEndList();

	// Generate display list for the car.

	carID = glGenLists(1);

	glNewList(carID, GL_COMPILE);
	car.Draw();
	glEndList();


	// Generate the display list for terrain, including road and grass.
	terrainID = glGenLists(1);
	glNewList(terrainID, GL_COMPILE);
	glDisable(GL_LIGHTING);

	
	// Grass

	glColor3f(0, 0.8, 0);

	glBegin(GL_QUADS);
	
		glVertex3f(-1000, 0, 1000);		//	left-bottom part
		glVertex3f(-10, 0, 1000);
		glVertex3f(-10, 0, 10);
		glVertex3f(-1000, 0, 10);

		glVertex3f(10, 0, 1000);		//	right-bottom
		glVertex3f(1000, 0, 1000);
		glVertex3f(1000, 0, 10);
		glVertex3f(10, 0, 10);

		glVertex3f(10, 0, -10);			//	right-top
		glVertex3f(1000, 0, -10);
		glVertex3f(1000, 0, -1000);
		glVertex3f(10, 0, -1000);
		
		glVertex3f(-1000, 0, -10);		//	left-top
		glVertex3f(-10, 0, -10);
		glVertex3f(-10, 0, -1000);
		glVertex3f(-1000, 0, -1000);
		
	glEnd();

	
	// Roads
	
	glColor3f(0.2, 0.2, 0.2);

	glBegin(GL_QUADS);

		//	North-South

		glVertex3f(-10, 0, 1000);		//	Starting from left-bottom, 
		glVertex3f(10, 0, 1000);
		glVertex3f(10, 0, -1000);
		glVertex3f(-10, 0, -1000);
		
		//	East-West

		glVertex3f(-1000, 0, 10);		//	Starting from left-bottom,
		glVertex3f(1000, 0, 10);
		glVertex3f(1000, 0, -10);
		glVertex3f(-1000, 0, -10);
	
	glEnd();

	
	// Yellow line
	
	//North ->south

	glColor3f(1, 1, 0);

	glBegin(GL_POLYGON);
	
	glVertex3f(-0.125, 0.05, -10);		//	I increased the width of yellow line a little bit
	glVertex3f(0.125, 0.05, -10);		//	Width = (.125+.125) = .25
	glVertex3f(0.125, 0.05, -1000);
	glVertex3f(-0.125, 0.05, -1000);
	glEnd();
	
	glBegin(GL_POLYGON);
	
	glVertex3f(-0.125, 0.05, 1000);
	glVertex3f(0.125, 0.05, 1000);
	glVertex3f(0.125, 0.05, 10);
	glVertex3f(-0.125, 0.05, 10);
	glEnd();
	
	
	//East --> West 
	
	glBegin(GL_POLYGON);
	
	glVertex3f(10, 0.05, -0.125);
	glVertex3f(10, 0.05, 0.125);
	glVertex3f(1000, 0.05, -0.125);
	glVertex3f(1000, 0.05, 0.125);
	glEnd();

	glBegin(GL_POLYGON);
	
	glVertex3f(-1000, 0.05, -0.125);
	glVertex3f(-1000, 0.05, 0.125);
	glVertex3f(-10, 0.05, -0.125);
	glVertex3f(-10, 0.05, 0.125);
	glEnd();
	
	
	//White broken lines

	glColor3f(1, 1, 1);
	
	for (int i = 0; i<83; i++)
	{
		//	North --> South

		glBegin(GL_POLYGON);
		
		glVertex3f(4.90, 0.05, i * 12 + 10);		//	Width = 5.1-4.9 = 0.2
		glVertex3f(5.1, 0.05, i * 12 + 10);			//	Yellow and broken both are normal lines
		glVertex3f(5.1, 0.05, i * 12 + 13);			//	But I keep broken little less width
		glVertex3f(4.90, 0.05, i * 12 + 13);
		glEnd();

		glBegin(GL_POLYGON);
		
		glVertex3f(-4.9, 0.05, i * 12 + 10);
		glVertex3f(-5.1, 0.05, i * 12 + 10);
		glVertex3f(-5.1, 0.05, i * 12 + 13);
		glVertex3f(-4.9, 0.05, i * 12 + 13);
		glEnd();

		glBegin(GL_POLYGON);
		
		glVertex3f(4.9, 0.05, -i * 12 - 10);
		glVertex3f(5.1, 0.05, -i * 12 - 10);
		glVertex3f(5.1, 0.05, -i * 12 - 13);
		glVertex3f(4.9, 0.05, -i * 12 - 13);
		glEnd();

		glBegin(GL_POLYGON);
		
		glVertex3f(-4.9, 0.05, -i * 12 - 10);
		glVertex3f(-5.1, 0.05, -i * 12 - 10);
		glVertex3f(-5.1, 0.05, -i * 12 - 13);
		glVertex3f(-4.9, 0.05, -i * 12 - 13);
		glEnd();

		//west -> east
		glBegin(GL_POLYGON);
		
		glVertex3f(-i * 12 - 10, 0.05, 5.1);
		glVertex3f(-i * 12 - 10, 0.05, 4.9);
		glVertex3f(-i * 12 - 13, 0.05, 4.9);
		glVertex3f(-i * 12 - 13, 0.05, 5.1);
		glEnd();

		glBegin(GL_POLYGON);
		
		glVertex3f(i * 12 + 10, 0.05, 5.1);
		glVertex3f(i * 12 + 10, 0.05, 4.9);
		glVertex3f(i * 12 + 13, 0.05, 4.9);
		glVertex3f(i * 12 + 13, 0.05, 5.1);
		glEnd();

		glBegin(GL_POLYGON);
		
		glVertex3f(-i * 12 - 10, 0.05, -5.1);
		glVertex3f(-i * 12 - 10, 0.05, -4.9);
		glVertex3f(-i * 12 - 13, 0.05, -4.9);
		glVertex3f(-i * 12 - 13, 0.05, -5.1);
		glEnd();

		glBegin(GL_POLYGON);
		
		glVertex3f(i * 12 + 10, 0.05, -5.1);
		glVertex3f(i * 12 + 10, 0.05, -4.9);
		glVertex3f(i * 12 + 13, 0.05, -4.9);
		glVertex3f(i * 12 + 13, 0.05, -5.1);
		glEnd();

	}
	
	
	//Solid stop lines (at the intersection)
	
	glBegin(GL_POLYGON);
	
	glVertex3f(10, 0.05, 10);
	glVertex3f(0, 0.05, 10);
	glVertex3f(0, 0.05, 10.5);			//		Stop lines are twice much width as normal lines.
	glVertex3f(10, 0.05, 10.5);
	glEnd();
	
	glBegin(GL_POLYGON);
	
	glVertex3f(-10, 0.05, -10);
	glVertex3f(0, 0.05, -10);
	glVertex3f(0, 0.05, -10.5);
	glVertex3f(-10, 0.05, -10.5);
	glEnd();

	glBegin(GL_POLYGON);
	
	glVertex3f(-10.5, 0.05, 0);
	glVertex3f(-10.5, 0.05, 10);
	glVertex3f(-10, 0.05, 10);
	glVertex3f(-10, 0.05, 0);
	glEnd();

	glBegin(GL_POLYGON);
	
	glVertex3f(10, 0.05, 0);
	glVertex3f(10, 0.05, -10);
	glVertex3f(10.5, 0.05, -10);
	glVertex3f(10.5, 0.05, 0);
	glEnd();
	
	glEndList();
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Generate head-up display (HUD)
	stringstream ss;

	// Setup viewport, projection, and camera for the main view.
	glViewport(0, 0, winWidth, winHeight - sHeight - 50);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (float) winWidth / (winHeight - sHeight - 50), 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	
	// Update the code here for the correct third person camera that moves with the car.

	gluLookAt(carPosition.x + worldCameraOffset.x, carPosition.y + 2, carPosition.z + worldCameraOffset.z, 
		      carPosition.x, carPosition.y + 1.5, carPosition.z, 
			  0, 1, 0);


	
	drawScene();

	
	// Setup viewport, projection, and camera for the South-East camera and draw the scene again.
	glViewport(winWidth / 40, winHeight - sHeight, sWidth, sHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, sWidth / sHeight, 1, 100);	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(10, 2.5, 10, -15, 0, -10, 0, 1, 0);


	drawScene();

	
	// Setup the viewport, projection, camera for the top view and draw the scene again.
	glViewport(winWidth / 20 + sWidth, winHeight - sHeight, sWidth, sHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, (float)sWidth / (float)sHeight, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 50, 0, 0, 0.5, 0, 0, 0, -1);

	

	drawScene();

	// Setup viewport, projection, camera for the South-West camera and draw the scene again.
	glViewport(3 * winWidth / 40 + 2 * sWidth, winHeight - sHeight, sWidth, sHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, (float)sWidth / (float)sHeight, 2, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(-10, 2.5, 10, 15, 0, -10, 0, 1, 0);

	drawScene();

	
	string direction;
	if (localCarSpeed.z<0)
		direction = "Backward";
	else
		direction = "Forward";
	glColor3f(1.0, 1.0, 1.0);
	glWindowPos2f(winWidth / 2 - sWidth / 2 + 1, winHeight - sHeight - 20);
	printLargeString("Heading: " + carHeading + "  Car speed: " + to_string(abs(localCarSpeed.z) / 0.02) + " m/s (" + direction + ")");

	
	glutSwapBuffers();
	glFlush();

	//printf("\ncar pos: %f, %f", carPosition.x, carPosition.z);
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
		// Add code for reset
		carPosition.x = carInitialPos.x;
		carPosition.y = carInitialPos.y;
		carPosition.z = carInitialPos.z;
		carDirection = 180;
		carHeading = "N";

		localCarSpeed.z = 0;
		worldCarSpeed.x = 0;
		worldCarSpeed.z = 0;

		worldCameraOffset.x = localCameraOffset.z*sin(carDirection*PI / 180);
		worldCameraOffset.z = localCameraOffset.z*cos(carDirection*PI / 180);
		counter = 0;
		NS_Signal = Green;
		WE_Signal = Red;
		break;

	case 'b':
		// Add code for breaking.
		localCarSpeed.z = 0;
		worldCarSpeed.x = worldCarSpeed.y = worldCarSpeed.z = 0;

		printf("\ncar pos: %f, %f", carPosition.x, carPosition.z);
		break;

	case 's':
		// Code for screen snapshot
	
		snapshot.AllocateMemory(winWidth, winHeight);
		glReadPixels(0, 0, winWidth, winHeight, GL_RGB, GL_UNSIGNED_BYTE, snapshot.image);


		snapshot.VerticalFlip();
		snapshot.WriteFile("snapshot.ppm", "P6");
				
		break;

	case 27:
		exit(0);
		break;
	}

	
	glutPostRedisplay();
}

void update()
{
	// Update car position.

	int d2 = delay / 5;

	carPosition.x += worldCarSpeed.x;
	carPosition.z += worldCarSpeed.z;


	// State machine for the traffic signals using three variables: NS_Signal, WE_Signal, and counter.
	
	switch (NS_Signal){
	
	case Green:
		if (counter<delay){
			counter++;
		}
		else{
			counter = 0;
			NS_Signal = Yellow;
		}
		break;
	
	case Yellow:
		if (counter<d2){
			counter++;
		}
		else{
			counter = 0;
			NS_Signal = Red;
			WE_Signal = Green;
		}
		break;
	
	case Red:
		if (counter<delay){
			counter++;
		}
		else if (counter == delay){
			WE_Signal = Yellow;
			counter++;
		}
		else if (counter>delay && counter <(delay+d2)){
			counter++;
		}
		else {
			counter = 0;
			NS_Signal = Green;
			WE_Signal = Red;
		}
		
		/*
		if (carPosition.z < 5.0 && carPosition.z > -5.0)
		{
			snapshot.AllocateMemory(sWidth, sHeight);
			glReadPixels(winWidth / 40, winHeight - sHeight, sWidth, sHeight, GL_RGB, GL_UNSIGNED_BYTE, snp2.image);
			snapshot.VerticalFlip();
			snapshot.WriteFile("snp.ppm", "P6");
		}
		*/

		break;
	}
}


void timer(int miliseconds)
{
	update();
	glutTimerFunc(updateInterval, timer, updateInterval);	
	glutPostRedisplay();
}


//	Load all 3D models & Billboards
void loadModels(void)
{
	trafficLight.ReadFile("Models/TrafficLight.obj");
	car.ReadFile("Models/taxi.obj");
	surveillanceCamera.ReadFile("Models/camera.obj");

	//tree.ReadFile("Models/New2/tree_obj.obj");
	
	board[0].SetSize(20, 10);
	board[0].SetLocation(boardloc0);
	board[0].SetOrientation(180);
	board[0].ReadFile("Models/ad/cleanenergy.ppm");		//

	board[1].SetSize(24, 12);
	board[1].SetLocation(boardloc1);
	board[1].SetOrientation(180);
	board[1].ReadFile("Models/ad/nike.ppm");		//

	board[2].SetSize(18, 9);
	board[2].SetLocation(boardloc2);
	board[2].SetOrientation(90);
	board[2].ReadFile("Models/ad/pepsi.ppm");

	board[3].SetSize(16, 8);
	board[3].SetLocation(boardloc3);
	board[3].SetOrientation(90);
	board[3].ReadFile("Models/ad/macdonalds.ppm");		//	macdonalds

	board[4].SetSize(30, 15);
	board[4].SetLocation(boardloc4);
	board[4].SetOrientation(180);
	board[4].ReadFile("Models/ad/cocacola.ppm");

	board[5].SetSize(32, 16);
	board[5].SetLocation(boardloc5);
	board[5].SetOrientation(180);
	board[5].ReadFile("Models/ad/miami.ppm");
	

}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(30, 50);
	winWidth = 1300, winHeight = 708;	//	I found this setting gives me clear view with lines visible
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Traffic Simulation: Texture Mapping and Imaging Pipeline");
	glewInit();

	// Load the 3D models.
	loadModels();
	

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutTimerFunc(0, timer, updateInterval);
	glutMainLoop();

	system("pause");
}