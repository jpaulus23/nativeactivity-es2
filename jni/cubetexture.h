#ifndef CUBETEXTURE_H
#define CUBETEXTURE_H

#include "texture.h"

class CubeTexture{

public:

	int Height;
	int Width;
	GLuint *ID;    //identifier for texture in GL system

	//
	Texture RightTex;
	Texture LeftTex;
	Texture TopTex;
	Texture BottomTex;
	Texture BackTex;
	Texture FrontTex;

	bool loadCubeTexture();

};

bool CubeTexture::loadCubeTexture(){

	if(!RightTex.loadTGAOnly((char*)"right.tga"))
		return false;
	if(!LeftTex.loadTGAOnly((char*)"left.tga"))
			return false;
	if(!TopTex.loadTGAOnly((char*)"top.tga"))
			return false;
	if(!BottomTex.loadTGAOnly((char*)"bottom.tga"))
			return false;
	if(!BackTex.loadTGAOnly((char*)"back.tga"))
				return false;
	if(!FrontTex.loadTGAOnly((char*)"front.tga"))
			return false;

	this->ID = new GLuint;
	glGenTextures(1, &this->ID[0]);

	glBindTexture(GL_TEXTURE_CUBE_MAP, this->ID[0]);

	//set cube map parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);

	//load right face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			0,                         //mipmap level
			GL_RGBA,                   //format of data (only 3 since no alpha)
			RightTex.Width, //width
			RightTex.Height,//height
			0,                         //border
			GL_RGBA,                    //format
			GL_UNSIGNED_BYTE,          //type
			RightTex.RGB);    //pointer to data

	//load left face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			0,                         //mipmap level
			GL_RGBA,                   //format of data (only 3 since no alpha)
			LeftTex.Width, //width
			LeftTex.Height,//height
			0,                         //border
			GL_RGBA,                    //format
			GL_UNSIGNED_BYTE,          //type
			LeftTex.RGB);    //pointer to data
	//load top face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			0,                         //mipmap level
			GL_RGBA,                   //format of data (only 3 since no alpha)
			TopTex.Width, //width
			TopTex.Height,//height
			0,                         //border
			GL_RGBA,                    //format
			GL_UNSIGNED_BYTE,          //type
			TopTex.RGB);    //pointer to data
	//load bottom face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			0,                         //mipmap level
			GL_RGBA,                   //format of data (only 3 since no alpha)
			BottomTex.Width, //width
			BottomTex.Height,//height
			0,                         //border
			GL_RGBA,                    //format
			GL_UNSIGNED_BYTE,          //type
			BottomTex.RGB);    //pointer to data

	//load back face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			0,                         //mipmap level
			GL_RGBA,                   //format of data (only 3 since no alpha)
			BackTex.Width, //width
			BackTex.Height,//height
			0,                         //border
			GL_RGBA,                    //format
			GL_UNSIGNED_BYTE,          //type
			BackTex.RGB);    //pointer to data

	//load front face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			0,                         //mipmap level
			GL_RGBA,                   //format of data (only 3 since no alpha)
			FrontTex.Width, //width
			FrontTex.Height,//height
			0,                         //border
			GL_RGBA,                    //format
			GL_UNSIGNED_BYTE,          //type
			FrontTex.RGB);    //pointer to data

	//we need to delete the data on the cpu side since the data has been copied to GPU

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


	free(RightTex.RGB);
	free(LeftTex.RGB);
	free(TopTex.RGB);
	free(BottomTex.RGB);
	free(BackTex.RGB);
	free(FrontTex.RGB);

	LOGI("Done loading cube texture");
	return true;
}


#endif
