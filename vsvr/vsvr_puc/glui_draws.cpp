/**
 * @file    glui_draws.cpp
 * @author  Thomas Lewiner <thomas.lewiner@polytechnique.org>
 * @author  Math Dept, PUC-Rio
 * @version 0.1
 * @date    30/05/2006
 *
 * @brief   VSVR graphical interface: drawing commands
 */
//________________________________________________


#if !defined(WIN32) || defined(__CYGWIN__)
#pragma implementation "glui_defs.h"
#endif // WIN32


#include <GL/glew.h> // openGL extension
#include "glui_defs.h"


//_____________________________________________________________________________
// declarations of this file

// display element
int   show_cube = 1 ;
int   show_grid = 0 ;
int   show_tf   = 1 ;

// orthographic / perspective projection
int ortho = 0 ;

// object transformation
float view_rotate[16] = { 1.0f,0.0f,0.0f,0.0f, 0.0f,1.0f,0.0f,0.0f, 0.0f,0.0f,1.0f,0.0f, 0.0f,0.0f,0.0f,1.0f };
float obj_pos    [3 ] = { 0.0f, 0.0f, 0.0f };

// viewer position for drawing the grid on the right side of the cube
float viewer[3] = { 0.0f, 0.0f, 0.0f };

// mark parameter modification
bool  force_reload = true;

// volume rendering setting
int   static_nslices = 256;
int   motion_nslices = 128 ;


//-----------------------------------------------------------------------------

// draw the grid on one cube side
void draw_cube_grid(const float p0[3], const float p1[3], const float p2[3], const float p3[3], int g1,int g2) ;

// draw the grid on the cube sides
void draw_grid() ;

// draw the wire cube with front edges shaded
void draw_cube() ;


//-----------------------------------------------------------------------------

// draw the transfer function
void draw_tranfer() ;

// window resizing
void reshape( int x, int y ) ;

// main drawing function
void display() ;


//-----------------------------------------------------------------------------

// screen capture
void write_ppm() ;


//_____________________________________________________________________________






//_____________________________________________________________________________
// draw the grid on one cube side
void draw_cube_grid(const float p0[3], const float p1[3], const float p2[3], const float p3[3], int g1,int g2)
//-----------------------------------------------------------------------------
{
  int i ;
  float dq[3], dr[3], q[3], r[3] ;

  q[0] = p1[0] ;  q[1] = p1[1] ;  q[2] = p1[2] ;
  dq[0] = (p0[0]-p1[0]) / g1;
  dq[1] = (p0[1]-p1[1]) / g1;
  dq[2] = (p0[2]-p1[2]) / g1;

  r[0] = p3[0] ;  r[1] = p3[1] ;  r[2] = p3[2] ;
  dr[0] = (p2[0]-p3[0]) / g1;
  dr[1] = (p2[1]-p3[1]) / g1;
  dr[2] = (p2[2]-p3[2]) / g1;
  for (i = 0 ; i < g1-1 ; ++i)
  {
    q[0] += dq[0] ;  q[1] += dq[1] ;  q[2] += dq[2] ;
    ::glVertex3fv(q);
    r[0] += dr[0] ;  r[1] += dr[1] ;  r[2] += dr[2] ;
    ::glVertex3fv(r);
  }

  q[0] = p2[0] ;  q[1] = p2[1] ;  q[2] = p2[2] ;
  dq[0] = (p0[0]-p2[0]) / g2;
  dq[1] = (p0[1]-p2[1]) / g2;
  dq[2] = (p0[2]-p2[2]) / g2;

  r[0] = p3[0] ;  r[1] = p3[1] ;  r[2] = p3[2] ;
  dr[0] = (p1[0]-p3[0]) / g2;
  dr[1] = (p1[1]-p3[1]) / g2;
  dr[2] = (p1[2]-p3[2]) / g2;
  for (i = 0 ; i < g2-1 ; ++i)
  {
    q[0] += dq[0] ;  q[1] += dq[1] ;  q[2] += dq[2] ;
    ::glVertex3fv(q);
    r[0] += dr[0] ;  r[1] += dr[1] ;  r[2] += dr[2] ;
    ::glVertex3fv(r);
  }
}
//_____________________________________________________________________________





