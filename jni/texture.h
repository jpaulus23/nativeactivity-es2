#ifndef TEXTURE_H
#define TEXTURE_H

#ifndef LOG_TAG
#define  LOG_TAG    "headdemo"
#endif

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "androidassetmanager.hpp"

class Texture {

public:

	int Height;
	int Width;
	GLuint *ID;    //identifier for texture in GL system
	unsigned char* RGB; //color data

	Texture(char* filename);
	Texture();

	bool loadTexture(char* filename);
	unsigned char* loadTGAOnly(char* filename);

};

Texture::Texture(char* filename) {

	this->loadTexture(filename);
}

Texture::Texture() {
}

unsigned char* Texture::loadTGAOnly(char* filename) {

	AAsset* texFileAsset = AndroidAssetManager::Inst()->openAsset((char*)filename);
	int length=0;

	if(texFileAsset) {

		length = AAsset_getLength(texFileAsset);

		LOGI("Texture Asset: %s, size: %i", filename, length);

		char headerbytes[18];

		for (int j=0; j < 18; j++) {
			AAsset_read(texFileAsset, &headerbytes[j], 1);
		}

		unsigned char idlength = headerbytes[0];
		unsigned char colormaptype = headerbytes[1];
		unsigned char datatypecode = headerbytes[2];
		short colormaplength = ((((short) headerbytes[6]) << 8) | 0x00FF) & (((short) headerbytes[5]) | 0xFF00);
		short width = ((((short) headerbytes[13]) << 8) | 0x00FF) & (((short) headerbytes[12]) | 0xFF00);
		short height = ((((short) headerbytes[15]) << 8) | 0x00FF) & (((short) headerbytes[14]) | 0xFF00);

		unsigned char bitdepth = headerbytes[16];

		this->Width = (int) width;
		this->Height = (int) height;

		LOGI("Tex Width: %i",this->Width);
		LOGI("Tex WHeight: %i",this->Height);

		this->RGB = (unsigned char*) malloc(sizeof(unsigned char)*width*height*4);

		// if anything is not up to par, just throw an exception from the parser
		if (this->RGB == 0) {
			AAsset_close(texFileAsset);
			LOGI("Unable to malloc memory");
			return NULL;
		}
		if (datatypecode != 2 && datatypecode != 10) {
			AAsset_close(texFileAsset);
			LOGI("Can only handle image type 2 and 10");
			return NULL;
		}
		if (bitdepth != 24 && bitdepth != 32) {
			AAsset_close(texFileAsset);
			LOGI("Can only handle pixel depths of 24 and 32");
			return NULL;
		}
		if (colormaptype != 0 && colormaptype != 1) {
			AAsset_close(texFileAsset);
			LOGI("Can only handle colour map types of 0 and 1");
			return NULL;
		}

		unsigned int skipover = idlength;
		skipover += colormaplength;
		AAsset_seek(texFileAsset, skipover, SEEK_CUR);

		unsigned int byteCount = bitdepth / 8;
		unsigned char p[5];

		int n =0;
		while (n < width*height) {
			if (datatypecode == 2) {                     /* Uncompressed */

				if (AAsset_read(texFileAsset,p,byteCount) != byteCount) {
					AAsset_close(texFileAsset);
					free(this->RGB);
					LOGI("Unexpected end of file");
					return NULL;
				}

				this->RGB[n*4+0] = p[2];
				this->RGB[n*4+1] = p[1];
				this->RGB[n*4+2] = p[0];
				this->RGB[n*4+3] = p[3];

				n++;
			} else if (datatypecode == 10) {             /* Compressed */

				if (AAsset_read(texFileAsset,p,byteCount+1) != byteCount+1) {
					AAsset_close(texFileAsset);
					LOGI("Unexpected end of file");
					return NULL;
				}
				int j = p[0] & 0x7f;

				this->RGB[n*3+0] = p[3];
				this->RGB[n*3+1] = p[2];
				this->RGB[n*3+2] = p[1];
				this->RGB[n*4+3] = (byteCount == 4)?p[4]:255;

				n++;

				if (p[0] & 0x80) {         /* RLE chunk */
					for (int i=0;i<j;i++) {
						this->RGB[n*3+0] = p[3];
						this->RGB[n*3+1] = p[2];
						this->RGB[n*3+2] = p[1];
						this->RGB[n*4+3] = (byteCount == 4)?p[4]:255;
						n++;
					}
				} else {                   /* Normal chunk */
					for (int i=0;i<j;i++) {

						if (AAsset_read(texFileAsset,p,byteCount)  != byteCount) {
							AAsset_close(texFileAsset);
							LOGI("Unexpected end of file");
							return false;
						}
						this->RGB[n*4+0] = p[3];
						this->RGB[n*4+1] = p[2];
						this->RGB[n*4+2] = p[1];
						this->RGB[n*4+3] = (byteCount == 4)?p[4]:255;
						n++;
					}
				}
			}
		}
		AAsset_close(texFileAsset);
		return this->RGB;
	}
	return NULL;
}

bool Texture::loadTexture(char* filename) {

	unsigned char* bytes = this->loadTGAOnly(filename); //note: this->RGB gets set in this function if successful so we don't need to set that variable here

	//now do the GL stuff

	if(bytes) {

		this->ID = (GLuint*) malloc(sizeof(GLuint));

		glGenTextures(1, &this->ID[0]);
		glBindTexture(GL_TEXTURE_2D, this->ID[0]);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering

		glTexImage2D(GL_TEXTURE_2D,
				0,                         //mipmap level
				GL_RGBA,                   //format of data (only 3 since no alpha)
				this->Width, //width
				this->Height,//height
				0,                         //border
				GL_RGBA,                    //format
				GL_UNSIGNED_BYTE,          //type
				this->RGB);    //pointer to data

		glBindTexture(GL_TEXTURE_2D,0);

		free(this->RGB); //cleanup RAM, the image is now residing in GL VRAM
		return true;
	}
	else {
		LOGI("Tex bytes are EMPTY!");
		return false;
	}
}

#endif
