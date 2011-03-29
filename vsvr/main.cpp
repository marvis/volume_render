#include "vsvr.h"
#include "tiff_io.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;

#define ESCAPE 27
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

bool blend = false;

int tex_ni = 0;
int tex_nj = 0;
int tex_nk = 0;
int CHANNELS = 0;
VSVR vsvr;
float xmin = -100;
float xmax = 100;
float ymin = -150;
float ymax = 150;
float zmin = -20;
float zmax = 20;
string infile;
bool force_reload = false;

float xrot = 0.0f;
float yrot = 0.0f;
float zrot = 0.0f;
float scale = 1.1f;

void init()
{
	//---------------------------------------------//
	// init openGL extensions with GLew
	GLenum err = glewInit();
	if( err != GLEW_OK || !GL_EXT_texture3D )
	{
		printf(" Error: %s\n", glewGetErrorString(err));
		return;
	}
	
	glEnable( GL_DEPTH_TEST);
	glClearColor(25.0f/255.f,25.f/255.f,63.f/255.f, 38.f/255.f);
	glDisable(GL_TEXTURE_2D);

	unsigned char* img = readtiff((char*)infile.c_str(), &tex_ni, &tex_nj, &tex_nk, &CHANNELS);
	cout<<"tex_ni = "<<tex_ni<<" tex_nj = "<<tex_nj<<" tex_nk = "<<tex_nk<<" CHANNELS = "<<CHANNELS<<endl;
	xmin = - tex_ni/300.0;
	xmax =   tex_ni/300.0;
	ymin = - tex_nj/300.0;
	ymax =   tex_nj/300.0;
	zmin = - tex_nk/300.0;
	zmax =   tex_nk/300.0;

	vsvr.tex_set_intern();
	vsvr.tex_set_resolution(tex_ni, tex_nj, tex_nk);

	float * tex = new float[tex_ni * tex_nj * tex_nk * 4];
	for(int i = 0; i < tex_ni * tex_nj * tex_nk ; i++)
	{
		if(CHANNELS == 1)
		{
			tex[4*i] = img[i] / 255.0f;
			tex[4*i+1] = img[i] / 255.0f;
			tex[4*i+2] = img[i] / 255.0f;
			tex[4*i+3] = img[i] / 255.0f;

		}
		else if(CHANNELS == 3)
		{
			tex[4*i] = img[3*i] / 255.0f;
			tex[4*i+1] = img[3*i+1] / 255.0f;
			tex[4*i+2] = img[3*i+2] / 255.0f;
			tex[4*i+3] = (tex[4*i] + tex[4*i+1] + tex[4*i+2])/3.0f;
		}
	}
	
	vsvr.tex_set_extern(tex);
	delete img;
	force_reload = true;
	printf( "read texture %s of resolution %dx%dx%d and bbox (%.2f,%.2f)x(%.2f,%.2f)x(%.2f,%.2f)\n", (const char*)infile.c_str(), tex_ni, tex_nj, tex_nk, xmin, xmax, ymin, ymax, zmin, zmax ) ;

	force_reload = true;
}


