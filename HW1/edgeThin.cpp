/*
 * edgeThin.cpp
 *
 *  Created on: Sep 24, 2012
 *      Author: gu
 */


#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) //command line arguments

{
	// Define file pointer and variables
	FILE *file;

	// Allocate image data array
	float ImagedataGx[256][256];// gradient map x-direction  [use raw gradient map]
	float ImagedataGy[256][256];// .............y-direction  [use raw gradient map]
	unsigned char ImagedataEG[256][256];// edge map previous generated: 0=edge  255=non-edge
	unsigned char ImagedataE[266][266];	// extended edge map
	unsigned char ImagedataOri[256][256];	// Orientation map
	unsigned char ImagedataF[256][256];	// final results
	// 0: x-direction
	// 1: y-direction

	if (!(file=fopen(argv[1],"rb"))) {  // read gradient map 1 [default: x-direction]
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(ImagedataGx, sizeof(float), 256*256, file);
	fclose(file);

	if (!(file=fopen(argv[2],"rb"))) {  // read gradient map 2 [default: y-direction]
		cout << "Cannot open file: " << argv[2] <<endl;
		exit(1);
	}
	fread(ImagedataGy, sizeof(float), 256*256, file);
	fclose(file);

	if (!(file=fopen(argv[3],"rb"))) {  // read previous edge map
		cout << "Cannot open file: " << argv[3] <<endl;
		exit(1);
	}
	fread(ImagedataEG, sizeof(unsigned char), 256*256, file);
	fclose(file);

	// fill ImagedataE with original in the middle
	for (int i=0;i<=255;i++){
		for (int j=0;j<=255;j++){
			ImagedataE[i+5][j+5]=ImagedataEG[i][j];
		}
	}
	// edgeExtension : vertical long strips (excluding corners)
	for (int i=0;i<=265;i++){
		for (int j=0;j<=265;j++){
			if((i>=5)&&(i<=260)&&(j<=4)){ // left
				ImagedataE[i][j]=ImagedataE[i][10-(j+1)];
			}
			if((i>=5)&&(i<=260)&&(j>=261)){ // right
				ImagedataE[i][j]=ImagedataE[i][j-2*(j-260)-1];
			}
		}
	}
	// edgeExtension : horizontal long strips (including 4 corners)
	for (int i=0;i<=265;i++){
		for (int j=0;j<=265;j++){
			if(i<=4){ // left
				ImagedataE[i][j]=ImagedataE[10-(i+1)][j];
			}
			if(i>=261){ // right
				ImagedataE[i][j]=ImagedataE[i-2*(i-260)-1][j];
			}
		}
	}

	// calculation ================================================
	// save the dominate orientation map
	for (int i=0;i<255;i++){
		for (int j=0;j<=255;j++){
			if(ImagedataGx[i][j]>=ImagedataGy[i][j]){
				ImagedataOri[i][j]=0;
			}
			else{
				ImagedataOri[i][j]=1;
			}
		}
	}

	for (int i=0;i<255;i++){
		for (int j=0;j<=255;j++){
			if(ImagedataE[i+5][j+5]==0){ //check if its edge point.
				if(ImagedataOri[i][j]==0){ //if x is dominant, check y direction
					if((ImagedataE[i+5][j+5]<ImagedataE[i+4][j+5]) && (ImagedataE[i+5][j+5]<ImagedataE[i+6][j+5])){
						ImagedataE[i+5][j+5]=0;
					}
					else{
						ImagedataE[i+5][j+5]=255;
					}
				}
				else if (ImagedataOri[i][j]==1){ // if y is dominant, check x direction
					if((ImagedataE[i+5][j+5]<ImagedataE[i+5][j+4]) && (ImagedataE[i+5][j+5]<ImagedataE[i+5][j+6])){
						ImagedataE[i+5][j+5]=0;
					}
					else{
						ImagedataE[i+5][j+5]=255;
					}
				}
			}
		}
	}

	//resize image data
	for (int i=0;i<=255;i++){
		for (int j=0;j<=255;j++){
			ImagedataF[i][j]=ImagedataE[i+5][j+5];
		}
	}

	// Write image data (filename specified by second argument) from image data matrix
	string savename=argv[3];

	// Save into different file names based on argument input
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen((savename+"_thin"+".raw").c_str(),"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImagedataF, sizeof(unsigned char), 256*256, file);
	fclose(file);
	return 0;
}
