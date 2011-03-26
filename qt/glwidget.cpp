/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "vsvr.h"
#include "tiff_io.h"
#include <iostream>
#include <stdio.h>
#include <GL/glew.h>

#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "glwidget.h"
#include "qtlogo.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//! [0]
GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    logo = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;

    qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);

	tex_ni = 0;
	tex_nj = 0;
	tex_nk = 0;
	tex = NULL;

	xmin = 0.0f;
	ymin = 0.0f;
	zmin = 0.0f;
	xmax = 0.0f;
	ymax = 0.0f;
	ymax = 0.0f;

	force_reload = false;
}
//! [0]

//! [1]
GLWidget::~GLWidget()
{
}
//! [1]

//! [2]
QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}
//! [2]

//! [3]
QSize GLWidget::sizeHint() const
//! [3] //! [4]
{
    return QSize(400, 400);
}
//! [4]

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

//! [5]
void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}
//! [5]

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

//! [6]
void GLWidget::initializeGL()
{
    qglClearColor(qtPurple.dark());

    logo = new QtLogo(this, 64);
    logo->setColor(qtGreen.dark());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}
//! [6]

//! [7]
void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
	
	draw_cube();
	if(tex != NULL)
	{
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
	}
	else
	{
    	logo->draw();
	}
}
//! [7]

//! [8]
void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}
//! [8]

//! [9]
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}
//! [9]

//! [10]
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }
    lastPos = event->pos();
}
//! [10]

void GLWidget::loadTiff(QString file)
{
	int channels = 0;
	unsigned char * img = readtiff((char*)file.toStdString().c_str(),&tex_ni, &tex_nj, &tex_nk, &channels);	
	std::cout<<"tex_ni = "<<tex_ni<<std::endl;
	std::cout<<"tex_nj = "<<tex_nj<<std::endl;
	std::cout<<"tex_nk = "<<tex_nk<<std::endl;
	std::cout<<"channels = "<<channels<<std::endl;

	// init opengl 
	GLenum err = glewInit();
	if( err != GLEW_OK || !GL_EXT_texture3D )
	{
		printf(" Error: %s\n", glewGetErrorString(err));
		return;
	}

	glEnable( GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	xmin = - tex_ni/300.0;
	xmax =   tex_ni/300.0;
	ymin = - tex_nj/300.0;
	ymax =   tex_nj/300.0;
	zmin = - tex_nk/300.0;
	zmax =   tex_nk/300.0;

	vsvr.tex_set_intern();
	vsvr.tex_set_resolution(tex_ni, tex_nj, tex_nk);
	tex = new float[tex_ni * tex_nj * tex_nk * 4];
	for(int i = 0; i < tex_ni * tex_nj * tex_nk ; i++)
	{
		if(channels == 1)
		{
			tex[4*i] = img[i] / 255.0f;
			tex[4*i+1] = img[i] / 255.0f;
			tex[4*i+2] = img[i] / 255.0f;
			tex[4*i+3] = img[i] / 255.0f;

		}
		else if(channels == 3)
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
}

void GLWidget::draw_cube()
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
    ::glColor3f(col,0,0);
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

