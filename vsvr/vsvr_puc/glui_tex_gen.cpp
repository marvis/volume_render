/**
 * @file    glui_tex_gen.h
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
#include <assert.h>
#include "csg.h"
#include "tiff_io.h"
#include "fparser.h"
#include "glui_defs.h"




//_____________________________________________________________________________
// declarations of this file

// the renderer
VSVR  vsvr ;

// texture dimensions
int   tex_ni = 64, tex_nj = 64, tex_nk = 64 ;

// texture grid position
float xmin = -1.0f,xmax = 1.0f,  ymin = -1.0f, ymax = 1.0f,  zmin = -1.0f, zmax = 1.0f ;

// implicit function names
const char *tex_fun_list[TEX_FUN_NB] =
{
  ""            , // for user to type
  "Sphere"      ,
  "Ellipsoid"   ,
  "Hyperboloid" ,
  "Plane"       ,
  "Cubic"       ,
  "Cushin"      ,
  "Cassini"     ,
  "Blooby"      ,
  "Chair"       ,
  "Cyclide"     ,
  "2 Spheres"   ,
  "2 Torii"     ,
  "Heart"
} ;

// implicit function definitions
const char *tex_fun_def [TEX_FUN_NB] =
{
  ""            , // for user to type
  "x^2+y^2+z^2-0.49",
  "2*x^2+y^2+z^2-0.49",
  "2*x^2-y^2-z^2-0.49",
  "x+y+z",
  "4*y^2-8*x^3+2*x",
  "(1.5*z)^2*(1.5*x)^2 - (1.5*z)^4 - 2*(1.5*z)*(1.5*x)^2 + 2*(1.5*z)^3 + (1.5*x)^2 - (1.5*z)^2 - ((1.5*x)^2 - (1.5*z))*((1.5*x)^2 - (1.5*z)) - (1.5*y)^4 - 2*(1.5*x)^2*(1.5*y)^2 - (1.5*y)^2*(1.5*z)^2 + 2*(1.5*y)^2*(1.5*z) + (1.5*y)^2",
  "((1.7*x)^2 + (1.7*y)^2 + (1.7*z)^2 + 0.45^2)*((1.7*x)^2 + (1.7*y)^2 + (1.7*z)^2 + 0.45^2) - 16*0.45^2*((1.7*x)^2 + (1.7*z)^2) - 0.25",
  "(3*x)^4 - 45*x^2+ (3*y)^4 - 45*y^2 + (3*z)^4 - 45*z^2 + 11.8",
  "((5*x)^2+(5*y)^2+(5*z)^2-0.95*25)*((5*x)^2+(5*y)^2+(5*z)^2-0.95*25)-0.8*(((5*z)-5)^2-2*(5*x)^2)*(((5*z)+5)^2-2*(5*y)^2)",
  "(25 - (6.9)^2)*(25 - (2.9)^2)*((10*x+4)^4+(10*y)^4+(10*z)^4)+ 2*((25 - (6.9)^2 )*(25 - (2.9)^2) * ((10*x+4)^2*(10*y)^2+(10*x+4)^2*(10*z)^2+(10*y)^2*(10*z)^2))+ 18*((21+4.9^2)* (4*(10*x+4)+9))*((10*x+4)^2+(10*y)^2+(10*z)^2)+ 4*3^4*(2*(10*x+4))*(-9+2*(10*x+4))+4*3^4*4.9^2*(10*y)^2+3^8",
  "((x-0.31)^2+(y-0.31)^2+(z-0.31)^2-0.263) * ((x+0.3)^2+(y+0.3)^2+(z+0.3)^2-0.263)",
  "( ( (8*x)^2 + (8*y-2)^2 + (8*z)^2 + 16 - 1.85*1.85 ) * ( (8*x)^2 + (8*y-2)^2 + (8*z)^2 + 16 - 1.85*1.85 ) - 64 * ( (8*x)^2 + (8*y-2)^2 ) ) * ( ( (8*x)^2 + ((8*y-2)+4)*((8*y-2)+4) + (8*z)^2 + 16 - 1.85*1.85 ) * ( (8*x)^2 + ((8*y-2)+4)*((8*y-2)+4) + (8*z)^2 + 16 - 1.85*1.85 ) - 64 * ( ((8*y-2)+4)*((8*y-2)+4) + (8*z)^2 ) ) + 1025",
  "(2*(1.3*x)^2+(1.3*y)^2+(1.3*z)^2-1)^3-(1/10)*(1.3*x)^2*(1.3*z)^3-(1.3*y)^2*(1.3*z)^3"
} ;


// chosen implicit function
int   tex_fun_choice = 0 ;

// formula for the texture computation from an implicit function
char  tex_formula[512] =  "(type or choose from list)" ;


//-----------------------------------------------------------------------------

// name of the import file
GLUI_String in_filename  = "(filename.iso)" ;

// name of the export file
GLUI_String out_filename = "(filename.ppm)" ;


//-----------------------------------------------------------------------------

// texture computation from the implicit function
void tex_set_formula() ;

// texture computation from a csg model
void tex_read_csg() ;

// texture loading from file
void tex_read_iso() ;
void tex_read_vol() ;
void tex_read_raw() ;
void tex_read_tiff() ;

// texture saving
void tex_write_iso() ;

//_____________________________________________________________________________





//_____________________________________________________________________________
//_____________________________________________________________________________




//_____________________________________________________________________________
// texture computation from a csg model
void tex_read_csg()
//-----------------------------------------------------------------------------
{
  // open CSG file
  if( strlen(in_filename.c_str()) <= 0 ) return ;
  FILE *csgfile = fopen( in_filename.c_str(), "r" ) ;
  if( !csgfile ) return ;

  // parse CSG file
  CSG_Node *csg_root = CSG_Node::parse(csgfile) ;
  fclose( csgfile ) ;

  // allocate the texture
  vsvr.tex_set_intern() ;
  vsvr.tex_set_resolution( tex_ni, tex_nj, tex_nk ) ;
  vsvr.tex_alloc() ;

  // grid to world coordinates
  float dx = (xmax-xmin) / (tex_ni - 1) ;
  float dy = (ymax-ymin) / (tex_nj - 1) ;
  float dz = (zmax-zmin) / (tex_nk - 1) ;

  // fills the texture
  float x = xmin ;
  for( int i = 0 ; i < tex_ni ; ++i, x += dx )
  {
    float y = ymin ;
    for( int j = 0 ; j < tex_nj ; ++j, y += dy )
    {
      float z = zmin ;
      for( int k = 0 ; k < tex_nk ; ++k, z += dz )
      {
        vsvr.tex_set( i,j,k, csg_root->eval( x,y,z ) ) ;
      }
    }
  }

  delete csg_root ;
  force_reload = true ;
}
//_____________________________________________________________________________




//_____________________________________________________________________________
//_____________________________________________________________________________





//_____________________________________________________________________________
// texture computation from the implicit function
void tex_set_formula()
//-----------------------------------------------------------------------------
{
  // Parse formula
  if( tex_formula[0] == '\0' || !strcmp(tex_formula, "(type or choose from list)") ) return ;
  FunctionParser fparser ;
  int res = fparser.Parse( (const char*)tex_formula, "x,y,z" ) ;
  if( fparser.GetParseErrorType() != FunctionParser::NO_SYNTAX_ERROR )
  {
    printf( "tex formula error: %s\n\t%s\n\t% *d\n", fparser.ErrorMsg(), tex_formula, res, 1 ) ;
    return ;
  }

  // allocate the texture
  vsvr.tex_set_intern() ;
  vsvr.tex_set_resolution( tex_ni, tex_nj, tex_nk ) ;
  vsvr.tex_alloc() ;

  // grid to world coordinates
  float dx = (xmax-xmin) / (tex_ni - 1) ;
  float dy = (ymax-ymin) / (tex_nj - 1) ;
  float dz = (zmax-zmin) / (tex_nk - 1) ;

  // fills the texture
  float v[3], &x = v[0],  &y = v[1], &z = v[2] ;
  x = xmin ;
  for( int i = 0 ; i < tex_ni ; ++i, x += dx )
  {
    y = ymin ;
    for( int j = 0 ; j < tex_nj ; ++j, y += dy )
    {
      z = zmin ;
      for( int k = 0 ; k < tex_nk ; ++k, z += dz )
      {
        vsvr.tex_set( i,j,k, fparser.Eval(v) ) ;
      }
    }
  }

  force_reload = true ;
}
//_____________________________________________________________________________



//_____________________________________________________________________________
//_____________________________________________________________________________



//_____________________________________________________________________________
// texture loading from file
void tex_read_iso()
//-----------------------------------------------------------------------------
{
  // open ISO file
  if( strlen(in_filename.c_str()) <= 0 ) return ;
  FILE *isofile = fopen( in_filename.c_str(), "rb" ) ;
  if( !isofile ) return ;

  // read dimensions
  unsigned char buf[sizeof(float)] ;
  fread (buf, sizeof(float), 1, isofile);
  tex_ni = * (int*)buf ;
  fread (buf, sizeof(float), 1, isofile);
  tex_nj = * (int*)buf ;
  fread (buf, sizeof(float), 1, isofile);
  tex_nk = * (int*)buf ;

  // read position
  fread (buf, sizeof(float), 1, isofile);
  xmin = * (float*)buf ;
  fread (buf, sizeof(float), 1, isofile);
  xmax = * (float*)buf ;
  fread (buf, sizeof(float), 1, isofile);
  ymin = * (float*)buf ;
  fread (buf, sizeof(float), 1, isofile);
  ymax = * (float*)buf ;
  fread (buf, sizeof(float), 1, isofile);
  zmin = * (float*)buf ;
  fread (buf, sizeof(float), 1, isofile);
  zmax = * (float*)buf ;

  // allocate the texture
  vsvr.tex_set_intern() ;
  vsvr.tex_set_resolution( tex_ni, tex_nj, tex_nk ) ;
  vsvr.tex_alloc() ;

  // copy the texture
  for( int i = 0 ; i < tex_ni ; ++i )
  {
    for( int j = 0 ; j < tex_nj ; ++j )
    {
      for( int k = 0 ; k < tex_nk ; ++k )
      {
        fread (buf, sizeof(float), 1, isofile);
        vsvr.tex_set( i,j,k, * (float*) buf ) ;
      }
    }
  }

  // close the file
  fclose(isofile) ;

  // update the controls
  glui_side->sync_live() ;

  force_reload = true ;

  printf( "read texture %s of resolution %dx%dx%d and bbox (%.2f,%.2f)x(%.2f,%.2f)x(%.2f,%.2f)\n", (const char*)in_filename.c_str(), tex_ni, tex_nj, tex_nk, xmin, xmax, ymin, ymax, zmin, zmax ) ;
}
//_____________________________________________________________________________




//_____________________________________________________________________________
// texture loading from file: auxiliary function
void swap_bytes( int *intPtr, const unsigned int sizeInts )
//-----------------------------------------------------------------------------
{
  for( unsigned int i = 0; i < sizeInts; ++i )
  {
    unsigned char *bytePtr = (unsigned char *) intPtr;
    *intPtr++ = ((bytePtr[0]<<24) | (bytePtr[1]<<16) | (bytePtr[2]<<8) | bytePtr[3]);
  }
}
//_____________________________________________________________________________




//_____________________________________________________________________________
// texture loading from file (dimensions must be set already...)
void tex_read_raw()
//-----------------------------------------------------------------------------
{
  // open RAW file
  if( strlen(in_filename.c_str()) <= 0 ) return ;
  FILE *rawfile = fopen( in_filename.c_str(), "rb" ) ;
  if( !rawfile ) return ;

  // allocate the texture
  vsvr.tex_set_intern() ;
  vsvr.tex_set_resolution( tex_ni, tex_nj, tex_nk ) ;
  vsvr.tex_alloc() ;

  // copy the texture
  unsigned char buf ;
  for( int k = 0 ; k < tex_nk ; ++k )
  {
    for( int j = 0 ; j < tex_nj ; ++j )
    {
      for( int i = 0 ; i < tex_ni ; ++i )
      {
        fread (&buf, sizeof(unsigned char), 1, rawfile);
        vsvr.tex_set( i,j,k, (float) buf ) ;
      }
    }
  }

  // close the file
  fclose(rawfile) ;

  // update the controls
  glui_side->sync_live() ;

  force_reload = true ;

  printf( "read texture %s using resolution %dx%dx%d and bbox (%.2f,%.2f)x(%.2f,%.2f)x(%.2f,%.2f)\n", (const char*)in_filename.c_str(), tex_ni, tex_nj, tex_nk, xmin, xmax, ymin, ymax, zmin, zmax ) ;
}
//_____________________________________________________________________________

// texture loading from file (dimensions must be set already...)
void tex_read_tiff()
//-----------------------------------------------------------------------------
{
  // open ISO file
  if( strlen(in_filename.c_str()) <= 0 ) return ;

  int channels=0;
  unsigned char* img = readtiff((char *)in_filename.c_str(), &tex_ni, &tex_nj, &tex_nk, &channels);
  assert(channels == 1);

  xmin = - tex_ni / 100.0;
  xmax = - xmin;
  ymin = - tex_nj / 100.0;
  ymax = - ymin;
  zmin = - tex_nk / 100.0;
  zmax = - zmin;

  // allocate the texture
  vsvr.tex_set_intern() ;
  vsvr.tex_set_resolution( tex_ni, tex_nj, tex_nk ) ;
  vsvr.tex_alloc() ;

  // copy the texture
  for( int i = 0 ; i < tex_ni ; ++i )
  {
    for( int j = 0 ; j < tex_nj ; ++j )
    {
      for( int k = 0 ; k < tex_nk ; ++k )
      {
        vsvr.tex_set( i,j,k, (float)img[k * tex_ni * tex_nj + j * tex_ni + i]/255.0) ;
      }
    }
  }

	delete img;
  // update the controls
  glui_side->sync_live() ;

  force_reload = true ;

  printf( "read texture %s of resolution %dx%dx%d and bbox (%.2f,%.2f)x(%.2f,%.2f)x(%.2f,%.2f)\n", (const char*)in_filename.c_str(), tex_ni, tex_nj, tex_nk, xmin, xmax, ymin, ymax, zmin, zmax ) ;
}



//_____________________________________________________________________________
// texture loading from file
//-----------------------------------------------------------------------------
#define VOL_MAGIC_NUMBER_OLD 192837465
#define VOL_MAGIC_NUMBER_NEW 192837466
void tex_read_vol()
//-----------------------------------------------------------------------------
{
  // open VOL file
  if( strlen(in_filename.c_str()) <= 0 ) return ;
  FILE *volfile = fopen( in_filename.c_str(), "rb" ) ;
  if( !volfile ) return ;

  // read magic number
  int magicnumber ;
  fread (&magicnumber, sizeof(int), 1, volfile);
#ifdef  WIN32
  swap_bytes( &magicnumber, sizeof(int)>>2 );
#endif
  if( magicnumber != VOL_MAGIC_NUMBER_OLD && magicnumber != VOL_MAGIC_NUMBER_NEW )
    return ;

  // read dimensions and header
  int header_length, pixel_depth, index_bits ;
  fread( &header_length, sizeof(int), 1, volfile );
  fread( &tex_ni       , sizeof(int), 1, volfile );
  fread( &tex_nj       , sizeof(int), 1, volfile );
  fread( &tex_nk       , sizeof(int), 1, volfile );
  fread( &pixel_depth  , sizeof(int), 1, volfile );
  fread( &index_bits   , sizeof(int), 1, volfile );
#ifdef  WIN32
  swap_bytes( &header_length, sizeof(int)>>2 );
  swap_bytes( &tex_ni       , sizeof(int)>>2 );
  swap_bytes( &tex_nj       , sizeof(int)>>2 );
  swap_bytes( &tex_nk       , sizeof(int)>>2 );
  swap_bytes( &pixel_depth  , sizeof(int)>>2 );
  swap_bytes( &index_bits   , sizeof(int)>>2 );
#endif

  // read scale
  if( magicnumber == VOL_MAGIC_NUMBER_NEW )
  {
    fread( &xmax, sizeof(float), 1, volfile );
    fread( &ymax, sizeof(float), 1, volfile );
    fread( &zmax, sizeof(float), 1, volfile );
#ifdef  WIN32
    swap_bytes( (int*) &xmax, sizeof(float)>>2 );
    swap_bytes( (int*) &ymax, sizeof(float)>>2 );
    swap_bytes( (int*) &zmax, sizeof(float)>>2 );
#endif
    xmin = ymin = zmin = 0.0f ;
    xmax = 0.5f/xmax ;  xmin = -xmax ;
    ymax = 0.5f/ymax ;  ymin = -ymax ;
    zmax = 0.5f/zmax ;  zmin = -zmax ;
  }

  // skips header
  fseek( volfile, header_length, SEEK_SET);

  // allocate the texture
  vsvr.tex_set_intern() ;
  vsvr.tex_set_resolution( tex_ni, tex_nj, tex_nk ) ;
  vsvr.tex_alloc() ;

  // copy the texture
  unsigned char buf ;
  for( int k = 0 ; k < tex_nk ; ++k )
  {
    for( int j = 0 ; j < tex_nj ; ++j )
    {
      for( int i = 0 ; i < tex_ni ; ++i )
      {
        fread (&buf, 1, 1, volfile) ;
        vsvr.tex_set( i,j,k, (float)buf ) ;
      }
    }
  }

  // close the file
  fclose(volfile) ;

  // update the controls
  glui_side->sync_live() ;

  force_reload = true ;

  printf( "read texture %s of resolution %dx%dx%d and bbox (%.2f,%.2f)x(%.2f,%.2f)x(%.2f,%.2f)\n", (const char*)in_filename.c_str(), tex_ni, tex_nj, tex_nk, xmin, xmax, ymin, ymax, zmin, zmax ) ;
}
//_____________________________________________________________________________



//_____________________________________________________________________________
// texture saving
void tex_write_iso()
//-----------------------------------------------------------------------------
{
  int i = strlen(out_filename.c_str()) ;
  if( i < 1 ) return ;

  // force the filename extension
  if( out_filename[i-4] != '.' ) out_filename += ".iso" ;
  else if( strcmp( out_filename.c_str() + i-3, "iso" ) ) { out_filename[i-3] = 'i' ; out_filename[i-2] = 's' ; out_filename[i-1] = 'o' ; }

  // opens the output file
  FILE *iso_file = fopen( out_filename.c_str(), "wb" ) ;
  if( !iso_file ) return ;

  unsigned char buf[sizeof(float)] ;

  // header
  * (int*) buf = vsvr.tex_ni() ;
  fwrite(buf, sizeof(float), 1, iso_file);
  * (int*) buf = vsvr.tex_nj() ;
  fwrite(buf, sizeof(float), 1, iso_file);
  * (int*) buf = vsvr.tex_nk() ;
  fwrite(buf, sizeof(float), 1, iso_file);

  * (float*) buf = xmin ;
  fwrite(buf, sizeof(float), 1, iso_file);
  * (float*) buf = xmax ;
  fwrite(buf, sizeof(float), 1, iso_file);
  * (float*) buf = ymin ;
  fwrite(buf, sizeof(float), 1, iso_file);
  * (float*) buf = ymax ;
  fwrite(buf, sizeof(float), 1, iso_file);
  * (float*) buf = zmin ;
  fwrite(buf, sizeof(float), 1, iso_file);
  * (float*) buf = zmax ;
  fwrite(buf, sizeof(float), 1, iso_file);

  for( int i = 0 ; i < vsvr.tex_ni() ; ++i )
  {
    for( int j = 0 ; j < vsvr.tex_nj() ; ++j )
    {
      for( int k = 0 ; k < vsvr.tex_nk() ; ++k )
      {
        * (float*) buf = (float)vsvr.tex_get( i,j,k ) ;
        fwrite(buf, sizeof(float), 1, iso_file);
      }
    }
  }

  fclose(iso_file) ;

  printf( "wrote texture %s of resolution %dx%dx%d and bbox (%.2f,%.2f)x(%.2f,%.2f)x(%.2f,%.2f)\n", (const char*)out_filename.c_str(), tex_ni, tex_nj, tex_nk, xmin, xmax, ymin, ymax, zmin, zmax ) ;
}
//_____________________________________________________________________________

