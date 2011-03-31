/**
 * @file    glui_tf_gen.h
 * @author  Thomas Lewiner <thomas.lewiner@polytechnique.org>
 * @author  Math Dept, PUC-Rio
 * @version 0.1
 * @date    30/05/2006
 *
 * @brief   VSVR graphical interface: texture generator and i/o
 */
//________________________________________________


#if !defined(WIN32) || defined(__CYGWIN__)
#pragma implementation "glui_defs.h"
#endif // WIN32


#include <stdio.h>   // file reading
#include "colormaps.h"
#include "fparser.h"
#include "glui_defs.h"




//_____________________________________________________________________________
// declarations of this file


// color map to use
int   tf_colmap = 8 ;
int   tf_inv    = 0 ;
int   tf_sym    = 0 ;

// opacity factor
float opacity   = 0.1f ;

// transfer function name
const char *tf_fun_list[TF_FUN_NB] =
{
  ""            , // for user to type
  "Constant"    ,
  "Linear"      ,
  "Reverse"     ,
  "Quadratic"   ,
  "Rev Quad"    ,
  "Gaussian"    ,
  "Rev Gauss"   ,
} ;

// transfer function definition
const char *tf_fun_def [TF_FUN_NB] =
{
  ""            , // for user to type
  "1",
  "x",
  "1-x",
  "4*x*(1-x)",
  "1-4*x*(1-x)",
  "exp(-100*(x-.5)^2)",
  "1-exp(-100*(x-.5)^2)"
} ;

// chosen functions
int   tf_fun_choice ;

// formulas of the transfer and opacity functions
char  tf_fun_formula[512] = "(type or choose from list)" ;


// colormap names
const char *cmap_names[CMAP_NB] =
{
  "hsv"        ,
  "autumn"     ,
  "bone"       ,
  "colorcube"  ,
  "cool"       ,
  "copper"     ,
  "flag"       ,
  "hot"        ,
  "jet"        ,
  "lines"      ,
  "pink"       ,
  "prism"      ,
  "spring"     ,
  "summer"     ,
  "winter"
} ;

// colormaps
const float *cmaps[CMAP_NB] =
{
  color_hsv      ,
  color_autumn   ,
  color_bone     ,
  color_colorcube,
  color_cool     ,
  color_copper   ,
  color_flag     ,
  color_hot      ,
  color_jet      ,
  color_lines    ,
  color_pink     ,
  color_prism    ,
  color_spring   ,
  color_summer   ,
  color_winter
} ;


//-----------------------------------------------------------------------------

// compute the transfer function
void tf_set_formulas() ;

//_____________________________________________________________________________





//_____________________________________________________________________________
//
void tf_set_formula()
//-----------------------------------------------------------------------------
{
  // Parse transfer function formula
  if( tf_fun_formula[0] == '\0' ) return ;
  FunctionParser fparser ;
  int res = fparser.Parse( (const char*)tf_fun_formula, "x" ) ;
  if( fparser.GetParseErrorType() != FunctionParser::NO_SYNTAX_ERROR )
  {
    printf( "transfer formula error: %s\n\t%s\n\t% *d\n", fparser.ErrorMsg(), tf_fun_formula, res, 1 ) ;
    return ;
  }


  // Retreives the color map
  if( tf_colmap < 0 || tf_colmap >= CMAP_NB ) return ;
  const float *colmap = cmaps[tf_colmap] ;

  // allocate the transfer function
  int tf_size = tf_sym ? 512 : 256 ;
  vsvr.tf_set_intern() ;
  vsvr.tf_set_size( tf_size ) ;
  vsvr.tf_alloc() ;

  // fills the texture
  float  x = 0.0f ;
  float dx = 1.0f / (tf_size-1) ;
  for( int i = 0 ; i < tf_size ; ++i, x += dx )
  {
    int k = i ;
    // symmetric
    if( i > 255 ) k = 511 - i ;
    // inverse
    if( tf_inv ) k = 255 - k ;
    const float *col = colmap + 3*k ;

    float a = fparser.Eval( &x ) ;
    if( a < 0.0f ) a = 0.0f ;
    if( a > 1.0f ) a = 1.0f ;

    vsvr.tf_set( i, col[0], col[1], col[2], a*opacity ) ;
  }

  force_reload = true ;
}
//_____________________________________________________________________________
