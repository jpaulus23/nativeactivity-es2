#ifndef TGAIMAGE_H
#define TGAIMAGE_H

struct TGAImageType 
{
	unsigned char* rgb;
	int width, height;
};

class TGAImage
{

public:

	unsigned char* rgb;
	int width, height;
};

#endif



