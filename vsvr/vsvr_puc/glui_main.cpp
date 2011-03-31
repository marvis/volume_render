/**
 * @file    glui_main.h
 * @author  Thomas Lewiner <thomas.lewiner@polytechnique.org>
 * @author  Math Dept, PUC-Rio
 * @version 0.1
 * @date    30/05/2006
 *
 * @brief   VSVR graphical interface: commandline parser, glew init and main pipeline
 */
//________________________________________________


#if !defined(WIN32) || defined(__CYGWIN__)
#pragma implementation "glui_defs.h"
#endif // WIN32


#include <GL/glew.h> // openGL extension
#include "glui_defs.h"



//_____________________________________________________________________________
// declarations of this file

// main window id
int  main_window  = -1 ;

//-----------------------------------------------------------------------------

// parse command line
bool parse_command_line(int argc, char* argv[]) ;

// idel behaviour: redraw constantly
void idle() ;

//_____________________________________________________________________________




//_____________________________________________________________________________
// parse command line
bool parse_command_line(int argc, char* argv[])
//-----------------------------------------------------------------------------
{
  bool quit = false ;

  for( int i = 1 ; i < argc ; ++i )
  {
    if     ( !strcmp( argv[i], "-ii" ) )
    {
      if( ++i != argc )
      {
        in_filename = argv[i] ;
        control_cb( TEX_READ_ISO_ID ) ;
      }
    }
    if     ( !strcmp( argv[i], "-iv" ) )
    {
      if( ++i != argc )
      {
        in_filename = argv[i] ;
        control_cb( TEX_READ_VOL_ID ) ;
      }
    }
    if     ( !strcmp( argv[i], "-ir" ) )
    {
      if( ++i != argc )
      {
        in_filename = argv[i] ;
        control_cb( TEX_READ_RAW_ID ) ;
      }
    }
    else if( !strcmp( argv[i], "-ic" ) )
    {
      if( ++i != argc )
      {
        in_filename = argv[i] ;
        control_cb( TEX_READ_CSG_ID ) ;
      }
    }
    else if( !strcmp( argv[i], "-tx" ) )
    {
      if( ++i != argc ) { strcpy( tex_formula, argv[i] ) ;  control_cb( TEX_SET_FUN_ID ) ; }
    }
    else if( !strcmp( argv[i], "-tf" ) )
    {
      if( ++i != argc ) { strcpy( tf_fun_formula, argv[i] ) ;  control_cb( TF_SET_FUN_ID ) ; }
    }
    else if( !strcmp( argv[i], "-cl" ) )
    {
      if( ++i != argc ) { tf_colmap = atoi( argv[i] ) ;  control_cb( TF_SET_FUN_ID ) ; }
    }
    else if( !strcmp( argv[i], "-tfi" ) )
    {
      tf_inv = false ;  control_cb( TF_SET_FUN_ID ) ;
    }
    else if( !strcmp( argv[i], "+tfi" ) )
    {
      tf_inv = true ;  control_cb( TF_SET_FUN_ID ) ;
    }
    else if( !strcmp( argv[i], "-tfs" ) )
    {
      tf_sym = false ;  control_cb( TF_SET_FUN_ID ) ;
    }
    else if( !strcmp( argv[i], "+tfs" ) )
    {
      tf_sym = true ;  control_cb( TF_SET_FUN_ID ) ;
    }
    else if( !strcmp( argv[i], "-r" ) )
    {
      if( ++i != argc ) { tex_ni = tex_nj = tex_nk = atoi( argv[i] ) ; }
    }
    else if( !strcmp( argv[i], "-rx" ) )
    {
      if( ++i != argc ) tex_ni = atoi( argv[i] ) ;
    }
    else if( !strcmp( argv[i], "-ry" ) )
    {
      if( ++i != argc ) tex_nj = atoi( argv[i] ) ;
    }
    else if( !strcmp( argv[i], "-rz" ) )
    {
      if( ++i != argc ) tex_nk = atoi( argv[i] ) ;
    }
    else if( !strcmp( argv[i], "-m" ) )
    {
      if( ++i != argc ) { xmin = ymin = zmin = atof( argv[i] ) ; }
    }
    else if( !strcmp( argv[i], "-M" ) )
    {
      if( ++i != argc ) { xmax = ymax = zmax = atof( argv[i] ) ; }
    }
    else if( !strcmp( argv[i], "-mx" ) )
    {
      if( ++i != argc ) { xmin = atof( argv[i] ) ; }
    }
    else if( !strcmp( argv[i], "-Mx" ) )
    {
      if( ++i != argc ) { xmax = atof( argv[i] ) ; }
    }
    else if( !strcmp( argv[i], "-my" ) )
    {
      if( ++i != argc ) { ymin = atof( argv[i] ) ; }
    }
    else if( !strcmp( argv[i], "-My" ) )
    {
      if( ++i != argc ) { ymax = atof( argv[i] ) ; }
    }
    else if( !strcmp( argv[i], "-mz" ) )
    {
      if( ++i != argc ) { zmin = atof( argv[i] ) ; }
    }
    else if( !strcmp( argv[i], "-Mz" ) )
    {
      if( ++i != argc ) { zmax = atof( argv[i] ) ; }
    }
    else if( !strcmp( argv[i], "-lv" ) )
    {
      control_cb( LOAD_VIEWPORT_ID ) ;
    }
    else if( !strcmp( argv[i], "-ns" ) )
    {
      if( ++i != argc ) static_nslices = atoi( argv[i] ) ;
    }
    else if( !strcmp( argv[i], "-nm" ) )
    {
      if( ++i != argc ) motion_nslices = atoi( argv[i] ) ;
    }
    else if( !strcmp( argv[i], "-of" ) )
    {
      if( ++i != argc ) { opacity = atof( argv[i] ) ;  control_cb( TF_SET_FUN_ID ) ; }
    }
    else if( !strcmp( argv[i], "-op" ) )
    {
      if( ++i != argc ) { out_filename = argv[i] ; control_cb( TEX_WRITE_PPM_ID ) ; }
    }
    else if( !strcmp( argv[i], "-oi" ) )
    {
      if( ++i != argc ) { out_filename = argv[i] ; control_cb( TEX_WRITE_ISO_ID ) ; }
    }
    else if( !strcmp( argv[i], "-q" ) )
    {
      quit = true ;
    }
    else if( !strcmp( argv[i], "-h" ) )
    {
      printf( "usage %s [-ii file.iso] [-iv file.vol] [-ir file.raw] [-ic file.csg] [-tx \"implicit fun(x,y,z)\"] [-tf \"trans fun(x)\"] [-cl colmap] [[-+]tfi] [[-+]tfs] [-to \"opa fun(x)\"] [-r[xyz] res] [-m[xyz] min] [-M[xyz] max] [-lv] [-ns nslices] [-nm nslices] [-of opacity] [-op file.ppm] [-oi file.iso] [-q]\n", argv[0] ) ;
    }
  }
  glui_side  ->sync_live() ;
  glui_bottom->sync_live() ;

  return quit ;
}
//_____________________________________________________________________________



