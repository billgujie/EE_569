/*
 * edgeDoG.cpp
 * first order edge detection
 *  Created on: Sep 23, 2012
 *      Author: gu
 *
 *      difference of Gaussian for edge detection
 *      arguments: program_name input_image.raw output_image.raw
 *
 *      change 	line:166 for threshold selection
 *      		line:177 for output name
 */

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <fstream>

using namespace std;
void filter(const unsigned char data[][266], int row, int col, int *output);//low pass filter
void sobel(const unsigned char data[][266], int row, int col, float *output, char mode); //sobel filter
int main(int argc, char *argv[]) //command line arguments

{
	// Define file pointer and variables
	FILE *file;


	// Allocate image data array
	unsigned char Imagedata[256][256];//size of unsigned char is 0-255
	unsigned char ImagedataE[266][266];// extended image data, for edge extension,
	float ImagedataDx[256][256]; //x-direction gradient map raw value
	unsigned char ImagedataDx_int[256][256]; //x-direction gradient map remapped value
	float ImagedataDy[256][256]; //y-direction gradient map raw value
	unsigned char ImagedataDy_int[256][256]; // y-direction remapped value
	float ImagedataMag[256][256]; // gradient magnitude raw value
	unsigned char ImagedataMag_int[256][256]; // gradient magnitude remapped value
	unsigned char ImagedataEDGE[256][256]; // binary edge map

	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), 256*256, file);
	fclose(file);

	// fill ImagedataE with original in the middle
	for (int i=0;i<=255;i++){
		for (int j=0;j<=255;j++){
			ImagedataE[i+5][j+5]=Imagedata[i][j];
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


	// calculation
	//		gaussian filtering
	int pointvalue[1];
	for (int i=5;i<=260;i++){
		for (int j=5;j<=260;j++){
			filter(ImagedataE, i, j, pointvalue);
			ImagedataE[i][j]=pointvalue[0];
		}
	}

	// generating DfDx gradient map
	float minmax[]={0,0}; // loc0 for min, and loc1 for max
	float pointvalue1[1];
	for (int i=5;i<=260;i++){
		for (int j=5;j<=260;j++){
			char mode='x';
			sobel(ImagedataE, i, j, pointvalue1, mode);
			ImagedataDx[i-5][j-5]=pointvalue1[0];
			if (pointvalue1[0]<minmax[0]){
				minmax[0]=pointvalue1[0];
			}
			if (pointvalue1[0]>minmax[1]){
				minmax[1]=pointvalue1[0];
			}
		}
	}
	// normalizing to 0-255
	for (int i=0;i<=255;i++){
		for (int j=0;j<=255;j++){
			ImagedataDx[i][j]=ImagedataDx[i][j]+abs(minmax[0]);
			ImagedataDx_int[i][j]=static_cast<int>(ImagedataDx[i][j]/(abs(minmax[0])+minmax[1])*255);
		}
	}


	// generating DfDy gradient map
	for (int i=5;i<=260;i++){
		for (int j=5;j<=260;j++){
			char mode='y';
			sobel(ImagedataE, i, j, pointvalue1, mode);
			ImagedataDy[i-5][j-5]=pointvalue1[0];
			if (pointvalue1[0]<minmax[0]){
				minmax[0]=pointvalue1[0];
			}
			if (pointvalue1[0]>minmax[1]){
				minmax[1]=pointvalue1[0];
			}
		}
	}
	// normalizing to 0-255
	for (int i=0;i<=255;i++){
		for (int j=0;j<=255;j++){
			ImagedataDy[i][j]=ImagedataDy[i][j]+abs(minmax[0]);
			ImagedataDy_int[i][j]=static_cast<int>(ImagedataDy[i][j]/(abs(minmax[0])+minmax[1])*255);
		}
	}


	// gradient magnitude map:
	float magMax=0;
	float temp;
	for (int i=0;i<=255;i++){
		for (int j=0;j<=255;j++){
			temp=sqrt(ImagedataDx[i][j]*ImagedataDx[i][j]+ImagedataDy[i][j]*ImagedataDy[i][j]);
			ImagedataMag[i][j]=temp;
			if (temp>magMax){
				magMax=temp; // keep record of the max of temp for normalization
			}
		}
	}
	// normalizing to 0-255
	for (int i=0;i<=255;i++){
		for (int j=0;j<=255;j++){
			ImagedataMag_int[i][j]=static_cast<int>(ImagedataMag[i][j]/magMax*255);
		}
	}

	// thresholding
	//	girl.raw:   95%  =  190
	//				90%  =  175
	//				80%  =  166
	//
	//	elaine.raw: 95%  =  161
	//				90%  =  152
	//				80%  =  145
	for (int i=0;i<=255;i++){
		for (int j=0;j<=255;j++){
			if (ImagedataMag_int[i][j]>=161)//threshold
			ImagedataEDGE[i][j]=0;
			else ImagedataEDGE[i][j]=255;
		}
	}

	// Write image data (filename specified by second argument) from image data matrix
	string savename=argv[2];

	// Save into different file names based on argument input
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen((savename+"_Tre95"+".raw").c_str(),"wb"))) { // output name
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImagedataEDGE, sizeof(unsigned char), 256*256, file);
	fclose(file);
	return 0;
}

void sobel(const unsigned char data[][266], int row, int col, float *output, char mode){
	float dFdx,dFdy;
	float A0=data[row-1][col-1];
	float A1=data[row-1][col];
	float A2=data[row-1][col+1];
	float A3=data[row][col+1];
	float A4=data[row+1][col+1];
	float A5=data[row+1][col];
	float A6=data[row+1][col-1];
	float A7=data[row][col-1];
	if (mode=='x'){
		dFdx=(A2+2*A3+A4-A0-2*A7-A6)/4;
		output[0]=dFdx;
	}
	else if (mode=='y'){
		dFdy=(A0+2*A1+A2-A6-2*A5-A4)/4;
		output[0]=dFdy;
	}

}

void filter(const unsigned char data[][266],  int row, int col, int *output){//low pass filter
	double value;
	double sum=0;

	// 3x3 gaussian
	int weight[]={1,2,1,2,4,2,1,2,1};
	int count=0;
	for (int i=row-1;i<=row+1;i++){
		for (int j=col-1;j<=col+1;j++){
			sum=sum+data[i][j]*weight[count];
			count++;
		}
	}
	value=sum/16;

	output[0]=static_cast<int>(value);
}
