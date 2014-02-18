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

int htCompare(int a, double b){ //return 1 if a > than threshold b
	int b_temp=floor(b*255 + 0.5);// change b into integer

	if(a>b_temp)
		return 1;
	else
		return 0;
}


int remap(int value){// map [0,1] to [255 0]
	if (value==1)
		return 255;
	else
		return 0;
}


int main(int argc, char *argv[])
{
	FILE *file;


	/*
	METHOD:
	2=bayer 2x2
	4=bayer 4x4 + A matrix 4x4
	8=bayer 8x8 + A matrix 8x8
	3=Floyd-Steinberg error diffusion
	5=JJN + Stucki Error Diffusion
	*/
	int method=3; 
	const int SizeW=512;
	const int SizeH=512;
	unsigned char *Image = new unsigned char [SizeW*SizeH];
	unsigned char ImageE[SizeH][SizeW]; // Image
	unsigned char ImageE2[SizeH][SizeW]; // Image
	unsigned char ImageF[SizeH][SizeW]; // Image
	float ImageD[SizeH+4][SizeW+4];// convert to normalized [0,1], Double type
	float ImageD2[SizeH+4][SizeW+4];

	double B22[2][2]={
		{0.125,0.625},
		{0.875,0.375}
	};

	double B44[4][4]={
		{0.0313,0.5313,0.1563,0.6563},
		{0.7813,0.2813,0.9063,0.4063},
		{0.2188,0.7188,0.0938,0.5938},
		{0.9688,0.4688,0.8438,0.3438}
	};

	double B88[8][8]={
		{0.0078,0.5078,0.1328,0.6328,0.0391,0.5391,0.1641,0.6641},
		{0.7578,0.2578,0.8828,0.3828,0.7891,0.2891,0.9141,0.4141},
		{0.1953,0.6953,0.0703,0.5703,0.2266,0.7266,0.1016,0.6016},
		{0.9453,0.4453,0.8203,0.3203,0.9766,0.4766,0.8516,0.3516},
		{0.0547,0.5547,0.1797,0.6797,0.0234,0.5234,0.1484,0.6484},
		{0.8047,0.3047,0.9297,0.4297,0.7734,0.2734,0.8984,0.3984},
		{0.2422,0.7422,0.1172,0.6172,0.2109,0.7109,0.0859,0.5859},
		{0.9922,0.4922,0.8672,0.3672,0.9609,0.4609,0.8359,0.3359}
	};

	double A88[8][8]={
		{0.9453,0.6641,0.6797,0.6953,0.7109,0.7266,0.7422,0.9609},
		{0.6484,0.5234,0.3828,0.3984,0.4141,0.4297,0.5391,0.7578},
		{0.6328,0.3672,0.2266,0.1641,0.1797,0.2422,0.4453,0.7734},
		{0.6172,0.3516,0.1484,0.0547,0.0078,0.0703,0.4609,0.7891},
		{0.6016,0.3359,0.1328,0.0391,0.0234,0.0859,0.4766,0.8047},
		{0.5859,0.3203,0.2109,0.1172,0.1016,0.1953,0.4922,0.8203},
		{0.5703,0.5078,0.3047,0.2891,0.2734,0.2578,0.5547,0.8359},
		{0.9922,0.9297,0.9141,0.8984,0.8828,0.8672,0.8516,0.9766}
	};

	double A44[4][4]={
		{0.9063,0.6563,0.7188,0.9688},
		{0.5938,0.2188,0.0313,0.2813},
		{0.5313,0.1563,0.0938,0.3438},
		{0.8438,0.4688,0.4063,0.7813}
	};


	double FS[3][3]={  //Floyd-Steinberg error diffusion
		{0,0,0},
		{0,0,0.4},
		{0.2,0.3,0.1}
	};

	double JJN[5][5]={ //Jarvis, Judice, and Ninke Error Diffusion
		{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,0.1458,0.1042},
		{0.0625,0.1042,0.1458,0.1042,0.0625},
		{0.0208,0.0625,0.1042,0.0625,0.0208}
	};

	double S[5][5]={//Stucki Error Diffusion
		{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,0.1905,0.0952},
		{0.0476,0.0952,0.1905,0.0952,0.0476},
		{0.0238,0.0476,0.0952,0.0476,0.0238}
	};




	// read shrinked gear file
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

	count=0;// make a copy of ImageE into ImageE2
	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageE2[i][j]=Image[count];
			count++;
		}
	}
	delete [] Image;

	//Normalized [0-1] and save into ImageD
	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageD[i+2][j+2]=((double)ImageE[i][j])/255;
		}
	}
	//copy of ImageD
	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageD2[i+2][j+2]=((double)ImageE[i][j])/255;
		}
	}





	if (method==2||method==4||method==8){
		for (int i=0;i<SizeH;){//for all pixels
			for(int j=0;j<SizeW;){
				for (int wi=0;wi<method;wi++){//window
					for (int wj=0;wj<method;wj++){
						switch(method){
						case 2:
							ImageE[i+wi][j+wj]=htCompare(ImageE[i+wi][j+wj],B22[wi][wj]);
							break;
						case 4:
							ImageE2[i+wi][j+wj]=htCompare(ImageE[i+wi][j+wj],A44[wi][wj]);
							ImageE[i+wi][j+wj]=htCompare(ImageE[i+wi][j+wj],B44[wi][wj]);
							break;
						case 8:
							ImageE2[i+wi][j+wj]=htCompare(ImageE[i+wi][j+wj],A88[wi][wj]);
							ImageE[i+wi][j+wj]=htCompare(ImageE[i+wi][j+wj],B88[wi][wj]);
							break;
						}
					}
				}
				j=j+method;
			}
			i=i+method;
		}
	}


	//ERROR diffusion

	if (method==3||method==5){
		double thresh=0.5;// threshold for error diffusion
		int ERR; // error value
		for (int i=0;i<SizeH;i++){//for all pixels
			for(int j=0;j<SizeW;j++){
				if(ImageD[i][j]>=thresh){
					ERR=ImageD[i][j]-1;
					ImageD[i][j]=1;
					ImageD2[i][j]=1;
				}
				else{
					ERR=ImageD[i][j]-0;
					ImageD[i][j]=0;
					ImageD2[i][j]=0;
				}
				for (int Ei=-((method-1)/2);Ei<((method-1)/2+1);Ei++){ // marker for actual image array
					for (int Ej=-((method-1)/2);Ej<((method-1)/2+1);Ej++){
						for (int Di=0;Di<method;Di++){//marker for error diffusion table
							for (int Dj=0;Dj<method;Dj++){
								switch(method){
								case 3:
									ImageD[i+Ei][j+Ej]=ImageD[i+Ei][j+Ej]+(FS[Di][Dj]*ERR);
									break;
								case 5:
									ImageD[i+Ei][j+Ej]=ImageD[i+Ei][j+Ej]+(JJN[Di][Dj]*ERR);
									ImageD2[i+Ei][j+Ej]=ImageD2[i+Ei][j+Ej]+(S[Di][Dj]*ERR);
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	if (method==2||method==4||method==8){
		//remap [0,1] to [0,255]
		for (int i=0;i<SizeH;i++){
			for(int j=0;j<SizeW;j++){
				ImageF[i][j]=remap(ImageE[i][j]);
			}
		}
		if (!(file=fopen("man_B8x8.raw","wb"))){
			cout<<"Cannot open file"<<endl;
			exit(1);
		}
		fwrite(ImageF,sizeof(unsigned char),(SizeW*SizeH),file);
		fclose(file);
	}

	if (method==4 ||method==8){
		for (int i=0;i<SizeH;i++){
			for(int j=0;j<SizeW;j++){
				ImageF[i][j]=remap(ImageE2[i][j]);
			}
		}
		if (!(file=fopen("man_A8x8.raw","wb"))){
			cout<<"Cannot open file"<<endl;
			exit(1);
		}
		fwrite(ImageF,sizeof(unsigned char),(SizeW*SizeH),file);
		fclose(file);
	}

	if (method==3 ||method==5){
		for (int i=0;i<SizeH;i++){
			for(int j=0;j<SizeW;j++){
				ImageF[i][j]=remap(ImageD[i+2][j+2]);
			}
		}
		if (!(file=fopen("man_Floyd.raw","wb"))){
			cout<<"Cannot open file"<<endl;
			exit(1);
		}
		fwrite(ImageF,sizeof(unsigned char),(SizeW*SizeH),file);
		fclose(file);
	}
	if (method==5){
		for (int i=0;i<SizeH;i++){
			for(int j=0;j<SizeW;j++){
				ImageF[i][j]=remap(ImageD2[i+2][j+2]);
			}
		}
		if (!(file=fopen("man_Stucki.raw","wb"))){
			cout<<"Cannot open file"<<endl;
			exit(1);
		}
		fwrite(ImageF,sizeof(unsigned char),(SizeW*SizeH),file);
		fclose(file);
	}

}