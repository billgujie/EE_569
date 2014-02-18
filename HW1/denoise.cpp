/*
 * contrastHIST.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: gu
 */

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <algorithm>

using namespace std;

void filter(const unsigned char data[][522], int row, int col, int *output, string mode);//low pass filter

int order(const unsigned char data[],int size);//reorder and return median value

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
	unsigned char Imagedata[512][512];//size of unsigned char is 0-255
	unsigned char ImagedataE[522][522];// extended image data, for edge extension,
	unsigned char ImagedataF[512][512];
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), 512*512, file);
	fclose(file);

	// fill ImagedataE with original in the middle
	for (int i=0;i<=511;i++){
		for (int j=0;j<=511;j++){
			ImagedataE[i+5][j+5]=Imagedata[i][j];
		}
	}
	// edgeExtension : vertical long strips (excluding corners)
	for (int i=0;i<=521;i++){
		for (int j=0;j<=521;j++){
			if((i>=5)&&(i<=516)&&(j<=4)){ // left
				ImagedataE[i][j]=ImagedataE[i][10-(j+1)];
			}
			if((i>=5)&&(i<=516)&&(j>=517)){ // right
				ImagedataE[i][j]=ImagedataE[i][j-2*(j-516)-1];
			}
		}
	}
	// edgeExtension : horizontal long strips (including 4 corners)
	for (int i=0;i<=521;i++){
		for (int j=0;j<=521;j++){
			if(i<=4){ // left
				ImagedataE[i][j]=ImagedataE[10-(i+1)][j];
			}
			if(i>=517){ // right
				ImagedataE[i][j]=ImagedataE[i-2*(i-516)-1][j];
			}
		}
	}


	/*
	 * Method summary:
	 *
	 * null: 	do nothing
	 * L3:		average 3x3
	 * L5:		average 5x5
	 * L3W:		weighted average 3x3
	 * L5W:		weighted average 5x5
	 * M3:		median 3x3
	 * M5:		median 5x5
	 */

	string method1="L3";
	string method2="M5";
	string method3="null";

	//filtering
	int pointvalue[1];
	for (int i=5;i<=516;i++){ // first filtering
		for (int j=5;j<=516;j++){
			filter(ImagedataE, i, j, pointvalue, method1);
			ImagedataE[i][j]=pointvalue[0];
		}
	}
	for (int i=5;i<=516;i++){ //second filtering
		for (int j=5;j<=516;j++){
			filter(ImagedataE, i, j, pointvalue, method2);
			ImagedataE[i][j]=pointvalue[0];
		}
	}
	for (int i=5;i<=516;i++){ //third filtering
		for (int j=5;j<=516;j++){
			filter(ImagedataE, i, j, pointvalue, method3);
			ImagedataE[i][j]=pointvalue[0];
		}
	}


	//resize image data
		for (int i=0;i<=511;i++){
			for (int j=0;j<=511;j++){
				ImagedataF[i][j]=ImagedataE[i+5][j+5];
			}
		}


	// Write image data (filename specified by second argument) from image data matrix
	string savename=argv[2];

	// Save into different file names based on argument input
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen((savename+"_"+method1+"_"+method2+"_"+method3+".raw").c_str(),"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImagedataF, sizeof(unsigned char), 522*522, file);
	fclose(file);
	return 0;
}

void filter(const unsigned char data[][522],  int row, int col, int *output, string mode){//low pass filter
	double value;
	double sum=0;
	if (mode=="null"){ // do nothing
		value=data[row][col];
	}
	if (mode=="L3"){ // unweighted 3x3
		int i, j;
		for (i=row-1;i<=row+1;i++){
			for (j=col-1;j<=col+1;j++){
				sum=sum+data[i][j];
			}
		}
		value=sum/9;
	}
	else if (mode=="L5"){ // unweighted 5x5
		for (int i=row-2;i<=row+2;i++){
			for (int j=col-2;j<=col+2;j++){
				sum=sum+data[i][j];
			}
		}
		value=sum/25;
	}
	else if (mode=="L3W"){ // weighted 3x3
		int weight[]={1,2,1,2,4,2,1,2,1};
		int count=0;
		for (int i=row-1;i<=row+1;i++){
			for (int j=col-1;j<=col+1;j++){
				sum=sum+data[i][j]*weight[count];
				count++;
			}
		}
		value=sum/16;
	}
	else if (mode=="L5W"){ // weighted 5x5
		int weight[]={1,1,2,1,1,1,2,4,2,1,2,4,8,4,2,1,2,4,2,1,1,1,2,1,1};
		int count=0;
		for (int i=row-2;i<=row+2;i++){
			for (int j=col-2;j<=col+2;j++){
				sum=sum+data[i][j]*weight[count];
				count++;
			}
		}
		value=sum/52;
	}
	else if (mode=="M3"){ // median 3x3
		unsigned char array[9];
		int count=0;
		for (int i=row-1;i<=row+1;i++){
			for (int j=col-1;j<=col+1;j++){
				array[count]=sum+data[i][j];
				count++;
			}
		}
		value=order(array,9);
	}
	else if (mode=="M5"){ // median 5x5
		unsigned char array[25];
		int count=0;
		for (int i=row-2;i<=row+2;i++){
			for (int j=col-2;j<=col+2;j++){
				array[count]=sum+data[i][j];
				count++;
			}
		}
		value=order(array,25);
	}

	output[0]=static_cast<int>(value);
}

int order(const unsigned char data[],int size){ // sort array using std:sort in STL
	int elements = size / sizeof(data[0]);
	int array[25];
	for (int i=0;i<=size;i++){
		array[i]=data[i];
	}
	std::sort(array, array + elements);

	if (size==9){
		return array[4];
	}
	else if (size==25){
		return array[12];
	}
	else
		return 0;
}
