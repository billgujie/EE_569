#include "StdAfx.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;

//int htCompare(int a, double b){ //return 1 if a > than threshold b
//	int b_temp=floor(b*255 + 0.5);// change b into integer
//	int range;
//	if (a<=63)//0-63
//		range=1;
//	else if(a<=127)//64-127
//		range=2;
//	else//128-191
//		range=3;
//
//	if(a>b_temp)
//		return range+1;
//	else
//		return range-1;
//}
//
//int remap(unsigned char value){// map [0,1] to [255 0]
//	if (value==4)
//		return 255;
//	else if(value==3)
//		return 192;
//	else if(value==2)
//		return 127;
//	else
//		return 0;
//}



int remap(int value){// map [0,1] to [255 0]
	if (value==1)
		return 255;
	else
		return 0;
}

float ErrFS(float a, int i, int j,int d){
	if (d==1){
		float FS1[3][3]={  //Floyd-Steinberg error diffusion
			{0,0,0},
			{0,0,0.4},
			{0.2,0.3,0.1}
		};
		return (a*FS1[i][j]);
	}
	if (d==-1){
		float FS2[3][3]={  //Floyd-Steinberg error diffusion
			{0,0,0},
			{0.4,0,0},
			{0.1,0.3,0.2}
		};
		return (a*FS2[i][j]);
	}
}


float ErrJJN(float a, int i, int j,int d){
	if (d==1){
		float JJN1[5][5]={ //Jarvis, Judice, and Ninke Error Diffusion
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,0,0.1458,0.1042},
			{0.0625,0.1042,0.1458,0.1042,0.0625},
			{0.0208,0.0625,0.1042,0.0625,0.0208}
		};
		return (a*JJN1[i][j]);
	}
	if (d==-1){
		float JJN2[5][5]={ //Jarvis, Judice, and Ninke Error Diffusion
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0.1458,0.1042,0,0,0},
			{0.0625,0.1042,0.1458,0.1042,0.0625},
			{0.0208,0.0625,0.1042,0.0625,0.0208}
		};
		return (a*JJN2[i][j]);

	}
}





float ErrS(float a, int i, int j,int d){
	if (d==1){
		float S1[5][5]={//Stucki Error Diffusion
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,0,0.1905,0.0952},
			{0.0476,0.0952,0.1905,0.0952,0.0476},
			{0.0238,0.0476,0.0952,0.0476,0.0238}
		};
		return (a*S1[i][j]);
	}
	if (d==-1){
		float S2[5][5]={//Stucki Error Diffusion
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0.1905,0.0952,0,0,0},
			{0.0476,0.0952,0.1905,0.0952,0.0476},
			{0.0238,0.0476,0.0952,0.0476,0.0238}
		};
		return (a*S2[i][j]);
	}
}

