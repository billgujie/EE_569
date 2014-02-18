#ifndef IMAGE_H
#define IMAGE_H

class Image{
private:
	int imageHeight, imageWidth;
	unsigned char *imageData;
public:
	Image(int, int, int);
	~Image();
};

Image:: Image(int height, int width, int byte){
	imageHeight = height;
	imageWidth = width;
	imageData = new unsigned char [height * width *byte];

};

Image:: ~Image(){
	delete [] imageData;
};



#endif