//_____________________________________________________________________________
// draw the grid on the cube sides
void draw_grid()
//-----------------------------------------------------------------------------
{
  float p0[3], p1[3], p2[3], p3[3] ;

  ::glColor3f(0.3f, 0.3f, 0.3f);
  ::glBegin(GL_LINES);
  {
    p0[0] = p1[0] = p2[0] = p3[0] = (viewer[0] < 0) ? xmax : xmin ;
    p0[1] = p1[1] = ymin ;  p2[1] = p3[1] = ymax ;
    p0[2] = p2[2] = zmin ;  p1[2] = p3[2] = zmax ;
    draw_cube_grid(p0,p1,p2,p3, tex_nk-1,tex_nj-1);

    p0[1] = p1[1] = p2[1] = p3[1] = (viewer[1] < 0) ? ymax : ymin ;
    p0[0] = p1[0] = xmin ;  p2[0] = p3[0] = xmax ;
    p0[2] = p2[2] = zmin ;  p1[2] = p3[2] = zmax ;
    draw_cube_grid(p0,p1,p2,p3, tex_nk-1,tex_ni-1);

    p0[2] = p1[2] = p2[2] = p3[2] = (viewer[2] > 0) ? zmin : zmax ;
    p0[0] = p1[0] = xmin ;  p2[0] = p3[0] = xmax ;
    p0[1] = p2[1] = ymin ;  p1[1] = p3[1] = ymax ;
    draw_cube_grid(p0,p1,p2,p3, tex_nj-1,tex_ni-1);
  }
  ::glEnd();
}
//_____________________________________________________________________________





