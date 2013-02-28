#ifndef SKYBOX_H
#define SKYBOX_H

#include "texture.h"
#include "mesh.h"


/*
 *
 *              v2----------v3
 *             /|          /|
 *            / |         / |
 *           /  |        /  |
 *          /   v1------/---v0
 *         /    /      /    /
 *        v6---/------v7   /
 *        |   /       |   /
 *        |  /        |  /
 *        | /         | /
 *        v5----------v4
 *
 *
 *
 *
 */
float FrontVertices[12] = {3.0,-3.0,3.0, //v0
			             -3.0,-3.0,3.0, //v1
			             -3.0,3.0,3.0,  //v2
			             3.0,3.0,3.0};  //v3

float FrontTexCoords[8] = {1.0,0.0,
					0.0,0.0,
					0.0,1.0,
					1.0,1.0};

unsigned char FrontIndex[6] = {0,2,1,0,3,2};

float LeftVertices[12] = {-3.0,-3.0,3.0,  //v1
		              -3.0,-3.0,-3.0, //v5
		              -3.0,3.0,-3.0,  //v6
		              -3.0,3.0,3.0};  //v2

float LeftTexCoords[8] = {1.0,0.0,
		            0.0,0.0,
		            0.0,1.0,
		            1.0,1.0};

unsigned char LeftIndex[6] = {0,2,1,
                           0,3,2};

float RightVertices[12] = {3.0,-3.0,3.0,  //v0
		              3.0,-3.0,-3.0, //v4
		              3.0,3.0,-3.0,  //v7
		              3.0,3.0,3.0};  //v3

float RightTexCoords[8] = {0.0,0.0,
		            1.0,0.0,
		            1.0,1.0,
		            0.0,1.0};

unsigned char RightIndex[6] = {0,1,2,
                           0,2,3};

float BackVertices[12] = {3.0,-3.0,-3.0,  //v4
		              -3.0,-3.0,-3.0, //v5
		              -3.0,3.0,-3.0,  //v6
		              3.0,3.0,-3.0};  //v7

float BackTexCoords[8] = {0.0,0.0,
		            1.0,0.0,
		            1.0,1.0,
		            0.0,1.0};

unsigned char BackIndex[6] = {0,1,2,
                           0,2,3};

float BottomVertices[12] = {3.0,-3.0,-3.0,  //v4
		              -3.0,-3.0,-3.0, //v5
		              -3.0,-3.0,3.0,  //v1
		              3.0,-3.0,3.0};  //v0

float BottomTexCoords[8] = {
							0.0,0.0,
							0.0,1.0,
							1.0,1.0,
							1.0,0.0
      						};

unsigned char BottomIndex[6] = {0,2,1,
                           0,3,2};

float TopVertices[12] = {3.0,3.0,-3.0,  //v7
						-3.0,3.0,-3.0, //v6
						-3.0,3.0,3.0,  //v2
						3.0,3.0,3.0};  //v3

float TopTexCoords[8] = {0.0, 1.0,
						0.0, 0.0,
						1.0,0.0,
						1.0, 1.0

						};

unsigned char TopIndex[6] = {0,1,2,
                           0,2,3};

struct SkyBoxFace
{
	Texture Tex;
	float* Vertices; // (x,y,z) * 4 = 12 floats
	float* TexCoords; // (s,t) * 4 = 8 floats
	unsigned char* Index;

	unsigned int VBOVertId;
	unsigned int VBOTexCoordId;
	unsigned int VBOIndexId;
};

class SkyBox
{

public:

	SkyBoxFace Left;
	SkyBoxFace Front;
	SkyBoxFace Right;
	SkyBoxFace Top;
	SkyBoxFace Back;
	SkyBoxFace Bottom;

	SkyBox()
	{
	}

