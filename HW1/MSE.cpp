/*
 * contrastHIST.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: gu
 */

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <math.h>

using namespace std;

double SE(int value1, int value2);
double mean(const unsigned char data[][512],  int row, int col);
double variance(int value, double mean);
double SSIM(double muA, double muB, double vaA, double vaB, double cvAB);
int main(int argc, char *argv[]) //command line arguments
{
	// Define file pointer and variables
	FILE *file;

	// Check for proper syntax
	if (argc < 2){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name[0] input_image1.raw[1] input_image2.raw[2]" << endl;
		return 0;
	}

	// Allocate image data array
	unsigned char Imagedata1[512][512];//
	unsigned char Imagedata2[512][512];//
	unsigned char ImagedataErr[512][512];// absolute error map
	unsigned char ImagedataSSIM[512][512];
	double ImagedataSSIM_index[512][512];
	double mse_value;
	double psnr_value;
	double ssim_value;
	double muA,muB,vaA,vaB,cvAB;

	argv[1]="peppers.raw";
	argv[2]="denoise_M5_L3_null.raw";

	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata1, sizeof(unsigned char), 512*512, file);
	fclose(file);


	if (!(file=fopen(argv[2],"rb"))) {
		cout << "Cannot open file: " << argv[2] <<endl;
		exit(1);
	}
	fread(Imagedata2, sizeof(unsigned char), 512*512, file);
	fclose(file);


	//calculation MSE
	double value=0;
	double index=0;
	for (int i=0;i<=511;i++){
		for (int j=0;j<=511;j++){
			value=value+SE(Imagedata1[i][j],Imagedata2[i][j]);
			ImagedataErr[i][j]=abs(Imagedata1[i][j]-Imagedata2[i][j]);
			muA=mean(Imagedata1,i,j);
			muB=mean(Imagedata2,i,j);
			vaA=variance(Imagedata1[i][j], muA);
			vaB=variance(Imagedata2[i][j], muB);
			cvAB=(Imagedata1[i][j]-muA)*(Imagedata2[i][j]-muB);
			index=SSIM(muA,muB,vaA,vaB,cvAB);
			ImagedataSSIM_index[i][j]=index;
			ImagedataSSIM[i][j]=static_cast<int>((index+1)/2*255);//SSIM range:-[1,1]
		}
	}
	// pooling SSIM index : middle part has weight of 4
	for (int i=0;i<=511;i++){ // first scan
		for (int j=0;j<=511;j++){
			ssim_value=ssim_value+ImagedataSSIM_index[i][j];
		}
	}
	for (int i=85;i<=426;i++){ //second scan, add 3 weight for [85~426]*[85~426]
		for (int j=85;j<=426;j++){
			ssim_value=ssim_value+ImagedataSSIM_index[i][j]*3;
		}
	}
	ssim_value=ssim_value/(512*512+342*342*3);
	mse_value=value/(512*512);
	psnr_value=10*log10(255*255/mse_value);

	// Save into different file names based on argument input
	string savename=argv[2];
	// Save absolute error map
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen(("QA_AbEr_"+savename+".raw").c_str(),"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImagedataErr, sizeof(unsigned char), 512*512, file);
	fclose(file);

	// Save SSIM map
	if (!(file=fopen(("QA_SSIM_"+savename+".raw").c_str(),"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImagedataSSIM, sizeof(unsigned char), 512*512, file);
	fclose(file);

	// output txt file for MSE and PSNR
	if (!(file=fopen("MSE_PSNR_SSIM.txt","a+"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fprintf (file, "%s\t%f\t%f\t%f\n",argv[2],mse_value,psnr_value,ssim_value);
	fclose(file);
	return 0;
}

double mean(const unsigned char data[][512],  int row, int col){
	//mean of window 5x5, unweighted
	double value;
	double sum=0;
	for (int i=row-2;i<=row+2;i++){
		for (int j=col-2;j<=col+2;j++){
			sum=sum+data[i][j];
		}
	}
	value=sum/25;
	return value;
}

double variance(int value, double mean){
	double temp=value;
	double result;
	result=temp-mean;
	return (result*result);
}

double SE(int value1, int value2){  //square error
	double temp=0;
	temp=(value1-value2);
	double square=(temp*temp);
	return square;
}

double SSIM(double muA, double muB, double vaA, double vaB, double cvAB){
	// calculate SSIM index based on equation on EE569_HW1.pdf
	int L=255;
	double c1=0.01*0.01*L*L; // (0.01L)^2
	double c2=0.03*0.03*L*L; // (0.03L)^2
	double result, p1, p2, p3, p4;
	p1=2*muA*muB+c1;
	p2=2*cvAB+c2;
	p3=muA*muA+muB*muB+c1;
	p4=vaA+vaB+c2;
	result=(p1*p2)/(p3*p4);
	return result;
}