//_____________________________________________________________________________
// draw the wire cube with front edges shaded
void draw_cube()
//-----------------------------------------------------------------------------
{
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
//_____________________________________________________________________________






//_____________________________________________________________________________
// window resizing
void reshape( int x, int y )
//-----------------------------------------------------------------------------
{
  ::glutSetWindow(main_window);

  // get viewport
  int tx, ty, tw, th;
  GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
  ::glViewport( tx, ty, tw, th );

  // sets the window trackball
  mouse_rot.set_w( tw ) ;
  mouse_rot.set_h( th ) ;
  mouse_mv .set_w( tw ) ;
  mouse_mv .set_h( th ) ;
  mouse_zm .set_w( tw ) ;
  mouse_zm .set_h( th ) ;

  // sets the projection matrix
  ::glMatrixMode(GL_PROJECTION);
  ::glLoadIdentity();

  if( ortho )
  {
    if( th > tw )
      ::glOrtho( -1.3, 1.3, -1.3*(double)th/tw, 1.3*(double)th/tw, -4,10 ) ;
    else
      ::glOrtho( -1.3*(double)tw/th, 1.3*(double)tw/th, -1.3, 1.3, -4, 10 ) ;
  }
  else
    ::gluPerspective( 45.0, th>0?(double)tw/th:1.0, 0.5, 10. );

  // sets the viewport
  ::gluLookAt( 0.0f,0.0f,3.0f, 0.0f,0.0f,0.0f, 0.0f,1.0f,0.0f ) ;

  // switch to the modelview matrix
  ::glMatrixMode( GL_MODELVIEW );
  ::glLoadIdentity();

  // redisplay
  ::glutPostRedisplay();
}
//_____________________________________________________________________________




//_____________________________________________________________________________
//_____________________________________________________________________________




//_____________________________________________________________________________
// draw the transfer function
void draw_tranfer()
//-----------------------------------------------------------------------------
{
  ::glutSetWindow(main_window);

  // reset the matrix for 2d view
  ::glMatrixMode(GL_PROJECTION);
  ::glPushMatrix();
  ::glLoadIdentity() ;
  ::glMatrixMode(GL_MODELVIEW );
  ::glPushMatrix();
  ::glLoadIdentity() ;

  // transfer function position
  double dx = 1.9 / vsvr.tf_size() ;
  double dy = 0.07 ;
  double x  = -0.95 ;
  double y  = -0.95 ;

  /// draws a bar of the color with transparency
  float r, g, b, a ;
  ::glColor4f(1.0f, 1.0f, 1.0f, 1.0f );
  ::glBegin( GL_QUADS ) ;
  {
    for( int i=0 ; i<vsvr.tf_size() ; ++i, x+=dx )
    {
      vsvr.tf_get( i, r,g,b,a ) ;
      a /= opacity ;
      ::glColor3f ( a*r,a*g,a*b ) ;
      ::glVertex2d(  x  , y   ) ;
      ::glVertex2d( x+dx, y   ) ;
      ::glVertex2d( x+dx,y+dy ) ;
      ::glVertex2d(  x  ,y+dy ) ;
    }
  }
  ::glEnd() ;

  /// draws a frame of the color without transparency
  x  = -0.95 ;
  ::glLineWidth( 3.0 ) ;
  ::glBegin( GL_LINES );
  {
    for( int i=0 ; i<vsvr.tf_size() ; ++i, x+=dx )
    {
      vsvr.tf_get( i, r,g,b,a ) ;
      ::glColor3f ( r,g,b ) ;
      ::glVertex2d(  x  , y   ) ;
      ::glVertex2d( x+dx, y   ) ;

      ::glVertex2d( x+dx,y+dy ) ;
      ::glVertex2d(  x  ,y+dy ) ;
    }
  }
  ::glEnd() ;
  ::glLineWidth( 1.0 ) ;

  // back to 3d matrix
  ::glMatrixMode(GL_PROJECTION);
  ::glPopMatrix();
  ::glMatrixMode(GL_MODELVIEW );
  ::glPopMatrix();
}
//_____________________________________________________________________________






//_____________________________________________________________________________
// main drawing function
void display()
//-----------------------------------------------------------------------------
{
  // clear screen
  ::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // transformation matrix
  ::glLoadIdentity();
  ::glTranslatef( obj_pos[0], obj_pos[1], -obj_pos[2] );
  ::glMultMatrixf( view_rotate );
  if( ortho ) ::glScalef( -obj_pos[2], -obj_pos[2], -obj_pos[2] );

  // draw the external cube
  if( show_cube || show_grid )
  {
    float gl_model[16];
    ::glGetFloatv(GL_MODELVIEW_MATRIX ,(float *)gl_model );
    viewer[0] = gl_model[0*4+2] ;
    viewer[1] = gl_model[1*4+2] ;
    viewer[2] = gl_model[2*4+2] ;
    if( show_cube ) draw_cube() ;
    if( show_grid ) draw_grid() ;
  }

  // draw the volume
  ::glPushMatrix();
  {
    // object to grid coordinates
    ::glTranslatef( xmin, ymin, zmin );
    ::glScalef( (xmax-xmin)/vsvr.tex_ni(), (ymax-ymin)/vsvr.tex_nj(), (zmax-zmin)/vsvr.tex_nk() );

    // reload texture after important modifications
    if( force_reload )
      force_reload = !vsvr.gl_render( (motion_type == -1) ? static_nslices : motion_nslices ) ;

    // just draw the texture
    else
      force_reload = !vsvr.gl_redisplay( (motion_type == -1) ? static_nslices : motion_nslices ) ;
  }
  ::glPopMatrix();


  // draws the transfer function
  if( show_tf && vsvr.tf_size() > 0 )
    draw_tranfer() ;

  // next frame
  ::glutSwapBuffers();
}
//_____________________________________________________________________________



//_____________________________________________________________________________
//_____________________________________________________________________________



//_____________________________________________________________________________
//
void write_ppm()
//-----------------------------------------------------------------------------
{
  int i = strlen(out_filename.c_str()) ;
  if( i < 1 ) return ;

  // force the filename extension
  if( out_filename[i-4] != '.' ) out_filename += ".ppm" ;
  else if( strcmp( out_filename.c_str() + i-3, "ppm" ) ) { out_filename[i-3] = 'p' ; out_filename[i-2] = 'p' ; out_filename[i-1] = 'm' ; }

  // opens the output file
  FILE *ppm_file = fopen( out_filename.c_str(), "w" ) ;
  if( !ppm_file ) return ;

  // gets the viewport
  int tx, ty, tw, th;
  ::glutSetWindow(main_window);
  GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );

  // genmerate the openGL buffer
  int buf_size = (tw * th * 3) ;
  unsigned char *buffer = new unsigned char[buf_size] ;
  force_reload = true ;
  display () ;
  ::glFinish() ;
  ::glPixelStorei( GL_PACK_ALIGNMENT,  1 );
  ::glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
  ::glPixelStorei( GL_PACK_SKIP_ROWS,  0 );
  ::glPixelStorei( GL_PACK_SKIP_PIXELS,0 );
  ::glReadPixels(tx, ty, tw, th, GL_RGB, GL_UNSIGNED_BYTE, buffer);

  // write the ppm header
  fprintf(ppm_file, "P6\n%d %d\n", tw, th);
  fprintf(ppm_file, "# %s (VSVR by Thomas Lewiner)\n", (const char*)out_filename.c_str() ) ;

  // sets the maximal color
  int max_color = 0 ;
  for( i = 0 ; i < buf_size ; ++i )
    if( buffer[i] > max_color ) max_color = buffer[i] ;
  if( max_color > 255 )
  {
    printf( "bad maximal color %d\n", max_color ) ;
    max_color = 255 ;
  }
  fprintf(ppm_file, "%d\n", max_color );

  // write file
  for( int h = th-1 ; h >= 0 ; --h )
  {
    for( int w = 0 ; w < tw ; ++w )
    {
      unsigned char *ptr = buffer + ((tw*h+w)*3) ;
      fputc( ptr[0], ppm_file ) ;
      fputc( ptr[1], ppm_file ) ;
      fputc( ptr[2], ppm_file ) ;
    }
  }

  // close the file
  fclose(ppm_file);
  delete [] buffer ;
}
//_____________________________________________________________________________
