/**
 * @file    glui_defs.h
 * @author  Thomas Lewiner <thomas.lewiner@polytechnique.org>
 * @author  Math Dept, PUC-Rio
 * @version 0.1
 * @date    30/05/2006
 *
 * @brief   VSVR graphical interface
 */
//________________________________________________


#ifndef _VSVR_GLUI_H_
#define _VSVR_GLUI_H_

#if !defined(WIN32) || defined(__CYGWIN__)
#pragma interface
#endif // WIN32


#include <GL/glui.h> // openGL user interface
#include "vsvr.h"


#ifdef _DEBUG
#define PRINT_GL_DEBUG  { if( ::glGetError() != GL_NO_ERROR ) printf( "openGL watch at line %d: %s\n", __LINE__, ::gluErrorString( ::glGetError() ) ) ; }
#else  // _DEBUG
#define PRINT_GL_DEBUG  {}
#endif // _DEBUG


//_____________________________________________________________________________
// VSVR component

  /// the renderer
  extern VSVR  vsvr ;

//-----------------------------------------------------------------------------
// texture

  /// texture dimensions: width
  extern int   tex_ni ;
  /// texture dimensions: height
  extern int   tex_nj ;
  /// texture dimensions: depth
  extern int   tex_nk ;

  /// texture grid position: min horizontal
  extern float xmin ;
  /// texture grid position: max horizontal
  extern float xmax ;
  /// texture grid position: min vertical
  extern float ymin ;
  /// texture grid position: max vertical
  extern float ymax ;
  /// texture grid position: min transversal
  extern float zmin ;
  /// texture grid position: max transversal
  extern float zmax ;


  //-----------------------------------------------------------------------------
  /// implicit function examples
  #define TEX_FUN_NB 14

  /// implicit function names
  extern const char *tex_fun_list[TEX_FUN_NB] ;

  /// implicit function definitions
  extern const char *tex_fun_def [TEX_FUN_NB] ;

  /// chosen implicit function
  extern int   tex_fun_choice ;

  /// formula for the texture computation from an implicit function
  extern char  tex_formula[512] ;



/// texture computation from the implicit function
void tex_set_formula() ;

/// texture computation from a csg model
void tex_read_csg() ;

/// texture loading from ISO file
void tex_read_iso() ;
/// texture loading from RAW file (must specify the sizes before)
void tex_read_raw() ;
/// texture loading from VOL file
void tex_read_vol() ;

void tex_read_tiff();

//-----------------------------------------------------------------------------
// transfer function

  /// color map to use
  extern int   tf_colmap ;
  /// reverse colormap
  extern int   tf_inv    ;
  /// symmertric colors
  extern int   tf_sym    ;

  /// opacity factor
  extern float opacity   ;

  //-----------------------------------------------------------------------------
  /// transfer functionsexamples
  #define TF_FUN_NB 8

  /// transfer function name
  extern const char *tf_fun_list[TF_FUN_NB] ;

  /// transfer function definition
  extern const char *tf_fun_def [TF_FUN_NB] ;

  /// chosen transfer function
  extern int   tf_fun_choice ;

  /// formula of the transfer function
  extern char  tf_fun_formula[512] ;


  //-----------------------------------------------------------------------------
  /// colormaps number
  #define CMAP_NB 15

  /// colormap names
  extern const char *cmap_names[CMAP_NB] ;

  /// colormap pointers
  extern const float *cmaps    [CMAP_NB] ;



/// compute the transfer function
void tf_set_formula() ;

/// draw the transfer function
void draw_tranfer() ;


//_____________________________________________________________________________



//_____________________________________________________________________________
// Interface components

//-----------------------------------------------------------------------------
// GLUI class

  /// main window id
  extern int  main_window ;

  /// main glui class: (right) side panel
  extern GLUI *glui_side   ;

  /// bottom panel
  extern GLUI *glui_bottom ;


/// create side panel
void create_side_panel() ;

/// create bottom panel
void create_bottom_panel() ;

/// control events callback
void control_cb( int control ) ;


//-----------------------------------------------------------------------------
// mouse and object movements

  /// motion type (-1 -> no motion, 0 -> rotate, 1 -> zoom, 2 -> translate)
  extern int motion_type ;

  /// window trackball
  extern GLUI_Rotation     mouse_rot   ;
  /// panel trackball
  extern GLUI_Rotation    *objects_rot ;
  /// window translation
  extern GLUI_Translation  mouse_mv    ;
  /// panel translation
  extern GLUI_Translation *objects_mv  ;
  /// window zoom
  extern GLUI_Translation  mouse_zm    ;
  /// panel zoom
  extern GLUI_Translation *objects_zm  ;

  /// number of calls for updating the GLUI control
  extern int ncalls ;



/// init mouse and window controls
void init_trackballs() ;

/// mouse events tracking
void mouse(int button, int button_state, int x, int y ) ;

/// mouse motion tracking
void motion(int x, int y ) ;


//-----------------------------------------------------------------------------
// i/o filenames

  /// name of the import file
  extern GLUI_String in_filename  ;

  /// name of the export file
  extern GLUI_String out_filename ;



/// texture saving
void tex_write_iso() ;

/// screen capture
void write_ppm() ;


//-----------------------------------------------------------------------------
// drawing parameters

  /// display element switch: boundin box
  extern int   show_cube ;
  /// display element switch: projected grid
  extern int   show_grid ;
  /// display element switch: transfer function
  extern int   show_tf   ;

  /// orthographic / perspective projection switch
  extern int ortho ;

  /// object rotation
  extern float view_rotate[16] ;
  /// object translation
  extern float obj_pos    [3 ] ;

  /// viewer position for drawing the grid on the right side of the cube
  extern float viewer[3];

  /// mark parameter modification
  extern bool  force_reload ;

  /// volume rendering setting: fine resolution
  extern int   static_nslices ;
  /// volume rendering setting: interactive resolution
  extern int   motion_nslices ;


/// window resizing
void reshape( int x, int y ) ;

/// main drawing function
void display() ;

//_____________________________________________________________________________




//_____________________________________________________________________________
/// Callback ids
enum
{
  TEX_SET_FUN_ID   ,
  TEX_READ_CSG_ID  ,
  TEX_READ_ISO_ID  ,
  TEX_READ_RAW_ID  ,
  TEX_READ_VOL_ID  ,
  TEX_READ_TIFF_ID ,
  TEX_WRITE_PPM_ID ,
  TEX_WRITE_ISO_ID ,

  TF_SEL_FUN_ID    ,
  TF_SET_FUN_ID    ,

  SAVE_VIEWPORT_ID  ,
  LOAD_VIEWPORT_ID  ,

  FORCE_RELOAD_ID   ,

  SET_PROJ_ID       ,

  EXIT_ID           ,
};
//_____________________________________________________________________________




#endif // _VSVR_GLUI_H_
