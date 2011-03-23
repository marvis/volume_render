#include "vsvr.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>

void init()
{
	//---------------------------------------------//
	// init openGL extensions with GLew
	/*
	 * GLenum err = glewInit();
	if( err != GLEW_OK || !GL_EXT_texture3D )
	{
		printf(" Error: %s\n", glewGetErrorString(err));
		return 0;
	}
	*/
	glEnable( GL_DEPTH_TEST);
	glClearColor(230,100,0,1);
	glDisable(GL_TEXTURE_2D);
}

void display()
{
	printf("display now\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(230,100,0,1);
	glLoadIdentity();
	glutSwapBuffers();
}

void reshape(int wid, int ht)
{
	printf("reshape width = %d hight = %d\n", wid, ht);
	glViewport(0, 0, wid, ht);
}

void key(unsigned char key, int x, int y)
{
	printf("key %d pressed!\n", key);
}

/**********************************************
 * When drag the mouse
 * ********************************************/
void motion(int x, int y)
{
	printf("mouse move %d, %d\n", x, y);
}

/**********************************************
 * when mouse clicked
 * ********************************************/
void mouse(int button, int state, int x, int y)
{
	printf("pisition(%d, %d), button %d is %d \n", x, y, button, state);
}

int main(int argc, char* argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(50,50);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Volume Rendering Test");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(key);

	glutMainLoop();

	return 0;
}
