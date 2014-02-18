
#include "StdAfx.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;


int C2B(unsigned char value){ // change 0 to 1(dots), and rest to 0
	if (value==0)
		return 1;
	else
		return 0;
}

int remap(unsigned char value){// map [0,1] to [0 255]
	if (value==1)
		return 255;
	else
		return 0;
}

bool isolateCheck(int a[]){
	if( a[0]+a[1]+a[2]+a[3]+a[4]+a[5]+a[6]+a[7]==0 )
		return true;
	else
		return false;
}

int check(int a[]){ // if any of its neighbour is a black pixel
	if (a[0]+a[1]+a[2]+a[3]+a[4]+a[5]+a[6]+a[7]>=1)
		return 1;
	else
		return 0;
}

double round(double d)
{
  return floor(d + 0.5);
}

int imOR(unsigned char a, unsigned char b){
	if ((a+b)>0)
		return 1;
	else
		return 0;
}

int main(int argc, char *argv[])
{
	FILE *file;
	const int SizeW=250;
	const int SizeH=250;
	unsigned char *Image = new unsigned char [SizeW*SizeH];
	unsigned char ImageE[SizeH+2][SizeW+2]; // for edge extension
	unsigned char ImageF[SizeH][SizeW]; // final output image array
	unsigned char ImageO[SizeH+2][SizeW+2]; //  image Outline
	unsigned char ImageT[SizeH+2][SizeW+2]; //  image missing tooth outline
	unsigned char ImageC[SizeH+2][SizeW+2]; //  combined



	// read shrinked gear file
	if (!(file=fopen("GearToothS.raw","rb"))) { // change file name
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
	//delete [] Image;
	

	// {0,1} binary conversion
	for (int i=0;i<SizeH+2;i++){
		for(int j=0;j<SizeW+2;j++){
			ImageE[i][j]=C2B(ImageE[i][j]);
		}
	}

	for (int i=0;i<SizeH+2;i++){
		for(int j=0;j<SizeW+2;j++){
			ImageO[i][j]=C2B(ImageO[i][j]);
		}
	}

	for (int i=0;i<SizeH+2;i++){
		for(int j=0;j<SizeW+2;j++){
			ImageT[i][j]=C2B(ImageT[i][j]);
		}
	}


	// Center pixel calculation================================
	// isolate pixels detection
	int arr[8];
	double pixels[5][2];
	if (!(file=fopen("Coord.txt","wb+"))){
		cout<<"Cannot open file"<<endl;
		exit(1);
	}
	count=0;
	for (int Ei=1;Ei<SizeH+1;Ei++){ // scan each pixels in current image array
		for (int Ej=1;Ej<SizeW+1;Ej++){
			if (ImageE[Ei][Ej]==1){
				arr[0]=ImageE[Ei-1][Ej-1];
				arr[1]=ImageE[Ei-1][Ej];
				arr[2]=ImageE[Ei-1][Ej+1];
				arr[3]=ImageE[Ei][Ej-1];
				arr[4]=ImageE[Ei][Ej+1];
				arr[5]=ImageE[Ei+1][Ej-1];
				arr[6]=ImageE[Ei+1][Ej];
				arr[7]=ImageE[Ei+1][Ej+1];
				if(isolateCheck(arr)){
					fprintf (file, "%i,%i\n",Ej,Ei);
					pixels[count][0]=Ej; // minus 1 to get rid of the extended edge effect!
					pixels[count++][1]=Ei;
				}
			}
		}
	}
	
	// calculate gear center based on 4 points  @ http://blogs.mathworks.com/loren/2011/08/29/intersecting-lines/
	// for this homework, we can safely assume that the first isolate pixel is the artifact from shrinking
	// therefore we only use the rest 4 pixels
	double slop1=(pixels[1][1]-pixels[4][1])/(pixels[1][0]-pixels[4][0]);
	double slop2=(pixels[2][1]-pixels[3][1])/(pixels[2][0]-pixels[3][0]);
	double b1=pixels[1][1]-slop1*pixels[1][0];
	double b2=pixels[2][1]-slop2*pixels[2][0];

	double Cx=((b2-b1)/(slop1-slop2)); // GearCenter_x
	double Cy=((slop1*Cx)+b1); // GearCenter_y

	Cx=round(Cx);
	Cy=round(Cy);
	
	fprintf (file, "%.0f,%.0f\n",Cx,Cy);
	fclose(file);

	//END of Center pixel calculation=================================
	

	// OUTLINE detection=============================
	// read original Gear
	if (!(file=fopen("GearTooth.raw","rb"))) { // change file name
		cout << "Cannot open file"<<endl;
		exit(1);
	}
	fread(Image, sizeof(unsigned char), SizeW*SizeH, file);
	fclose(file);
	// save it to ImageE to complete Edge extesion 
	count=0;
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

	// outline pixels scan
	int windowArr[8];
	for (int Ei=1;Ei<SizeH+1;Ei++){ 
		for (int Ej=1;Ej<SizeW+1;Ej++){
			if (ImageE[Ei][Ej]==0){ // check all white part only
				windowArr[0]=ImageE[Ei-1][Ej-1];
				windowArr[1]=ImageE[Ei-1][Ej];
				windowArr[2]=ImageE[Ei-1][Ej+1];
				windowArr[3]=ImageE[Ei][Ej-1];
				windowArr[4]=ImageE[Ei][Ej+1];
				windowArr[5]=ImageE[Ei+1][Ej-1];
				windowArr[6]=ImageE[Ei+1][Ej];
				windowArr[7]=ImageE[Ei+1][Ej+1];
				ImageO[Ei][Ej]=check(windowArr); // outline pixels are saved as 1 here
			}
		}
	}

	int x_new, y_new;
	//check ImageO against it self by construct lines and detect opposite pixels based on Center pixel
	for (int Ei=1;Ei<SizeH+1;Ei++){ // for conditional scan
		for (int Ej=1;Ej<SizeW+1;Ej++){
			if (ImageO[Ei][Ej]==1){ // check the outline pixels
				x_new=Cx+(Cx-Ej);
				y_new=Cy+(Cy-Ei);
				if (ImageE[y_new][x_new]!=0){
					ImageT[y_new][x_new]=1;
				}
			}
		}
	}

	// END of OUTLINE detection=============================

	int temp;
	//Combine images
	for (int Ei=1;Ei<SizeH+1;Ei++){ // for conditional scan
		for (int Ej=1;Ej<SizeW+1;Ej++){
			if (ImageE[Ei][Ej]==0)
				temp=1;
			else
				temp=0;
			ImageC[Ei][Ej]=imOR(ImageT[Ei][Ej],temp);
		}
	}

	//save for output, and remap to [0-255 range]
	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageF[i][j]=remap(ImageC[i+1][j+1]);
		}
	}
	if (!(file=fopen("GearTooth_Result.raw","wb"))){
		cout<<"Cannot open file"<<endl;
		exit(1);
	}
	fwrite(ImageF,sizeof(unsigned char),(SizeW*SizeH),file);
	fclose(file);


	//save for output, and remap to [0-255 range]
	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageF[i][j]=remap(ImageO[i+1][j+1]);
		}
	}
	if (!(file=fopen("GearTooth_Outline.raw","wb"))){
		cout<<"Cannot open file"<<endl;
		exit(1);
	}
	fwrite(ImageF,sizeof(unsigned char),(SizeW*SizeH),file);
	fclose(file);


	//save for output, and remap to [0-255 range]
	for (int i=0;i<SizeH;i++){
		for(int j=0;j<SizeW;j++){
			ImageF[i][j]=remap(ImageT[i+1][j+1]);
		}
	}
	if (!(file=fopen("GearTooth_MissMask.raw","wb"))){
		cout<<"Cannot open file"<<endl;
		exit(1);
	}
	fwrite(ImageF,sizeof(unsigned char),(SizeW*SizeH),file);
	fclose(file);





}
