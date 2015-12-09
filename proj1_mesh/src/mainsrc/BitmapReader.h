#ifndef BITMAPREADER_H
#define BITMAPREADER_H

#include <Windows.h>
#include <cstdio>
#include <cmath>

class BitmapReader{
	public:
		int height, width;
		BitmapReader(char *fileName);
		unsigned char *imageMatrix(){return img;}
		~BitmapReader();
	private:
		unsigned char *img;
};

#endif // BITMAPREADER_H