	bool loadSkyBox()
	{

		Left.Tex.loadTexture((char*)"left.tga");
		Front.Tex.loadTexture((char*)"front.tga");
        	Right.Tex.loadTexture((char*)"right.tga");
		Top.Tex.loadTexture((char*)"top.tga");
		Back.Tex.loadTexture((char*)"back.tga");

		Bottom.Tex.loadTexture((char*)"bottom.tga");
		LOGI("stupid debug trace");
		/* Front */
		//vert
		glGenBuffers(1, &Front.VBOVertId);
		glBindBuffer( GL_ARRAY_BUFFER, Front.VBOVertId);
		glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), FrontVertices, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//tex
		glGenBuffers(1, &Front.VBOTexCoordId);
		glBindBuffer( GL_ARRAY_BUFFER, Front.VBOTexCoordId);
		glBufferData( GL_ARRAY_BUFFER, 8*sizeof(GLfloat), FrontTexCoords, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//index buffer
		glGenBuffers(1, &Front.VBOIndexId);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Front.VBOIndexId);
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned char), FrontIndex, GL_STATIC_DRAW);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,0);

		/* Left */
		//vert
		glGenBuffers(1, &Left.VBOVertId);
		glBindBuffer( GL_ARRAY_BUFFER, Left.VBOVertId);
		glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), LeftVertices, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//tex
		glGenBuffers(1, &Left.VBOTexCoordId);
		glBindBuffer( GL_ARRAY_BUFFER, Left.VBOTexCoordId);
		glBufferData( GL_ARRAY_BUFFER, 8*sizeof(GLfloat), LeftTexCoords, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//index buffer
		glGenBuffers(1, &Left.VBOIndexId);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Left.VBOIndexId);
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned char), LeftIndex, GL_STATIC_DRAW);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,0);

		/* Right */
		//vert
		glGenBuffers(1, &Right.VBOVertId);
		glBindBuffer( GL_ARRAY_BUFFER, Right.VBOVertId);
		glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), RightVertices, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//tex
		glGenBuffers(1, &Right.VBOTexCoordId);
		glBindBuffer( GL_ARRAY_BUFFER, Right.VBOTexCoordId);
		glBufferData( GL_ARRAY_BUFFER, 8*sizeof(GLfloat), RightTexCoords, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//index buffer
		glGenBuffers(1, &Right.VBOIndexId);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Right.VBOIndexId);
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned char), RightIndex, GL_STATIC_DRAW);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,0);

		/*Back*/
		//vert
		glGenBuffers(1, &Back.VBOVertId);
		glBindBuffer( GL_ARRAY_BUFFER, Back.VBOVertId);
		glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), BackVertices, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//tex
		glGenBuffers(1, &Back.VBOTexCoordId);
		glBindBuffer( GL_ARRAY_BUFFER, Back.VBOTexCoordId);
		glBufferData( GL_ARRAY_BUFFER, 8*sizeof(GLfloat), BackTexCoords, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//index buffer
		glGenBuffers(1, &Back.VBOIndexId);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Back.VBOIndexId);
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned char), BackIndex, GL_STATIC_DRAW);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,0);

		/*Bottom*/

		//vert
		glGenBuffers(1, &Bottom.VBOVertId);
		glBindBuffer( GL_ARRAY_BUFFER, Bottom.VBOVertId);
		glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), BottomVertices, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//tex
		glGenBuffers(1, &Bottom.VBOTexCoordId);
		glBindBuffer( GL_ARRAY_BUFFER, Bottom.VBOTexCoordId);
		glBufferData( GL_ARRAY_BUFFER, 8*sizeof(GLfloat), BottomTexCoords, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//index buffer
		glGenBuffers(1, &Bottom.VBOIndexId);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Bottom.VBOIndexId);
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned char), BottomIndex, GL_STATIC_DRAW);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,0);

		/*Top*/

		//vert
		glGenBuffers(1, &Top.VBOVertId);
		glBindBuffer( GL_ARRAY_BUFFER, Top.VBOVertId);
		glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), TopVertices, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//tex
		glGenBuffers(1, &Top.VBOTexCoordId);
		glBindBuffer( GL_ARRAY_BUFFER, Top.VBOTexCoordId);
		glBufferData( GL_ARRAY_BUFFER, 8*sizeof(GLfloat), TopTexCoords, GL_STATIC_DRAW);
		glBindBuffer( GL_ARRAY_BUFFER, 0);

		//index buffer
		glGenBuffers(1, &Top.VBOIndexId);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Top.VBOIndexId);
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned char), TopIndex, GL_STATIC_DRAW);
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,0);
	


		return true;
	}
};
#endif