float change(unsigned char a){
	float temp;
	temp=a;
	return temp/255;
}
int main(int argc, char *argv[])
{
	FILE *file;


	/*
	METHOD:
	3=Floyd-Steinberg error diffusion
	5=JJN + Stucki Error Diffusion
	*/
	int method=5; 
	const int SizeW=512;
	const int SizeH=512;
	unsigned char *Image = new unsigned char [SizeW*SizeH];
	unsigned char ImageE[SizeH][SizeW]; // Image
	unsigned char ImageF[SizeH][SizeW]; // Image
	//float ImageD[SizeH+4][SizeW+4];// convert to normalized [0,1], Double type

	float **ImageD = new float* [SizeH+4];
	for (int i = 0; i < SizeH+4; i++) {
		ImageD [i] = new float [SizeW+4];
	}

	float **ImageD2 = new float* [SizeH+4];
	for (int i = 0; i < SizeH+4; i++) {
		ImageD2 [i] = new float [SizeW+4];
	}

	
	if (!(file=fopen("man.raw","rb"))) { // change file name
		cout << "Cannot open file"<<endl;
		exit(1);
	}
	fread(Image, sizeof(unsigned char), SizeW*SizeH, file);
	fclose(file);

	int count=0; // save image data into ImageE
	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageE[i][j]=Image[count];
			count++;
		}
	}
	delete [] Image;

	//Normalized [0-1] and save into ImageD and ImageD2
	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageD[i+2][j+2]=change(ImageE[i][j]);
		}
	}
	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageD2[i+2][j+2]=change(ImageE[i][j]);
		}
	}


	////copy of ImageD
	//for (int i=0;i<SizeH;i++){
	//	for(int j=0;j<SizeW;j++){
	//		ImageD2[i+2][j+2]=((float)ImageE[i][j])/255;
	//	}
	//}



	//ERROR diffusion

	int direction=1;//1: left to right  -1:right to left
	float thresh=0.5;// threshold for error diffusion
	float ERR; // error value
	for (int i=0;i<SizeH;i++){//for all pixels
		if (direction==1){
			for(int j=0;j<SizeW;j++){
				if(ImageD[i+2][j+2]>=thresh){
					ERR=ImageD[i+2][j+2]-1;
					ImageD[i+2][j+2]=1;
					ImageD2[i+2][j+2]=1;
				}
				else{
					ERR=ImageD[i+2][j+2];
					ImageD[i+2][j+2]=0;
					ImageD2[i+2][j+2]=0;
				}
				int i_count=0;
				int j_count=0;
				for (int Ei=-((method-1)/2);Ei<((method-1)/2+1);Ei++){ // marker for actual image array
					for (int Ej=-((method-1)/2);Ej<((method-1)/2+1);Ej++){
						if (method==3)
						ImageD[i+2+Ei][j+2+Ej]=ImageD[i+2+Ei][j+2+Ej]+ErrFS(ERR,i_count,j_count,direction);
						if (method==5){
						ImageD[i+2+Ei][j+2+Ej]=ImageD[i+2+Ei][j+2+Ej]+ErrJJN(ERR,i_count,j_count,direction);
						ImageD2[i+2+Ei][j+2+Ej]=ImageD2[i+2+Ei][j+2+Ej]+ErrS(ERR,i_count,j_count,direction);
						}
						j_count++;
					}
					j_count=0;
					i_count++;
				}
			}
			//direction=direction*-1;
		}
		else if(direction==-1){ // inverse scan
			for(int j=SizeW;j>0;j--){
				if(ImageD[i+1][j+1]>=thresh){
					ERR=ImageD[i+1][j+1]-1;
					ImageD[i+1][j+1]=1;
					//ImageD2[i][j]=1;
				}
				else{
					ERR=ImageD[i+1][j+1];
					ImageD[i+1][j+1]=0;
					//ImageD2[i][j]=0;
				}
				int i_count=0;
				int j_count=0;
				for (int Ei=-1;Ei<2;Ei++){ // marker for actual image array
					for (int Ej=-1;Ej<2;Ej++){
						ImageD[i+1+Ei][j+1+Ej]=ImageD[i+1+Ei][j+1+Ej]+ErrFS(ERR,i_count,j_count,direction);
						j_count++;
					}
					j_count=0;
					i_count++;
				}
			}
			direction=direction*-1;
		}
	}


	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageF[i][j]=remap(ImageD[i+2][j+2]);
		}
	}
	if (!(file=fopen("man_JJN_progscan.raw","wb"))){
		cout<<"Cannot open file"<<endl;
		exit(1);
	}
	fwrite(ImageF,sizeof(unsigned char),(SizeW*SizeH),file);
	fclose(file);

	if (method==5){
		for (int i=0;i<SizeH;i++){
			for(int j=0;j<SizeW;j++){
				ImageF[i][j]=remap(ImageD2[i+2][j+2]);
			}
		}
		if (!(file=fopen("man_Stucki_progscan.raw","wb"))){
			cout<<"Cannot open file"<<endl;
			exit(1);
		}
		fwrite(ImageF,sizeof(unsigned char),(SizeW*SizeH),file);
		fclose(file);
	}

}