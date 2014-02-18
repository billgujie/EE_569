
/*
* photoFilter.cpp
*
*  Created on: Oct 7, 2012
*      Author: gu
*/

// Using the probe image and result image, get the filter response and color mapping for 256 levels in RGB
// input 2 images, output 3 txt files: mapping for RBG channels
// adapted from readraw.cpp file provided for this homework

#include "StdAfx.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

//check if the color level is out of range
//clipping method is used here

int rangeCheck (int value) {
	if (value<=0){
		return 0;
	}
	else if (value>=255){
		return 255;
	}
	else
		return value;
}

//by comparing border and origianl image against criteria
//return the necessary RGB values
int* borderCheck (int origin[], int border[]){ 
	int arr[3];
	int* result;

	if (border[0]==255&&border[1]==255&&border[2]==255){
		result=origin;
		return result;
	}
	else{
		arr[0]=border[0];
		arr[1]=border[1];
		arr[2]=border[2];
		result=arr;
		return result;
	}
}


	// argv = probe.raw[1] response.raw[2] input.raw[3] grain.raw[4] border.raw[5]
int main(int argc, char *argv[]) //command line arguments

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel=3;
	int SizeW=256;
	int SizeH=256;
	// Allocate image data array
	unsigned char *prob = new unsigned char [SizeW*SizeH*BytesPerPixel]; //prob image
	unsigned char *result = new unsigned char [SizeW*SizeH*BytesPerPixel];	//result image
	unsigned char *image = new unsigned char [SizeW*SizeH*BytesPerPixel]; //input image
	unsigned char *grain = new unsigned char [SizeW*SizeH*BytesPerPixel];
	unsigned char *border = new unsigned char [SizeW*SizeH*BytesPerPixel];
	int Prob_RAW[64];	// prob image values
	int Result[256][3];// color map Results, filling with 64 values initially, leave others as blank

	//read prob file
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(prob, sizeof(unsigned char), SizeW*SizeH*BytesPerPixel, file);
	fclose(file);

	//read prob result file
	if (!(file=fopen(argv[2],"rb"))) {
		cout << "Cannot open file: " << argv[2] <<endl;
		exit(1);
	}
	fread(result, sizeof(unsigned char), SizeW*SizeH*BytesPerPixel, file);
	fclose(file);

	//read input image
	if (!(file=fopen(argv[3],"rb"))) {
		cout << "Cannot open file: " << argv[3] <<endl;
		exit(1);
	}
	fread(image, sizeof(unsigned char), SizeW*SizeH*BytesPerPixel, file);
	fclose(file);

	// probe reading new probe image and response images are 16x16 blocks in the centre of image
	// Write txt files
	if (!(file=fopen("Result_R.txt","wb"))) {
		cout << "Cannot open file" << endl;
		exit(1);
	}
	int count=0;
	int prob_temp;
	int R,G,B;
	for (int i=0;i<8;i++){
		for (int j=0;j<8;j++){
			prob_temp=prob[(72*256+16*i*SizeW)*3+(16*(j+4)+8)*3];
			R=result[(72*256+16*i*SizeW)*3+(16*(j+4)+8)*3];
			G=result[(72*256+16*i*SizeW)*3+(16*(j+4)+8)*3+1];
			B=result[(72*256+16*i*SizeW)*3+(16*(j+4)+8)*3+2];
			Prob_RAW[count]=prob_temp;
			Result[prob_temp][0]=R;
			Result[prob_temp][1]=G;
			Result[prob_temp][2]=B;
			count++;
			fprintf (file, "%i\t%i\t%i\t%i\n",prob_temp,R,G,B);
		}
	}
	fclose(file);
	delete [] prob; //release memory
	delete [] result;

	//interpolate data from Prob result into linear mapping (havent consider boundary conditions!!)
	int y;
	for (int CH=0;CH<3;CH++){
		for (int i=0;i<63;i++){ //only perform uptp 63 probed values [0-62]
			int y0=Result[Prob_RAW[i]][CH]; // variables for constructing interpolation equations
			int y1=Result[Prob_RAW[i+1]][CH];
			int x0=Prob_RAW[i];
			int x1=Prob_RAW[i+1];
			for (int tot=1;tot<(Prob_RAW[i+1]-Prob_RAW[i]);tot++){ // dynamically change the recurrsion time
				y=(y1-y0)/(x1-x0)*(Prob_RAW[i]+tot-x0)+y0;
				Result[Prob_RAW[i]+tot][CH]=y;
			}
		}
	}

	//Fix the boundary condition
	// Front
	Result[0][0]=Result[1][0];
	Result[0][1]=Result[1][1];
	Result[0][2]=Result[1][2];
	// Back
	Result[254][0]=Result[253][0];
	Result[254][1]=Result[253][1];
	Result[254][2]=Result[253][2];
	Result[255][0]=Result[253][0];
	Result[255][1]=Result[253][1];
	Result[255][2]=Result[253][2];

	// save txt files for plotting
	if (!(file=fopen("Result_I.txt","wb"))) {
		cout << "Cannot open file" << endl;
		exit(1);
	}
	for (int i=0;i<SizeW;i++){
		int R=Result[i][0];
		int G=Result[i][1];
		int B=Result[i][2];
		fprintf (file, "%i\t%i\t%i\t%i\n",i,R,G,B);
	}
	fclose(file);

	//	color mapping
	for (int i=0;i<SizeW*SizeH*BytesPerPixel;i++){
		image[i]=Result[image[i]][0];i++;
		image[i]=Result[image[i]][1];i++;
		image[i]=Result[image[i]][2];
	}
	//	Add grain
	//		read grain.raw image
	if (!(file=fopen(argv[4],"rb"))) {
		cout << "Cannot open file: " << argv[4] <<endl;
		exit(1);
	}
	fread(grain, sizeof(unsigned char), SizeW*SizeH*BytesPerPixel, file);
	fclose(file);
	//		add grain
	for (int i=0;i<SizeW*SizeH*BytesPerPixel;i++){

		// original algorithm provided by homework manual
		//image[i]=rangecheck(image[i]-0.4*grain[i]+30);i++;
		//image[i]=rangecheck(image[i]-0.4*grain[i]+30);i++;
		//image[i]=rangecheck(image[i]-0.4*grain[i]+30);

		// algorithm that consider out of range
		image[i]=rangeCheck(image[i]-(255-grain[i]));i++;
		image[i]=rangeCheck(image[i]-(255-grain[i]));i++;
		image[i]=rangeCheck(image[i]-(255-grain[i]));
	}
	delete [] grain;


	//	Add Border
	//		read border raw image
	if (!(file=fopen(argv[5],"rb"))) {
		cout << "Cannot open file: " << argv[4] <<endl;
		exit(1);
	}
	fread(border, sizeof(unsigned char), SizeW*SizeH*BytesPerPixel, file);
	fclose(file);
	//		add border
	for (int i=0;i<SizeW*SizeH*BytesPerPixel;i++){
		int origin[] = {image[i],image[i+1],image[i+2]};
		int bor[] = {border[i],border[i+1],border[i+2]};
		int* back=borderCheck(origin,bor);
		image[i]=back[0];i++;
		image[i]=back[1];i++;
		image[i]=back[2];
	}
	delete [] border;


	// save to ".raw"
	char output[80]="OUTPUT";
	strcat(output,argv[3]);
	if (!(file=fopen(output,"wb"))){
		cout<<"Cannot open file"<<endl;
		exit(1);
	}
	fwrite(image,sizeof(unsigned char),SizeW*SizeH*BytesPerPixel,file);
	fclose(file);
	delete []image;
	return 0;
}
