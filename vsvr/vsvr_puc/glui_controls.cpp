/**
 * @file    glui_controls.cpp
 * @author  Thomas Lewiner <thomas.lewiner@polytechnique.org>
 * @author  Math Dept, PUC-Rio
 * @version 0.1
 * @date    30/05/2006
 *
 * @brief   VSVR graphical interface: interface controls
 */
//________________________________________________


#if !defined(WIN32) || defined(__CYGWIN__)
#pragma implementation "glui_defs.h"
#endif // WIN32


#include <GL/glui.h> // openGL user interface
#include "glui_defs.h"


//_____________________________________________________________________________
// declarations of this file

// main glui class: (right) side panel
GLUI *glui_side   = NULL ;

// bottom panel
GLUI *glui_bottom = NULL ;


//-----------------------------------------------------------------------------

// control events callback
void control_cb( int control ) ;

// create side panel
void create_side_panel() ;

// create bottom panel
void create_bottom_panel() ;

//_____________________________________________________________________________




//_____________________________________________________________________________
// control events callback
void control_cb( int control )
//-----------------------------------------------------------------------------
{
  FILE *fp     ;
//  float val[3] ;

  ::glutSetWindow(main_window);
  switch( control )
  {
  // changed the texture implicit function input
  case TEX_SET_FUN_ID   :
    if( tex_fun_choice > 0 && tex_fun_choice < TEX_FUN_NB )
      strcpy( tex_formula, tex_fun_def[tex_fun_choice] ) ;
    glui_side->sync_live() ;
    tex_set_formula() ;
    break ;


  // changed the opacity function
  case TF_SEL_FUN_ID   :
    if( tf_fun_choice > 0 && tf_fun_choice < TF_FUN_NB )
      strcpy( tf_fun_formula, tf_fun_def[tf_fun_choice] ) ;
    glui_bottom->sync_live() ;
    // no break
  case TF_SET_FUN_ID   :
    tf_set_formula() ;
    break ;


  // input texture files
  case TEX_READ_CSG_ID   :  tex_fun_choice = 0 ;  tex_read_csg () ;  break ;
  case TEX_READ_ISO_ID   :  tex_fun_choice = 0 ;  tex_read_iso () ;  break ;
  case TEX_READ_VOL_ID   :  tex_fun_choice = 0 ;  tex_read_vol () ;  break ;
  case TEX_READ_RAW_ID   :  tex_fun_choice = 0 ;  tex_read_raw () ;  break ;
  case TEX_READ_TIFF_ID  :  tex_fun_choice = 0;   tex_read_tiff (); break;
  // output texture files
  case TEX_WRITE_ISO_ID  :  tex_write_iso() ;  break ;

  // output image files
  case TEX_WRITE_PPM_ID  :  write_ppm()     ;  break ;

  // load/save viewpoint
  case SAVE_VIEWPORT_ID   :
    fp = fopen( "viewport.txt", "w" ) ;
    if( !fp ) break ;
    fprintf( fp, "rotate:\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\n",
              view_rotate[ 0], view_rotate[ 1], view_rotate[ 2], view_rotate[ 3],
              view_rotate[ 4], view_rotate[ 5], view_rotate[ 6], view_rotate[ 7],
              view_rotate[ 8], view_rotate[ 9], view_rotate[10], view_rotate[11],
              view_rotate[12], view_rotate[13], view_rotate[14], view_rotate[15] ) ;
    fprintf( fp, "translate:\t%f %f %f\n", obj_pos[0], obj_pos[1], obj_pos[2] ) ;
    fclose( fp ) ;
    break ;
  case LOAD_VIEWPORT_ID   :
    fp = fopen( "viewport.txt", "r" ) ;
    if( !fp ) break ;
    fscanf( fp, "rotate: %f %f %f %f  %f %f %f %f  %f %f %f %f  %f %f %f %f ",
              view_rotate +  0, view_rotate +  1, view_rotate +  2, view_rotate +  3,
              view_rotate +  4, view_rotate +  5, view_rotate +  6, view_rotate +  7,
              view_rotate +  8, view_rotate +  9, view_rotate + 10, view_rotate + 11,
              view_rotate + 12, view_rotate + 13, view_rotate + 14, view_rotate + 15 ) ;
    fscanf( fp, "translate: %f %f %f ", obj_pos + 0, obj_pos + 1, obj_pos + 2 ) ;
    fclose( fp ) ;

    objects_rot->sync_live(0,1) ;
    objects_mv ->sync_live(0,1) ;
    objects_zm ->sync_live(0,1) ;
    mouse_rot  . sync_live(0,1) ;
    mouse_mv   . sync_live(0,1) ;
    mouse_zm   . sync_live(0,1) ;
    break ;

  // changed the texture
  case FORCE_RELOAD_ID   : force_reload = true ;  break ;

  // orthographic/perspective projection
  case SET_PROJ_ID       :  reshape   (0,0) ;  break ;

  // quit
  case EXIT_ID            :  exit(0) ;

  default :  break ;
  }
  ::glutPostRedisplay();
}
//_____________________________________________________________________________




