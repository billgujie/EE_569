// Linear contrast enhancement
// this program assume image size is 400x300
// arguments: program_name input_image.raw output_image.raw
// output: two linear scaled image using clipping and absolute value

#include <stdio.h>
#include <iostream>
#include <stdlib.h>


using namespace std;


int contrastLS(int input, int fmax, int fmin, char mode);//linear scaling contrast enhancement

int main(int argc, char *argv[]) //command line arguments

{
	// Define file pointer and variables
	FILE *file;

	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name[0] input_image.raw[1] output_image.raw[2]" << endl;
		return 0;
	}


	// Allocate image data array
	unsigned char Imagedata[400][300];//size of unsigned char is 0-255
	unsigned char ImageQ1b1[400][300], ImageQ1b2[400][300];

	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), 400*300, file);
	fclose(file);

	//calculation
	int temp;
	char mode;
	mode='C';
	for (int i=0; i<=399; i++){
		for (int j=0; j<=299; j++){
			temp=Imagedata[i][j];
			ImageQ1b1[i][j]=contrastLS(temp, 21, 0, mode);
		}
	}
	mode='A';
	for (int i=0; i<=399; i++){
		for (int j=0; j<=299; j++){
			temp=Imagedata[i][j];
			ImageQ1b2[i][j]=contrastLS(temp, 21, 0, mode);
		}
	}

	// Write image data (filename specified by second argument) from image data matrix
	string savename=argv[2];
	// Save into different file names based on argument input
	savename.erase(savename.end()-4, savename.end());
	if (!(file=fopen((savename+"CLIP.raw").c_str(),"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImageQ1b1, sizeof(unsigned char), 400*300, file);
	fclose(file);

	if (!(file=fopen((savename+"AVS.raw").c_str(),"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(ImageQ1b2, sizeof(unsigned char), 400*300, file);
	fclose(file);

	return 0;
}

int contrastLS(int input, int fmax, int fmin, char mode)
//	string mode: CLIP for clipping ; AVS for absolute value scaling
{
;
	int temp;
	temp=((input-fmin)*(255/(fmax-fmin)));
	switch (mode)
	{
	case 'C': // clipping
		if (temp<0)
			temp=0;
		else if (temp>255)
			temp=255;
		break;
	case 'A': //avs
		if (temp<0)
			temp=-temp;
		else if (temp>255)
			temp=-temp+255*2;
		break;
	}
	return temp;
}

