/*
 * contrastHIST.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: gu
 *
 *      histogram equalization
 *      arguments: program_name input_image.raw output_image.raw
 *		output: cdf and digital method image along with CDF and PDF of the input image
 *
 *		CDF and PDF processing part can be used to generate cdf and pdf for other images.
 *
 *
 */



#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <fstream>

using namespace std;


void PDF(const unsigned char data[][300], int *output);
void CDF(const int pdf[], int *output);
void file_text(int input_array[], char mode);

int main(int argc, char *argv[]) //command line arguments

{
	// Define file pointer and variables
	FILE *file;


	// Allocate image data array
	unsigned char Imagedata[400][300];//size of unsigned char is 0-255

	unsigned char ImageQ1b1[400][300], ImageQ1b2[400][300];
	int pdf [256] = {0};
	int cdf [256] = {0}; // distribution, added up to 1.0

	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), 400*300, file);
	fclose(file);

	//calculation

	PDF(Imagedata, pdf);
	file_text(pdf,'P');
	CDF(pdf, cdf);
	file_text(cdf,'C');


	// histogram equal: CDF
	int value;
	double temp;
	for (int i=0;i<=399;i++){
		for (int j=0;j<=299;j++){
			value=Imagedata[i][j];
			temp=(static_cast<double>(cdf[value])/120000);
			ImageQ1b1[i][j]=static_cast<int>(temp*255);
		}
	}


	// histogram equal: digital implementation, 256 division
	// 469 pixels for first 255 divion, and 405 pixels for last division
	// FOR last division: GreyLevel=255, it will be automatically filled up.

	// FOR first 255 division: 0-254
	int count[255]={0};
	int currentCount=0; // mark the position of the immediate unfilled new histogram division
	// filling new image based on GreyLevel Order,, this increase run time, but easy to implement
	for (int GreyLv=0;GreyLv<=254;GreyLv++){
		for (int i=0;i<=399;i++){
			for (int j=0;j<=299;j++){
				if(Imagedata[i][j]==GreyLv){
					if (count[currentCount]==469){// if current is filled, apply to next division
						currentCount++;
						ImageQ1b2[i][j]=currentCount;
						count[currentCount]=1;
					}
					else{//if divison is not filled
						ImageQ1b2[i][j]=currentCount;
						count[currentCount]=count[currentCount]+1;
					}
				}
			}
		}
	}


	// Write image data (filename specified by second argument) from image data matrix
	string savename=argv[2];

	// Save into different file names based on argument input
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen((savename+"HIST_a.raw").c_str(),"wb"))) { // cdf method
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImageQ1b1, sizeof(unsigned char), 400*300, file);
	fclose(file);

	savename=argv[2];
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen((savename+"HIST_d.raw").c_str(),"wb"))) {  //digital method
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImageQ1b2, sizeof(unsigned char), 400*300, file);
	fclose(file);

	return 0;
}



// function that create a PDF based on input image data
void PDF(const unsigned char data[][300], int *output)
{
	int temp;
	for (int i=0;i<=399;i++){
		for (int j=0;j<=299;j++){
			temp=data[i][j];
			output[temp]=output[temp]+1;
		}
	}

	return;
}

void CDF(const int pdf[], int *output)
{
	double value=0;
	for (int i=0;i<=255;i++){
		value=value+pdf[i];
		output[i]=value;
	}
}

// save array into a text file: mode = 'pdf' OR 'cdf'
void file_text(int input_array[], char mode)
{
	ofstream myfile;
	if (mode=='P')
		myfile.open ("pdf.txt");
	else if (mode=='C')
		myfile.open("cdf.txt");
	string ddd;
	double kkk;
	for (int i=0;i<=255;i++){
		kkk=input_array[i];
		myfile << kkk;
		myfile <<"\n";
	}
	myfile.close();
	return;
}


