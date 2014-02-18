/*
 * edgeVariance.cpp
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

double mean(const unsigned char data[][266],  int row, int col);
double variance(int value, double mean);
void filterG(const unsigned char data[][266], int row, int col, int *output);//gaussian filter
void filterLP(const unsigned char data[][266], int row, int col, int *output);//laplacian filter
int main(int argc, char *argv[]) //command line arguments

{
	// Define file pointer and variables
	FILE *file;


	// Allocate image data array
	unsigned char Imagedata[256][256];//size of unsigned char is 0-255
	unsigned char ImagedataE[266][266];// extended image data, for edge extension,
	double ImagedataV[266][266];
	unsigned char ImagedataEDGE[256][256];
	double meanvalue;
	double varmax=0;
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

	// mean and variance
	double temp;
	for (int i=5;i<=260;i++){
		for (int j=5;j<=260;j++){
			meanvalue=mean(ImagedataE,i,j);
			temp=variance(ImagedataE[i][j], meanvalue);
			ImagedataV[i][j]=temp;
			if (temp>varmax){
				varmax=temp;
			}
		}
	}

	// normalizing to 0-255
	for (int i=5;i<=260;i++){
		for (int j=5;j<=260;j++){
			ImagedataEDGE[i-5][j-5]=static_cast<int>(ImagedataV[i][j]/(varmax)*255);
		}
	}


	// Write image data (filename specified by second argument) from image data matrix
	string savename=argv[2];

	// Save into different file names based on argument input
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen((savename+"_Vari"+".raw").c_str(),"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImagedataEDGE, sizeof(unsigned char), 256*256, file);
	fclose(file);
	return 0;
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

double mean(const unsigned char data[][266],  int row, int col){
	//mean of window 3x3, unweighted
	double value;
	double sum=0;
	for (int i=row-1;i<=row+1;i++){
		for (int j=col-1;j<=col+1;j++){
			sum=sum+data[i][j];
		}
	}
	value=sum/9;
	return value;
}
double variance(int value, double mean){
	double temp=value;
	double result;
	result=temp-mean;
	return (result*result);
}
