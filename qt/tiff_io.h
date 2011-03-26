//
//=======================================================================
// Copyright 2010 Institute PICB.
// Authors: Hang Xiao, Axel Mosig
// Data : July 13, 2010
//=======================================================================
//
#ifndef MYALGORITHMS_H_H
#define MYALGORITHMS_H_H


unsigned char* readtiff(char *myfile, int *width, int *height, int *depth,
						int *channels);
void writetiff( char *myfile, unsigned char *img, int channels,\
			   int width, int height, int depth );

#endif
