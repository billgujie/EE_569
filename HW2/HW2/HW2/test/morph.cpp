#include "StdAfx.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include <string.h>

using namespace std;


int C2B(unsigned char value){ // change 255 to zero(background) and 0 to 1(dots)
	if (value==255)
		return 0;
	else if(value==0)
		return 1;
}

int bondcount(int value[]){ // calculate the bond

	int result=0;
	result=value[0]*1+value[1]*2+value[2]*1+value[3]*2+value[4]*2+value[5]*1+value[6]*2+value[7]*1;	
	return result;
}

int maskMatch(int value[]){ // convert the 8bit binary pattern into decimal for mask matching
	int result=0;
	int windowWT[8]={128,64,32,16,8,4,2,1};
	for (int i=0;i<8;i++){
		result=result+value[i]*windowWT[i];
	}
	return result;
}


int gg(int v[]){
	//pattern for shrink
	int result=0;//default=not match
	
	if(((v[1]+v[4]==1 && v[2]==1 && v[5]==1))||(v[1]+v[3]==1 && v[0]==1 && v[7]==1)||(v[6]+v[3]==1 && v[2]==1 && v[50	32	128	2	8	40	96	192	144	20	6	3	9	112	200	73	42	100	44	193	145	52	38	137	131
]==1))
	




}


int main(int argc, char *argv[])
{
	FILE *file;
	const int SizeW=550;
	const int SizeH=400;
	unsigned char *Image = new unsigned char [SizeW*SizeH];
	unsigned char ImageE[SizeH+2][SizeW+2]; // for edge extension
	unsigned char ImageM[SizeH+2][SizeW+2]; //for conditional marking




	int op=0;	

	//	Operation Type:
	// Shrink: 1&3&4..........0
	// Thin: 2&3&4............1
	// Skeletonize: 2&3&5.....2
	//...........................
	//		0=null
	//		1=S
	//		2=TK
	//		3=STK
	//		4=ST
	//		5=K

	int opArray[3][3]={
		{1,3,4}, 
		{2,3,4},
		{2,3,5}
	};

	//mask construction

	//STK conditional mask
	int maskArray[12][10]={
		{0,0,0,0,0,0,0,0,0,0},//leave first row blank for easier indexing
		{32, 128, 4, 1, 256, 256, 256, 256, 256, 256},//B=1
		{8, 64, 16, 2, 256, 256, 256, 256, 256, 256},//B=2
		{40, 96, 192, 144, 20, 6, 3, 9, 256, 256},//B=3
		{72, 80, 18, 10, 41, 224, 148, 7, 256, 256},//B=4
		{200, 73, 112, 42, 104, 208, 22, 11, 256, 256},//B=5
		{201, 116, 232, 105, 240, 212, 150, 23, 15, 43},//B=6
		{233, 244, 151, 47, 256, 256, 256, 256, 256, 256},//B=7
		{107, 248, 214, 31, 256, 256, 256, 256, 256, 256},//B=8
		{235, 111, 252, 249, 246, 215, 159, 63, 256, 256},//B=9
		{239, 253, 247, 191, 256, 256, 256, 256, 256, 256},//B=10
		{251, 254, 223, 127, 256, 256, 256, 256, 256, 256}//B=11
	};


	// operation lookup table, values here represents Operation parameters [0-5]
	int opLUT [12][10]={
		{0,0,0,0,0,0,0,0,0,0},//blank
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0},//B=1
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0},//B=2
		{1, 1, 1, 1, 1, 1, 1, 1, 0, 0},//B=3
		{2, 2, 2, 2, 3, 3, 3, 3, 0, 0},//B=4
		{4, 4, 4, 4, 4, 4, 4, 4, 0, 0},//B=5
		{4, 4, 3, 3, 3, 3, 3, 3, 3, 3},//B=6
		{3, 3, 3, 3, 0, 0, 0, 0, 0, 0},//B=7
		{3, 3, 3, 3, 0, 0, 0, 0, 0, 0},//B=8
		{3, 3, 3, 3, 3, 3, 3, 3, 0, 0},//B=9
		{3, 3, 3, 3, 0, 0, 0, 0, 0, 0},//B=10
		{5, 5, 5, 5, 0, 0, 0, 0, 0, 0}//B=11
	};

	//ST Unconditional mask
	int STU1[24]={32,128,2,8,40,96,192,144,20,6,3,9,112,200,73,42,100,44,193,145,52,38,137,131};




	// read PCB.raw file
	if (!(file=fopen("PCB.raw","rb"))) { // change file name
		cout << "Cannot open file"<<endl;
		exit(1);
	}
	fread(Image, sizeof(unsigned char), SizeW*SizeH, file);
	fclose(file);
	// save it to ImageE to complete Edge extesion 
	int count=0;
	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageE[i+1][j+1]=Image[count];
			count++;
		}
	}
	delete [] Image;

	// {0,1} binary conversion
	for (int i=0;i<SizeH+2;i++){
		for(int j=0;j<SizeW+2;j++){
			ImageE[i][j]=C2B(ImageE[i][j]);
		}
	}
	// Initialize Mask array with all 0
	for (int i=0;i<SizeH+2;i++){
		for(int j=0;j<SizeW+2;j++){
			ImageM[i][j]=0;
		}
	}

	// conditional scan
	int LUT;
	int windowArr[8];// 3x3 window values into array
	int B=0;//bond value
	int V=0;//8-bit to int value
	for (int Ei=1;Ei<SizeH+1;Ei++){
		for (int Ej=1;Ej<SizeW+1;Ej++){
			if (ImageE[Ei][Ej]==0){ // MISS
				ImageM[Ei][Ej]=0;
			}
			else{	//HIT
				windowArr[0]=ImageE[Ei-1][Ej-1];
				windowArr[1]=ImageE[Ei-1][Ej];
				windowArr[2]=ImageE[Ei-1][Ej+1];
				windowArr[3]=ImageE[Ei][Ej-1];
				windowArr[4]=ImageE[Ei][Ej+1];
				windowArr[5]=ImageE[Ei+1][Ej-1];
				windowArr[6]=ImageE[Ei+1][Ej];
				windowArr[7]=ImageE[Ei+1][Ej+1];
				B=bondcount(windowArr);
				V=maskMatch(windowArr);
				for (int opI=0;opI<3;opI++){
					LUT=opArray[op][opI];
					for (int arrayI=0;arrayI<10;arrayI++){
						if (opLUT[B][arrayI]==LUT){
							for (int arrayI2=0;arrayI2<10;){
								if (maskArray[B][arrayI2]==V){
									ImageM[Ei][Ej]=1;
								}
							}
						}
					}
				}
			}
		}
	}

	// Unconditional check
	if (op==2){
		//skeleton()
	}
	else{// For S & T


	}



	if (!(file=fopen("PCB_E.raw","wb"))){
		cout<<"Cannot open file"<<endl;
		exit(1);
	}
	fwrite(ImageE,sizeof(unsigned char),((SizeW+2)*(SizeH+2)),file);
	fclose(file);




}