//_____________________________________________________________________________
//_____________________________________________________________________________



//_____________________________________________________________________________
// create side panel
void create_side_panel()
//-----------------------------------------------------------------------------
{
  GLUI_Rollout     *roll ;
  GLUI_Panel       *pan  ;
  GLUI_Listbox     *list ;
  GLUI_EditText    *text ;

  glui_side   = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_RIGHT  );

  //--------------------------------------------------//
  // Texture input
  roll = glui_side->add_rollout( "Texture", true );

  // Input : functions
  text = glui_side->add_edittext_to_panel( roll, "Formula", GLUI_EDITTEXT_TEXT, tex_formula ) ;
  text->set_w(200) ;
  list = glui_side->add_listbox_to_panel( roll, "Implicit Functions:", &tex_fun_choice, TEX_SET_FUN_ID, control_cb );
  int i ;
  for( i=0; i<TEX_FUN_NB; ++i ) list->add_item( i, tex_fun_list[i] );

  // Input : file
  text = glui_side->add_edittext_to_panel( roll, "File name", in_filename ) ;
  text->set_w(200) ;
  pan  = glui_side->add_panel_to_panel( roll, "", GLUI_PANEL_NONE ) ;
  glui_side->add_button_to_panel( pan, "Open ISO", TEX_READ_ISO_ID, control_cb ) ;
  glui_side->add_button_to_panel( pan, "Open CSG", TEX_READ_CSG_ID, control_cb ) ;
  glui_side->add_column_to_panel( pan, false );
  glui_side->add_button_to_panel( pan, "Open VOL", TEX_READ_VOL_ID, control_cb ) ;
  glui_side->add_button_to_panel( pan, "Open RAW", TEX_READ_RAW_ID, control_cb ) ;
  glui_side->add_button_to_panel( pan, "Open TIFF", TEX_READ_TIFF_ID, control_cb ) ;

  //--------------------------------------------------//
  // Texture dimension
  roll = glui_side->add_rollout( "Dimensions", true );
  tex_ni = tex_nj = tex_nk = 64 ;
  glui_side->add_edittext_to_panel( roll, "ni", GLUI_EDITTEXT_INT, &tex_ni, TEX_SET_FUN_ID, control_cb ) ;
  glui_side->add_edittext_to_panel( roll, "nj", GLUI_EDITTEXT_INT, &tex_nj, TEX_SET_FUN_ID, control_cb ) ;
  glui_side->add_edittext_to_panel( roll, "nk", GLUI_EDITTEXT_INT, &tex_nk, TEX_SET_FUN_ID, control_cb ) ;

  // ratios
  roll = glui_side->add_rollout( "Position", true );
  xmin = ymin = zmin = -1.0f ;
  xmax = ymax = zmax =  1.0f ;
  pan  = glui_side->add_panel_to_panel( roll, "", GLUI_PANEL_NONE ) ;
  text = glui_side->add_edittext_to_panel( pan, "xmin", GLUI_EDITTEXT_FLOAT, &xmin, TEX_SET_FUN_ID, control_cb ) ;  text->set_w(50) ;
  text = glui_side->add_edittext_to_panel( pan, "ymin", GLUI_EDITTEXT_FLOAT, &ymin, TEX_SET_FUN_ID, control_cb ) ;  text->set_w(50) ;
  text = glui_side->add_edittext_to_panel( pan, "zmin", GLUI_EDITTEXT_FLOAT, &zmin, TEX_SET_FUN_ID, control_cb ) ;  text->set_w(50) ;
  glui_side->add_column_to_panel( pan, false );
  text = glui_side->add_edittext_to_panel( pan, "xmax", GLUI_EDITTEXT_FLOAT, &xmax, TEX_SET_FUN_ID, control_cb ) ;  text->set_w(50) ;
  text = glui_side->add_edittext_to_panel( pan, "ymax", GLUI_EDITTEXT_FLOAT, &ymax, TEX_SET_FUN_ID, control_cb ) ;  text->set_w(50) ;
  text = glui_side->add_edittext_to_panel( pan, "zmax", GLUI_EDITTEXT_FLOAT, &zmax, TEX_SET_FUN_ID, control_cb ) ;  text->set_w(50) ;

  //--------------------------------------------------//
  // export
  roll = glui_side->add_rollout( "Export texture", true );
  text = glui_side->add_edittext_to_panel( roll, "File name", out_filename ) ;
  text->set_w(200) ;
  pan  = glui_side->add_panel_to_panel( roll, "", GLUI_PANEL_NONE ) ;
  glui_side->add_button_to_panel( pan, "Save ISO", TEX_WRITE_ISO_ID, control_cb ) ;
  glui_side->add_column_to_panel( pan, false );
  glui_side->add_button_to_panel( pan, "Save PPM", TEX_WRITE_PPM_ID, control_cb ) ;
  pan  = glui_side->add_panel_to_panel( roll, "", GLUI_PANEL_NONE ) ;
  glui_side->add_button_to_panel( pan, "Save View", SAVE_VIEWPORT_ID, control_cb ) ;
  glui_side->add_column_to_panel( pan, false );
  glui_side->add_button_to_panel( pan, "Load View", LOAD_VIEWPORT_ID, control_cb ) ;

  //--------------------------------------------------//
  // quit
  glui_side->add_button( "Quit", EXIT_ID, control_cb );
}
//_____________________________________________________________________________





