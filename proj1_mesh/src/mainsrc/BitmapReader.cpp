#include "BitmapReader.h"

BitmapReader::BitmapReader(char *fileName){
	img = NULL;

	tagBITMAPFILEHEADER fh;
	tagBITMAPINFOHEADER infoh;
	FILE *fp = fopen(fileName, "rb");

	if(!fp){
		printf("\nFailed to read Bitmap");
		return;
	}

	fread(&fh, sizeof(fh), 1, fp);
	fread(&infoh, sizeof(infoh), 1, fp);

	if(infoh.biBitCount != 24){
		printf("\n File not 24 bit BMP file");
		return;
	}

	width = infoh.biWidth;
	height = infoh.biHeight;

	img = new unsigned char(infoh.biSizeImage);

	fread(img, infoh.biSizeImage, 1, fp);

	unsigned char temp;

	for(int i = 0; i < infoh.biSizeImage; i++){
		temp = img[i];
		img[i] = img[i+2];
		img[i+2] = temp;
		i += 2;
	}
}

BitmapReader::~BitmapReader(){
	if(img) delete []img;
}