// draw the wire cube with front edges shaded
void draw_cube()
//-----------------------------------------------------------------------------
{

	float gl_model[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, (float *) gl_model );
	float viewer[3];
	viewer[0] = gl_model[0*4 + 2];
	viewer[1] = gl_model[1*4 + 2];
	viewer[2] = gl_model[2*4 + 2];

  float col ;

  ::glBegin(GL_LINES);
  {
    col = (viewer[1]>0 && viewer[2]>0)? 0.4f : 1 ;
    //::glColor3f(col,0,0);
    ::glColor3f(1.0,0,0);
    ::glVertex3f(xmin                 ,ymin,zmin); // e0
    ::glVertex3f(xmax+0.1f*(xmax-xmin),ymin,zmin); // e1

    col = (viewer[1]<0 && viewer[2]>0)? 0.4f : 1 ;
    ::glColor3f(col,0,0);
    ::glVertex3f(xmin,ymax,zmin); // e2
    ::glVertex3f(xmax,ymax,zmin); // e3

    col = (viewer[1]<0 && viewer[2]<0)? 0.4f : 1 ;
    ::glColor3f(col,0,0);
    ::glVertex3f(xmin,ymax,zmax); // e6
    ::glVertex3f(xmax,ymax,zmax); // e7

    col = (viewer[1]>0 && viewer[2]<0)? 0.4f : 1 ;
    ::glColor3f(col,0,0);
    ::glVertex3f(xmin,ymin,zmax); // e4
    ::glVertex3f(xmax,ymin,zmax); // e5

/*---------------------------------------------------------------*/

    col = (viewer[0]>0 && viewer[2]>0)? 0.4f : 1 ;
    ::glColor3f(0,col,0);
    ::glVertex3f(xmin,ymin                 ,zmin); // e0
    ::glVertex3f(xmin,ymax+0.1f*(ymax-ymin),zmin); // e2

    col = (viewer[0]<0 && viewer[2]>0)? 0.4f : 1 ;
    ::glColor3f(0,col,0);
    ::glVertex3f(xmax,ymin,zmin); // e1
    ::glVertex3f(xmax,ymax,zmin); // e3

    col = (viewer[0]<0 && viewer[2]<0)? 0.4f : 1 ;
    ::glColor3f(0,col,0);
    ::glVertex3f(xmax,ymin,zmax); // e5
    ::glVertex3f(xmax,ymax,zmax); // e7

    col = (viewer[0]>0 && viewer[2]<0)? 0.4f : 1 ;
    ::glColor3f(0,col,0);
    ::glVertex3f(xmin,ymin,zmax); // e4
    ::glVertex3f(xmin,ymax,zmax); // e6
/*---------------------------------------------------------------*/

    col = (viewer[0]>0 && viewer[1]>0)? 0.4f : 1 ;
    ::glColor3f(0,0,col);
    ::glVertex3f(xmin,ymin,zmin                 ); // e0
    ::glVertex3f(xmin,ymin,zmax+0.1f*(zmax-zmin)); // e4

    col = (viewer[0]<0 && viewer[1]>0)? 0.4f : 1 ;
    ::glColor3f(0,0,col);
    ::glVertex3f(xmax,ymin,zmin); // e1
    ::glVertex3f(xmax,ymin,zmax); // e5

    col = (viewer[0]<0 && viewer[1]<0)? 0.4f : 1 ;
    ::glColor3f(0,0,col);
    ::glVertex3f(xmax,ymax,zmin); // e3
    ::glVertex3f(xmax,ymax,zmax); // e7

    col = (viewer[0]>0 && viewer[1]<0)? 0.4f : 1 ;
    ::glColor3f(0,0,col);
    ::glVertex3f(xmin,ymax,zmin); // e2
    ::glVertex3f(xmin,ymax,zmax); // e6
  }
  ::glEnd();
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	yrot += 1.0f;
	glScalef(scale, scale, scale);
	scale *= 1.001f;
	// draw the cube
	draw_cube();

	glPushMatrix();
	{	
	glTranslatef(xmin, ymin, zmin);
	glScalef((xmax-xmin)/vsvr.tex_ni(), (ymax - ymin)/vsvr.tex_nj(), (zmax-zmin)/vsvr.tex_nk());
	if(force_reload)
	{
		force_reload = !vsvr.gl_render();
	}
	else
	{
		force_reload = !vsvr.gl_redisplay();
	}
	}
	glPopMatrix();
	glutSwapBuffers();
}

void reshape(int wid, int ht)
{
	glViewport(0, 0, wid, ht);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if(0)
	{
	if(ht > wid)
		glOrtho( - 1.3, 1.3, -1.3*(double)ht/wid, 1.3 *(double)ht/wid, -4, 10);
	else
		glOrtho(-1.3*(double)wid/ht, 1.3*(double)wid/ht, -1.3, 1.3, -4, 10);
	}
	else
	{
		gluPerspective(45.0, ht>0?(double)wid/ht:1.0, 0.5, 10.);
	}
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

void key(unsigned char key, int x, int y)
{
	usleep(100);
	switch(key)
	{
		case ESCAPE:
			glutDestroyWindow(NULL);
			exit(1);
			break;
		case 98:
		case 130:
			printf("B/b pressed; blending is : %d\n", blend);
			blend = blend ? 0 : 1;
			if(!blend){
				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
			}
			else 
			{
				glEnable(GL_BLEND);
				glDisable(GL_DEPTH_TEST);
			}
	}
}

/**********************************************
 * When drag the mouse
 * ********************************************/
void motion(int x, int y)
{
}

/**********************************************
 * when mouse clicked
 * ********************************************/
void mouse(int button, int state, int x, int y)
{
}

int main(int argc, char* argv[])
{
	infile = string(argv[1]);
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(50,50);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Volume Rendering Test");

	init();

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(key);

	glutMainLoop();

	return 0;
}