//_____________________________________________________________________________
// create bottom panel
void create_bottom_panel()
//-----------------------------------------------------------------------------
{
  GLUI_Rollout     *roll ;
  GLUI_Listbox     *list ;
  GLUI_EditText    *text ;

  glui_bottom = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_BOTTOM );

  //--------------------------------------------------//
  // position
  objects_rot = glui_bottom->add_rotation( "Objects", view_rotate );
  objects_rot->set_spin( 1.0f );
  glui_bottom->add_column( false );
  objects_mv = glui_bottom->add_translation( "Objects XY", GLUI_TRANSLATION_XY, obj_pos );
  objects_mv->set_speed( .005f );
  glui_bottom->add_column( false );
  objects_zm = glui_bottom->add_translation( "Objects Z", GLUI_TRANSLATION_Z, &obj_pos[2] );
  objects_zm->set_speed( .005f );

  // view element
  glui_bottom->add_column( true );
  glui_bottom->add_checkbox( "Ortho"   , &ortho, SET_PROJ_ID, control_cb );
  glui_bottom->add_checkbox( "Cube"    , &show_cube, -1, control_cb );
  glui_bottom->add_checkbox( "Grid"    , &show_grid, -1, control_cb );
  glui_bottom->add_checkbox( "Transfer", &show_tf  , -1, control_cb );


  //--------------------------------------------------//
  // Texture input
  glui_bottom->add_column( true );
  roll = glui_bottom->add_rollout( "Transfer Function", true );

  // Input : colormap
  list = glui_bottom->add_listbox_to_panel( roll, "Colormap:", &tf_colmap, TF_SET_FUN_ID, control_cb );
  for( int i=0; i<CMAP_NB; ++i ) list->add_item( i, cmap_names[i] );
  glui_bottom->add_checkbox_to_panel( roll, "Invert"   , &tf_inv, TF_SET_FUN_ID, control_cb );
  glui_bottom->add_checkbox_to_panel( roll, "Symmetric", &tf_sym, TF_SET_FUN_ID, control_cb );

  glui_bottom->add_column_to_panel( roll, false );

  // Input : transfer
  list = glui_bottom->add_listbox_to_panel( roll, "Opacity:", &tf_fun_choice, TF_SEL_FUN_ID, control_cb );
  for( int i=0; i<TF_FUN_NB; ++i ) list->add_item( i, tf_fun_list[i] );

  tf_fun_choice = 6 ;
  text = glui_bottom->add_edittext_to_panel( roll, "Formula", GLUI_EDITTEXT_TEXT, tf_fun_formula, TF_SET_FUN_ID, control_cb ) ;
  text->set_w(150) ;



  //--------------------------------------------------//
  // Redering tunings
  glui_bottom->add_column( true );
  glui_bottom->add_edittext( "Nslices static" , GLUI_EDITTEXT_INT  , &static_nslices, FORCE_RELOAD_ID, control_cb ) ;
  glui_bottom->add_edittext( "Nslices motion" , GLUI_EDITTEXT_INT  , &motion_nslices, FORCE_RELOAD_ID, control_cb ) ;
  glui_bottom->add_edittext( "Opacity factor" , GLUI_EDITTEXT_FLOAT, &opacity       , TF_SET_FUN_ID  , control_cb ) ;
}
//_____________________________________________________________________________
