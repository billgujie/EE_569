/*
 * edgeLoG.cpp
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
void filterG(const unsigned char data[][266], int row, int col, int *output);//gaussian filter
void filterLP(const unsigned char data[][266], int row, int col, int *output);//laplacian filter
void zeroCross(const int data[][266], int row, int col, int *output); //zero crossing detection
int main(int argc, char *argv[]) //command line arguments

{
	// Define file pointer and variables
	FILE *file;


	// Allocate image data array
	unsigned char Imagedata[256][256];//size of unsigned char is 0-255
	unsigned char ImagedataE[266][266];// extended image data, for edge extension,
	int ImagedataTri[266][266]; // trinary map array
	unsigned char ImagedataEDGE[256][256];

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

	// calculation ===========================================================
	//		gaussian filtering
	int pointvalue[1];
	for (int i=4;i<=261;i++){
		for (int j=4;j<=261;j++){
			filterG(ImagedataE, i, j, pointvalue);
			ImagedataE[i][j]=pointvalue[0];
		}
	}

	//		laplacian filtering and trinary maping

	for (int i=4;i<=261;i++){
		for (int j=4;j<=261;j++){
			filterLP(ImagedataE, i, j, pointvalue);
			ImagedataTri[i][j]=pointvalue[0];
		}
	}

	// zerocrossing ==========================================================
	for (int i=5;i<=260;i++){
		for (int j=5;j<=260;j++){
			zeroCross(ImagedataTri, i, j, pointvalue);
			ImagedataEDGE[i-5][j-5]=pointvalue[0];
		}
	}

	// Write image data (filename specified by second argument) from image data matrix
	string savename=argv[2];

	// Save into different file names based on argument input
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen((savename+"_LoG"+".raw").c_str(),"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImagedataEDGE, sizeof(unsigned char), 256*256, file);
	fclose(file);
	return 0;
}

void zeroCross(const int data[][266], int row, int col, int *output){
	int A0=data[row-1][col-1];
	int A1=data[row-1][col];
	int A2=data[row-1][col+1];
	int A3=data[row][col+1];
	int A4=data[row+1][col+1];
	int A5=data[row+1][col];
	int A6=data[row+1][col-1];
	int A7=data[row][col-1];

	if (data[row][col]!=0)
		output[0]=output[0]+0; // 255=white  0=edge
	else if (data[row][col]==0){
		if(A0!=0 && A0+A4==0){
			output[0]=0;
		}
		else if(A1!=0 && A1+A5==0){
			output[0]=0;
		}
		else if(A2!=0 && A2+A6==0){
			output[0]=0;
		}
		else if(A3!=0 && A3+A7==0){
			output[0]=0;
		}
		else{
			output[0]=255;
		}
	}
}

void filterLP(const unsigned char data[][266],  int row, int col, int *output){//low pass filter
	double value;
	int pt=data[row][col];
	int up=data[row-1][col];
	int down=data[row+1][col];
	int left=data[row][col-1];
	int right=data[row][col+1];

	// 5 point laplacian
	value=4*pt-up-down-left-right;

	// thresholding to get trinary map
	if (value<=-8){
		value=-1;
	}
	else if (value>=8){
		value=1;
	}
	else{
		value=0;
	}
	output[0]=value;
}


void filterG(const unsigned char data[][266],  int row, int col, int *output){//gaussian filter
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

