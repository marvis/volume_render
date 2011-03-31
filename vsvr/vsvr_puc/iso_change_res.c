//------------------------------------------------
// Isosurface tools
//------------------------------------------------
//
// Isosurface operations
// Version 0.1 - 03/09/2003
//
// Thomas Lewiner: thomas.lewiner@polytechnique.org
// Luiz Velho: lvelho@impa.br
// Math Dept, PUC-Rio
//
//________________________________________________


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


/******************************************************************************
Main program.
******************************************************************************/

void usage(char *) ;

int main(int argc, char *argv[])
{
  char  fon[80];
  char *fn;
  FILE *fi;
  FILE *fo;

  int   verb ;
  int   i,j,k, b, dx,dy,dz, resx, resy, resz ;
  unsigned char buf[sizeof(float)] ;

  verb = 0 ;
  if( argc != 6 )
  {
    usage( argv[0] ) ;
    exit( -1 );
  }

  /* opens the input file */
  fi = fopen( argv[4], "rb" ) ;
  if (fi == NULL)
  {
    fprintf (stderr, "Cannot open file '%s'\n", fn);
    exit (-1);
  }

  /* opens the output file */
  fo = fopen( argv[5], "wb" ) ;
  if (fo == NULL)
  {
    fprintf (stderr, "Cannot create file '%s'\n", fon);
    exit (-1);
  }

  /* extracts the factor */
  dx = atoi( argv[1] ) ;
  dy = atoi( argv[2] ) ;
  dz = atoi( argv[3] ) ;


  /* header */
  fread (buf, sizeof(float), 1, fi);
  resx = * (int*)buf ;
  * (int*) buf = (resx-1)/dx + 1 ;
  fwrite(buf, sizeof(float), 1, fo);
  fread (buf, sizeof(float), 1, fi);
  resy = * (int*)buf ;
  * (int*) buf = (resy-1)/dy + 1 ;
  fwrite(buf, sizeof(float), 1, fo);
  fread (buf, sizeof(float), 1, fi);
  resz = * (int*)buf ;
  * (int*) buf = (resz-1)/dz + 1 ;
  fwrite(buf, sizeof(float), 1, fo);

  fread (buf, sizeof(float), 1, fi);
  fwrite(buf, sizeof(float), 1, fo);
  fread (buf, sizeof(float), 1, fi);
  fwrite(buf, sizeof(float), 1, fo);
  fread (buf, sizeof(float), 1, fi);
  fwrite(buf, sizeof(float), 1, fo);
  fread (buf, sizeof(float), 1, fi);
  fwrite(buf, sizeof(float), 1, fo);
  fread (buf, sizeof(float), 1, fi);
  fwrite(buf, sizeof(float), 1, fo);
  fread (buf, sizeof(float), 1, fi);
  fwrite(buf, sizeof(float), 1, fo);

  printf( "resolution %d x %d x %d\n", resx, resy, resz ) ;

  /* sweep the file */
  b = 0 ;
  for( i = 0 ; i < resx ; ++i )
  {
    for( j = 0 ; j < resy ; ++j )
    {
      b = (i % dx == 0) && (j % dy == 0) ;
      for( k = 0 ; k < resz ; ++k )
      {
        fread (buf, sizeof(float), 1, fi);
        if( b && (k % dz == 0) ) fwrite(buf, sizeof(float), 1, fo);
      }
    }
  }

  fclose( fi ) ;
  fclose( fo ) ;

  return 1;
}


/******************************************************************************
Print out usage information.
******************************************************************************/

void usage(char *progname)
{
  fprintf (stderr, "usage: %s xfact yfact zfact in.iso out.iso\n", progname);
}