//_____________________________________________________________________________
//_____________________________________________________________________________



//_____________________________________________________________________________
// idel behaviour: redraw constantly
void idle()
//-----------------------------------------------------------------------------
{
  if ( ::glutGetWindow() != main_window ) ::glutSetWindow(main_window);
  ::glutPostRedisplay();
}
//_____________________________________________________________________________




//_____________________________________________________________________________
// main
int main(int argc, char* argv[])
//-----------------------------------------------------------------------------
{
  // create main window
  ::glutInit( &argc, argv ) ;
  ::glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
  ::glutInitWindowPosition( 50, 50 );
  ::glutInitWindowSize( 800, 600 );
  main_window = ::glutCreateWindow( "Very Simple Volume Rendering" );


  //--------------------------------------------------//
  // init openGL extensions with GLew
  GLenum err = ::glewInit();
  if( err != GLEW_OK || !GL_EXT_texture3D )
  {
    printf( "Error: %s\n", ::glewGetErrorString(err) ) ;
    return 0 ;
  }
  ::glEnable( GL_DEPTH_TEST );
  ::glClearColor( 0,0,0,1 );
  ::glDisable(GL_TEXTURE_2D);


  //--------------------------------------------------//
  // Create the side and bottom subwindow
#ifdef WIN32
  create_bottom_panel() ;
  create_side_panel  () ;
#else  // WIN32
  create_side_panel  () ;
  create_bottom_panel() ;
#endif // WIN32


  //--------------------------------------------------//
  // set callback functions
  ::glutDisplayFunc ( display );
  ::glutMotionFunc  ( motion );
  ::glutMouseFunc   ( mouse );
  GLUI_Master.set_glutIdleFunc   ( idle    );
  GLUI_Master.set_glutReshapeFunc( reshape );


  //--------------------------------------------------//
  // init trackball
  init_trackballs() ;


  //--------------------------------------------------//
  // sets the transfer function
  control_cb( TF_SEL_FUN_ID ) ;

  //--------------------------------------------------//
  // Parse command line
  bool quit = parse_command_line(argc, argv) ;

  //--------------------------------------------------//
  // GLUT main loop
  if( !quit ) ::glutMainLoop();

  return 0 ;
}
//_____________________________________________________________________________
