#include <stdio.h>
#include <stdlib.h>
#include "GL\glut.h"
#include "Mesh.h"

float			_zoom		= 15.0f;
float			_rot_x		= 0.0f;
float			_rot_y		= 0.001f;
float			_trans_x	= 0.0f;
float			_trans_y	= 0.0f;
int				_last_x		= 0;
int				_last_y		= 0;
unsigned char	_buttons[3] = { 0 };

Mesh* mesh		= NULL;
int _renderMode = 3;

void Init(void)
{
	glEnable(GL_DEPTH_TEST);
}

void Draw(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	switch (_renderMode) {
	case 0:
		mesh->drawPoint();
		break;
	case 1:
		mesh->drawWireframe();
		break;
	case 2:
		mesh->drawSurface();
		break;
	case 3:
		mesh->drawSurface(true);
		break;
	}
	glDisable(GL_LIGHTING);
}

void Display(void)
{
	//glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0, 0, -_zoom);
	glTranslatef(_trans_x, _trans_y, 0);
	glRotatef(_rot_x, 1, 0, 0);
	glRotatef(_rot_y, 0, 1, 0);

	Draw();
	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	if (w == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / h, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'Q': case 'q':
		exit(0);
	case 'p': case 'P':
		_renderMode = 0;
		break;
	case 'w': case 'W':
		_renderMode = 1;
		break;
	case 's': case 'S':
		_renderMode = 2;
		break;
	case 'a': case 'A':
		_renderMode = 3;
		break;
	case 'f': case 'F':
		mesh->LaplacianSmoothing(5);
		break;
	case 'c': case 'C':
		mesh->CotangentLaplacianSmoothing(100);
		break;
	case 'r': case 'R':
		mesh->reset();
		break;
	default: break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	_last_x = x;
	_last_y = y;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		_buttons[0] = state == GLUT_DOWN ? 1 : 0;
		break;
	case GLUT_MIDDLE_BUTTON:
		_buttons[1] = state == GLUT_DOWN ? 1 : 0;
		break;
	case GLUT_RIGHT_BUTTON:
		_buttons[2] = state == GLUT_DOWN ? 1 : 0;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	int diff_x = x - _last_x;
	int diff_y = y - _last_y;
	_last_x = x;
	_last_y = y;

	if (_buttons[2]) {
		_zoom -= (float)0.02f * diff_x;
	}
	else if (_buttons[1]) {
		_trans_x += (float)0.02f * diff_x;
		_trans_y -= (float)0.02f * diff_y;
	}
	else if (_buttons[0]) {
		_rot_x += (float)0.2f * diff_y;
		_rot_y += (float)0.2f * diff_x;
	}
	glutPostRedisplay();
}

void main(int argc, char* argv[])
{
	mesh = new Mesh("OBJ\\dragon.obj");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("polygonal-mesh-smoothing");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	Init();
	glutMainLoop();
}