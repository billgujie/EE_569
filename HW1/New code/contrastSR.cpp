/*
 * contrastSR.cpp
 *
 *  Created on: Sep 9, 2012
 *      Author: gu
 *
 *      square root scaling
 *      arguments: program_name input_image.raw output_image.raw
 *      ouput: square root scaled image
 *
 */



#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <fstream>

using namespace std;

int contrastSR(int input);//square root contrast enhancement

int main(int argc, char *argv[]) //command line arguments

{
	// Define file pointer and variables
	FILE *file;

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
			ImageQ1b1[i][j]=contrastSR(temp);
		}
	}

	// Write image data (filename specified by second argument) from image data matrix
	string savename=argv[2];
	// Save into different file names based on argument input
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen((savename+"SR.raw").c_str(),"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImageQ1b1, sizeof(unsigned char), 400*300, file);
	fclose(file);

	return 0;
}

int contrastSR(int input)
//	string mode: CLIP for clipping ; AVS for absolute value scaling
{
	double temp;
	temp= double(input);
	temp=sqrt (temp/256)*256;

	return static_cast<int>(temp);
}




