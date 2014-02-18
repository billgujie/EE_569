/*
 * contrastLOCAL.cpp
 *
 *  Created on: Sep 13, 2012
 *      Author: gu
 */

// This sample code reads in image data from a RAW image file and
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

#define SIZEW 400; // default size
#define SIZEH 300;

int contrastLOCAL(int input);//square root contrast enhancement

int main(int argc, char *argv[]) //command line arguments

{
	// Define file pointer and variables
	FILE *file;

	// Check for proper syntax
	if (argc < 5){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name[0] input_image.raw[1] output_image.raw[2] [SizeW = 256][3] [SizeH = 256][4]" << endl;
		return 0;
	}

	// Allocate image data array
	unsigned char Imagedata[400][300];//size of unsigned char is 0-255
	unsigned char ImageQ1b1[400][300];

	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), 400*300, file);
	fclose(file);

	//calculation
	int temp;
	for (int i=0; i<=399; i++){
		for (int j=0; j<=299; j++){
			temp=Imagedata[i][j];
			ImageQ1b1[i][j]=contrastLOCAL(temp);
		}
	}

	// Write image data (filename specified by second argument) from image data matrix
	string savename=argv[2];
	// Save into different file names based on argument input
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen((savename+"LOCAL.raw").c_str(),"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImageQ1b1, sizeof(unsigned char), 400*300, file);
	fclose(file);

	return 0;
}

int contrastLOCAL(int input)
{
	double temp;
	temp= double(input);
	if (temp>=130){
		return static_cast<int>(temp);
	}
	else{
		temp=sqrt (temp/256)*256;
		return static_cast<int>(temp);
	}

}






