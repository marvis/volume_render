//
//=======================================================================
// Copyright 2010 Institute PICB.
// Authors: Hang Xiao, Axel Mosig
// Data : July 11, 2010
//=======================================================================
//
#include "tiffio.h"
#include "tiff_io.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

//================================================================================
unsigned char *readtiff(char *myfile, int *width, int *height, int *depth, int *channels )
//================================================================================

{
    TIFF* tif = TIFFOpen(myfile, "r");
    if (tif)
    {
		*depth = 0;
		do
		{
			(*depth)++;
			uint16 compression, cc, bpp         ;
			
			//check if we are reading a compressed image. If so
			//return with an error
			TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression);
			
			if ( compression != COMPRESSION_NONE )
			{
				cerr<<"File is not an Uncompressed TIFF image !\n";
			}
			
			//check if the image has 8 bits-per-pixel in each channel
			TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
			
			if ( bpp != 8 )
			{
				cerr<<"File does not have 8 bits per channel !\n";
			}
			
			
			//read the dimensions of the TIFF image
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH , width);//width
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, height);//height
			//determine what type of image is it (color or grayscale)
			TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &cc);
			
			//get the photometric if a grayscale image is
			//being read
			if (cc == 1)
			{
				*channels = 1;
			}
			else
			{
				*channels = 3;
			}
		}while(TIFFReadDirectory(tif));
		
		TIFFClose(tif);
	}
	else
    {
        cerr<<"Unable to open file for reading !\n";
    }
	
	
	tif = TIFFOpen(myfile,"r");
	if(tif)
	{
		unsigned char *img = new unsigned char[(*channels)*(*width)*(*height)*(*depth)];
		int layer = 0;
		do
		{			
			uint16 p, compression, cc, bpp;
			
			//check if we are reading a compressed image. If so
			//return with an error
			TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression);
			
			if ( compression != COMPRESSION_NONE )
			{
				cerr<<"File is not an Uncompressed TIFF image !\n";
			}
			
			//check if the image has 8 bits-per-pixel in each channel
			TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
			
			if ( bpp != 8 )
			{
				cerr<<"File does not have 8 bits per channel !\n";
			}
			
			
			//read the dimensions of the TIFF image
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH , width);//width
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, height);//height
			//determine what type of image is it (color or grayscale)
			TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &cc);
			
			//get the photometric if a grayscale image is
			//being read
			if (cc == 1)
			{
				*channels = 1;
				TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &p);
			}
			else
			{
				*channels = 3;
			}
			
			//create the matrix to hold the image data
			
			//create a dummy array
			int linebytes = (*channels)*(*width);
			unsigned char *buf = new unsigned char[linebytes];
			
			//start reading the data from the TIFF file in scanlines
			int row;
			for (row = 0; row < *height; row++ )
			{
				if(TIFFReadScanline(tif, buf, row, 0) != 1)
				{
					cerr<<"Error reading TIFF scanline !\n";
				}
				memcpy(&img[layer*(*channels)*(*width)*(*height)+row*linebytes], buf, linebytes);
				
			}
			
			delete buf;
		
			layer++;
		} while(TIFFReadDirectory(tif));
			
        TIFFClose(tif);
		
		return img;
    }
	
	return NULL;
}

//===================================================================================================
void writetiff( char *myfile, unsigned char *img, int channels, int width, int height, int depth )
//===================================================================================================
{
    TIFF* tif = TIFFOpen(myfile, "w");
    if (tif)
    {
		int layerIndex;
		for(layerIndex = 0; layerIndex < depth ; layerIndex++)
		{
			//set tiff tags to generate an uncompressed TIFF image
			//SETTAGS( tif, width, height, channels );
			//dimensions of the image
			TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,  width);
			TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
			
			//starting from top-left corner
			TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
			
			//grayscale or color image
			TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, channels);
			TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
			TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			if (channels == 1)
				TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
			else
				if (channels == 3)
					TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
			
			//Miscalleneous Tags
			TIFFSetField( tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField( tif, TIFFTAG_COMPRESSION , COMPRESSION_NONE );
			TIFFSetField( tif, TIFFTAG_ROWSPERSTRIP,
						 TIFFDefaultStripSize(tif , (uint32) -1) );
			TIFFSetField(tif, TIFFTAG_XRESOLUTION, 72);
			TIFFSetField(tif, TIFFTAG_YRESOLUTION, 72);
			TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
			
			//create a dummy array
			int linebytes = channels*width;
			unsigned char *buf = new unsigned char[linebytes];
			TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, linebytes));
			
			//start writing the data into the TIFF file in scanlines
			int row;
			for (row = 0; row < height; row++)
			{
				//check if a grayscale or color image is being written
				memcpy(buf, &img[layerIndex*width*height*channels+row*linebytes], linebytes);
				
				if (TIFFWriteScanline(tif, buf, row, 0) < 0)
				{
					cerr<<"Error in scanline while writing !\n";
				}
			}
			delete buf;
			TIFFWriteDirectory(tif);
		}
        TIFFClose(tif);
    }
    else
    {
        cerr<<"Unable to open file for writing !\n";
    }
}


