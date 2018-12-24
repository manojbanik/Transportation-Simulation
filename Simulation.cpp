#include "Simulation.h"

Simulation *simulation;

void display()
{
	simulation->Draw();
}

void reshape(int w, int h)
{
	simulation->Reshape(w, h);
}

void keyboard(unsigned char key, int x, int y)
{
	simulation->Keyboard(key, x, y);
	glutPostRedisplay();
}

void specialKey(int key, int x, int y)
{
	simulation->SpecialKey(key, x, y);
	glutPostRedisplay();
}

void mouseMove(int x, int y)
{
	simulation->MouseMove(x, y);
	glutPostRedisplay();
}

void passiveMouseMove(int x, int y)
{
	simulation->PassiveMouseMove(x, y);
	glutPostRedisplay();
}

void clock(int time)
{
	simulation->Update();
	glutTimerFunc(time, clock, time);	
	glutPostRedisplay();
};

Simulation::Simulation(void)
{
}

Simulation::~Simulation(void)
{
}

void Simulation::InitializeSimulation(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
}

void Simulation::InitializeGraphics()
{
	glutInitWindowPosition(positionX, positionY);
	glutInitWindowSize(width, height);
	glutCreateWindow(title);
	glewInit();
}

void Simulation::SetWindowPosition(int x, int y)
{
	this->positionX = x;
	this->positionY = y;
}

void Simulation::SetWindowSize(int width, int height)
{
	this->width = width;
	this->height = height;
}

void Simulation::SetClock(int timer)
{
	this->timer = timer;
	timerSet = true;
}

void Simulation::RegisterCallbacks()
{
	::simulation = this;
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutMotionFunc(::mouseMove);
	::glutPassiveMotionFunc(::passiveMouseMove);

	if (timerSet)
		glutTimerFunc(timer, clock, timer);
}

void Simulation::Run()
{
	InitializeGraphics();
	RegisterCallbacks();
	glutMainLoop();
}