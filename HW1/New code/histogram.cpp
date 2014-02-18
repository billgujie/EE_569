/*
 * histogram.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: gu
 *
 *      a stand alone program for histogram generation
 *      argument: none
 *      ouput: text files of cdp and pdf
 *      !!!To run the program, need change line:37,81 and 83 for input and output name!!!
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
	int pdf [256] = {0};
	int cdf [256] = {0}; // distribution, added up to 1.0

	if (!(file=fopen("contrastSR.raw","rb"))) { // change file name
		cout << "Cannot open file"<<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), 400*300, file);
	fclose(file);

	//calculation

	PDF(Imagedata, pdf);
	file_text(pdf,'P');
	CDF(pdf, cdf);
	file_text(cdf,'C');
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
		myfile.open ("contrastSR_pdf.txt");
	else if (mode=='C')
		myfile.open("contrastSR_cdf.txt");
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




