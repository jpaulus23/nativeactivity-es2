// author John Paulus
#ifndef MESH_H
#define MESH_H

#define  LOG_TAG    "headdemo"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "headdemo", __VA_ARGS__))

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "androidassetmanager.hpp"

#define LINE_BUFFER_SIZE 256

// GETNUM just gets the next number from a line of input in an OBJ file
#ifndef GETNUM
#define GETNUM(lineBuffer, numBuffer, lindex, nindex, tval)  \
		nindex=0;\
		while ((lineBuffer[lindex] == ' ') || lineBuffer[lindex] == '/') lindex++;\
		while ((lineBuffer[lindex] != ' ') && (lineBuffer[lindex] != '/') && \
				(lineBuffer[lindex] != '\0') && (lineBuffer[lindex] != '\n') && (lindex != LINE_BUFFER_SIZE)) { \
			numBuffer[nindex] = lineBuffer[lindex]; \
			nindex++; \
			lindex++; \
		} \
		numBuffer[nindex] = '\0'; \
		tval = atoi(numBuffer);
#endif

struct VBOVerticesType{
	unsigned int Id;
	GLfloat* Vertices;
};

struct VBONormalsType{
	unsigned int Id;
	GLfloat* Normals;
};

struct VBOIndicesType{
	unsigned int Id;
	unsigned short* Indices;
};

struct VBOTexCoordsType{
	unsigned int Id;
	GLfloat* TexCoords;
};

class Mesh {

public:

	VBOVerticesType VBOVertices;
	VBONormalsType VBONormals;
	VBOIndicesType VBOIndices;
	VBOTexCoordsType VBOTexCoords;

	unsigned int IndexCount;

	float* Positions;
	float* Normals;
	unsigned short* Indices;
	float* UVs;

	float CenterX;
	float CenterY;
	float CenterZ;

	void deleteMesh();
	void loadMesh(char* filename);
};

void Mesh::deleteMesh() {
	LOGI("Deleting Buffer %i",this->VBOVertices.Id);
	glDeleteBuffers(1, &this->VBOVertices.Id);

	glDeleteBuffers(1, &this->VBONormals.Id);
	glDeleteBuffers(1, &this->VBOIndices.Id);
	glDeleteBuffers(1, &this->VBOTexCoords.Id);
}

void Mesh::loadMesh(char* filename) {
	//load the OBJ file

	//FILE* fin;
	//fin = fopen(filename, "r");
	//if (!fin) return;

	AAsset* meshFileAsset = AndroidAssetManager::Inst()->openAsset((char*)filename);

	if(!meshFileAsset)
		return;

	int filelength = AAsset_getLength(meshFileAsset);

	LOGI("Mesh Asset: %s, size: %i", filename, filelength);

	// temporary input buffers

	int* posIndex;
	int* normalIndex;
	int* uvIndex;
	int* faceIndex;

	float* filePosition;
	float* fileNormal;
	float* fileUV;

	float x,y,z,u,v;

	char lineBuffer[LINE_BUFFER_SIZE];
	char numBuffer[32];
	int lindex=0;
	int nindex=0;
	int ival, uvval, nval;

	int VertexCount=0;
	int VertexIndex=0;  //0 to VertexCount*3

	int NormalCount=0;
	int NormalIndex=0;

	int UVCount=0;
	int UVIndex=0;

	int FaceCount=0;
	int FaceIndex=0;

	int PositionIndexElement=0;
	int UVIndexElement=0;
	int NormalIndexElement=0;

	float sumX,sumY,sumZ;
	sumX=sumY=sumZ=0.0f;

	//parse file once to count the vertices, normals, etc
	while (AndroidAssetManager::Inst()->getLineFromAsset(lineBuffer, LINE_BUFFER_SIZE, meshFileAsset) != 0) {
		switch (lineBuffer[0]) {
		case 'v':
			// case vertex information
			if (lineBuffer[1] == ' ') {
				// regular vertex point
				VertexCount++;

			}
			else if (lineBuffer[1] == 't') {
				// texture coordinates
				UVCount++;
			}

			else if (lineBuffer[1] == 'n') {
				// normal vector
				NormalCount++;
			}
			break;

		case 'f':
			FaceCount++;

			break;
		default:
			break;

		}

	}

	//fseek(fin,0,SEEK_SET);
	AAsset_seek(meshFileAsset,0,SEEK_SET);

	//now parse the file again (ugh) to extract the elements in the arrays

	posIndex =     (int*) malloc (sizeof(int) * 3* FaceCount);
	normalIndex =  (int*) malloc (sizeof(int) * 3* FaceCount);
	uvIndex =      (int*) malloc (sizeof(int) * 3* FaceCount);

	LOGI("posIndex array size: %i",    3*FaceCount);
	LOGI("normalIndex array size: %i", 3*FaceCount);
	LOGI("uvIndex array size: %i",     3*FaceCount);

	filePosition = (float*) malloc(sizeof(float) * 3* VertexCount);
	fileNormal =   (float*) malloc(sizeof(float) * 3* NormalCount);
	fileUV =       (float*) malloc(sizeof(float) * 2* UVCount);

	LOGI("Loading this: %s", filename);
	LOGI("Vertex Count: %i",VertexCount);
	LOGI("Normal Count: %i",NormalCount);
	LOGI("UV Count: %i",UVCount);

	LOGI("filePosition array size: %i",    3 * VertexCount) ;
	LOGI("fileNormal array size: %i", 3*NormalCount);
	LOGI("fileUV array size: %i",     2*UVCount);


	char UStringBuffer[20];
	char VStringBuffer[20];

	int UCounter=0;

	// parse the data in
	while (AndroidAssetManager::Inst()->getLineFromAsset(lineBuffer, LINE_BUFFER_SIZE, meshFileAsset)) {

		switch (lineBuffer[0]) {
		case 'v':
			// case vertex information
			if (lineBuffer[1] == ' ') {
				// regular vertex point
				sscanf(&lineBuffer[2], "%f %f %f", &x, &y, &z);

				filePosition[VertexIndex++]=x;
				filePosition[VertexIndex++]=y;
				filePosition[VertexIndex++]=z;

				sumX += x;
				sumY += y;
				sumZ += z;

			} else if (lineBuffer[1] == 't') {
				// texture coordinates
				sscanf(&lineBuffer[3], "%f %f", &u, &v);
				fileUV[UVIndex++]=u;
				fileUV[UVIndex++]=v;

			} else if (lineBuffer[1] == 'n') {
				// normal vector
				sscanf(&lineBuffer[3], "%f %f %f", &x, &y, &z);
				fileNormal[NormalIndex++]=x;
				fileNormal[NormalIndex++]=y;
				fileNormal[NormalIndex++]=z;

			}
			break;

		case 'f':
			// case face information
			//LOGI("%s",lineBuffer);
			lindex = 2;

			for (int i=0; i < 3; i++) {

				GETNUM(lineBuffer, numBuffer, lindex, nindex, ival)

				// obj files go from 1..n, this just allows me to access the memory
				// directly by droping the index value to 0...(n-1)
				ival--;

				posIndex[PositionIndexElement++] = ival;

				if (lineBuffer[lindex] == '/') {
					lindex++;
					GETNUM(lineBuffer, numBuffer, lindex, nindex, uvval)
					uvIndex[UVIndexElement++]=uvval-1;
				}

				if (lineBuffer[lindex] == '/') {
					lindex++;
					GETNUM(lineBuffer, numBuffer, lindex, nindex, nval)

					normalIndex[NormalIndexElement++]=nval-1;
				}

				lindex++;
			}
			break;

		}

	}

	AAsset_close(meshFileAsset);

	this->CenterX = sumX/VertexCount;
	this->CenterY = sumY/VertexCount;
	this->CenterZ = sumZ/VertexCount;

	// merge everything back into one index array instead of multiple index arrays
	//mergeIndicies(filePosition, fileNormal, fileUV, posIndex, normalIndex, uvIndex);

	bool useNormals;

	if(NormalCount>0)
		useNormals = true;
	else
		useNormals=false;

	bool useUVs;

	if(UVCount > 0)
		useUVs = true;
	else
		useUVs = false;


	if (!useNormals && !useUVs) {
		this->Positions = filePosition;
		this->Indices = (unsigned short*) &posIndex;
		return;
	}
	else	{
		this->Positions = (float*) malloc(sizeof(float) * FaceCount * 3 * 3);
		this->Normals=    (float*) malloc(sizeof(float) * FaceCount * 3 * 3);
		this->Indices =   (unsigned short*)   malloc(sizeof(unsigned short) *   FaceCount * 3);
		this->UVs =       (float*) malloc(sizeof(float) * FaceCount * 3 *2);

		LOGI("this Array Sizes: %i",FaceCount*3);
		LOGI("this TexCoord Array Size: %i",FaceCount*6);
	}

	// assumes that vertexIndex = normalIndex = uvIndex

	int thisPosIndex=0;
	int thisNormalIndex = 0;
	int thisUVIndex = 0;

	this->IndexCount = 0;

	for (int i=0; i < 3* FaceCount; i++) {

		this->Positions[thisPosIndex++] = filePosition[posIndex[i]*3+0];
		this->Positions[thisPosIndex++] = filePosition[posIndex[i]*3+1];
		this->Positions[thisPosIndex++] = filePosition[posIndex[i]*3+2];




		if (useNormals) {
			this->Normals[thisNormalIndex++] =  fileNormal[normalIndex[i]*3+0] ;
			this->Normals[thisNormalIndex++] =  fileNormal[normalIndex[i]*3+1] ;
			this->Normals[thisNormalIndex++] =  fileNormal[normalIndex[i]*3+2] ;
		}


		if (useUVs) {


			this->UVs[thisUVIndex++] =  fileUV[uvIndex[i]*2+0] ;
			this->UVs[thisUVIndex++] =  fileUV[uvIndex[i]*2+1] ;
		}

		this->Indices[i]=i;
		this->IndexCount++;
	}

	free(filePosition);
	free(fileNormal);
	free(fileUV);

	free(posIndex);
	free(normalIndex);
	free(uvIndex);

	LOGI("centerx: %f",this->CenterX);
	LOGI("centery: %f",this->CenterY);
	LOGI("centerz: %f",this->CenterZ);


	//build VBOs
	this->VBOVertices.Vertices =  this->Positions;
	this->VBONormals.Normals =  this->Normals;
	this->VBOTexCoords.TexCoords = this->UVs;
	this->VBOIndices.Indices = this->Indices;

	this->Positions =0;
	this->Normals=0;
	this->UVs=0;
	this->Indices=0;

	//vertex array built, now load into GFX card memory.
	glGenBuffers(1, &this->VBOVertices.Id);
	glBindBuffer( GL_ARRAY_BUFFER, this->VBOVertices.Id);
	glBufferData( GL_ARRAY_BUFFER, 3*FaceCount*3*sizeof(GLfloat), this->VBOVertices.Vertices, GL_STATIC_DRAW);
	glBindBuffer( GL_ARRAY_BUFFER, 0);

	//normals also built, load them into GFX card memory
	glGenBuffers(1, &this->VBONormals.Id);
	glBindBuffer( GL_ARRAY_BUFFER, this->VBONormals.Id);
	glBufferData( GL_ARRAY_BUFFER, 3*FaceCount*3*sizeof(GLfloat), this->VBONormals.Normals, GL_STATIC_DRAW);
	glBindBuffer( GL_ARRAY_BUFFER, 0);

	//texcoords also built, load them into GFX card memory
	//this->VBOTexCoords.Id = (unsigned int*) malloc(sizeof(GLuint));
	glGenBuffers(1, &this->VBOTexCoords.Id);
	glBindBuffer( GL_ARRAY_BUFFER, this->VBOTexCoords.Id);
	glBufferData( GL_ARRAY_BUFFER, 6*FaceCount * sizeof(GLfloat), this->VBOTexCoords.TexCoords, GL_STATIC_DRAW);
	glBindBuffer( GL_ARRAY_BUFFER, 0);

	//index buffer
	glGenBuffers(1, &this->VBOIndices.Id);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->VBOIndices.Id);
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, this->IndexCount * sizeof(unsigned short), this->VBOIndices.Indices,GL_STATIC_DRAW);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,0);

	LOGI("VBOVertices.Id = %i", this->VBOVertices.Id);
	LOGI("VBONormals.Id = %i", this->VBONormals.Id);
	LOGI("VBOTexCoords.Id = %i", this->VBOTexCoords.Id);
	LOGI("VBOIndices.Id = %i", this->VBOIndices.Id);

}

/********
*
*    OLD
*
*
*********/
/*
struct MeshType{

	VBOVerticesType VBOVertices;
	VBONormalsType VBONormals;
	VBOIndicesType VBOIndices;
	VBOTexCoordsType VBOTexCoords;

	unsigned int IndexCount;

	float* Positions;
	float* Normals;
	unsigned short* Indices;
	float* UVs;

	float CenterX;
	float CenterY;
	float CenterZ;
};

void deleteMesh(MeshType* m) {
	LOGI("Deleting Buffer %i",m->VBOVertices.Id);
	glDeleteBuffers(1, &m->VBOVertices.Id);

	glDeleteBuffers(1, &m->VBONormals.Id);
	glDeleteBuffers(1, &m->VBOIndices.Id);
	glDeleteBuffers(1, &m->VBOTexCoords.Id);
}

void loadMesh(MeshType* Mesh, char* filename)
{
	//load the OBJ file
	FILE* fin;
	fin = fopen(filename, "r");
	if (!fin) return;

	// temporary input buffers

	int* posIndex;
	int* normalIndex;
	int* uvIndex;
	int* faceIndex;

	float* filePosition;
	float* fileNormal;
	float* fileUV;

	float x,y,z,u,v;

	char lineBuffer[LINE_BUFFER_SIZE];
	char numBuffer[32];
	int lindex=0;
	int nindex=0;
	int ival, uvval, nval;

	int VertexCount=0;
	int VertexIndex=0;  //0 to VertexCount*3

	int NormalCount=0;
	int NormalIndex=0;

	int UVCount=0;
	int UVIndex=0;

	int FaceCount=0;
	int FaceIndex=0;

	int PositionIndexElement=0;
	int UVIndexElement=0;
	int NormalIndexElement=0;

	float sumX,sumY,sumZ;
	sumX=sumY=sumZ=0.0f;



	//parse file once to count the vertices, normals, etc
	while (fgets(lineBuffer, LINE_BUFFER_SIZE, fin)) {
		switch (lineBuffer[0]) {
		case 'v':
			// case vertex information
			if (lineBuffer[1] == ' ') {
				// regular vertex point
				VertexCount++;

			}
			else if (lineBuffer[1] == 't') {
				// texture coordinates
				UVCount++;
			}

			else if (lineBuffer[1] == 'n') {
				// normal vector
				NormalCount++;
			}
			break;

		case 'f':
			FaceCount++;

			break;

		}

	}


	fseek(fin,0,SEEK_SET);

	//now parse the file again (ugh) to extract the elements in the arrays

	posIndex =     (int*) malloc (sizeof(int) * 3* FaceCount);
	normalIndex =  (int*) malloc (sizeof(int) * 3* FaceCount);
	uvIndex =      (int*) malloc (sizeof(int) * 3* FaceCount);

	LOGI("posIndex array size: %i",    3*FaceCount);
	LOGI("normalIndex array size: %i", 3*FaceCount);
	LOGI("uvIndex array size: %i",     3*FaceCount);

	filePosition = (float*) malloc(sizeof(float) * 3* VertexCount);
	fileNormal =   (float*) malloc(sizeof(float) * 3* NormalCount);
	fileUV =       (float*) malloc(sizeof(float) * 2* UVCount);

	LOGI("Loading Mesh: %s", filename);
	LOGI("Vertex Count: %i",VertexCount);
	LOGI("Normal Count: %i",NormalCount);
	LOGI("UV Count: %i",UVCount);

	LOGI("filePosition array size: %i",    3 * VertexCount) ;
	LOGI("fileNormal array size: %i", 3*NormalCount);
	LOGI("fileUV array size: %i",     2*UVCount);


	char UStringBuffer[20];
	char VStringBuffer[20];

	int UCounter=0;

	// parse the data in
	while (fgets(lineBuffer, LINE_BUFFER_SIZE, fin)) {

		//LOGI("%s",lineBuffer);



		switch (lineBuffer[0]) {
		case 'v':
			// case vertex information
			if (lineBuffer[1] == ' ') {
				// regular vertex point
				sscanf(&lineBuffer[2], "%f %f %f", &x, &y, &z);

				filePosition[VertexIndex++]=x;
				filePosition[VertexIndex++]=y;
				filePosition[VertexIndex++]=z;

				sumX += x;
				sumY += y;
				sumZ += z;

			} else if (lineBuffer[1] == 't') {
				// texture coordinates
				sscanf(&lineBuffer[3], "%f %f", &u, &v);
				fileUV[UVIndex++]=u;
				fileUV[UVIndex++]=v;

			} else if (lineBuffer[1] == 'n') {
				// normal vector
				sscanf(&lineBuffer[3], "%f %f %f", &x, &y, &z);
				fileNormal[NormalIndex++]=x;
				fileNormal[NormalIndex++]=y;
				fileNormal[NormalIndex++]=z;

			}
			break;

		case 'f':
			// case face information
			//LOGI("%s",lineBuffer);
			lindex = 2;

			for (int i=0; i < 3; i++) {

				GETNUM(lineBuffer, numBuffer, lindex, nindex, ival)

				// obj files go from 1..n, this just allows me to access the memory
				// directly by droping the index value to 0...(n-1)
				ival--;

				posIndex[PositionIndexElement++] = ival;

				if (lineBuffer[lindex] == '/') {
					lindex++;
					GETNUM(lineBuffer, numBuffer, lindex, nindex, uvval)
					uvIndex[UVIndexElement++]=uvval-1;
				}

				if (lineBuffer[lindex] == '/') {
					lindex++;
					GETNUM(lineBuffer, numBuffer, lindex, nindex, nval)

					normalIndex[NormalIndexElement++]=nval-1;
				}

				lindex++;
			}
			break;

		}

	}

	fclose(fin);


	Mesh->CenterX = sumX/VertexCount;
	Mesh->CenterY = sumY/VertexCount;
	Mesh->CenterZ = sumZ/VertexCount;

	// merge everything back into one index array instead of multiple index arrays
	//mergeIndicies(filePosition, fileNormal, fileUV, posIndex, normalIndex, uvIndex);


	bool useNormals;

	if(NormalCount>0)
		useNormals = true;
	else
		useNormals=false;

	bool useUVs;

	if(UVCount > 0)
		useUVs = true;
	else
		useUVs = false;


	if (!useNormals && !useUVs) {
		Mesh->Positions = filePosition;
		Mesh->Indices = (unsigned short*) &posIndex;
		return;
	}
	else	{
		Mesh->Positions = (float*) malloc(sizeof(float) * FaceCount * 3 * 3);
		Mesh->Normals=    (float*) malloc(sizeof(float) * FaceCount * 3 * 3);
		Mesh->Indices =   (unsigned short*)   malloc(sizeof(unsigned short) *   FaceCount * 3);
		Mesh->UVs =       (float*) malloc(sizeof(float) * FaceCount * 3 *2);

		LOGI("Mesh Array Sizes: %i",FaceCount*3);
		LOGI("Mesh TexCoord Array Size: %i",FaceCount*6);
	}

	// assumes that vertexIndex = normalIndex = uvIndex

	int MeshPosIndex=0;
	int MeshNormalIndex = 0;
	int MeshUVIndex = 0;

	Mesh->IndexCount = 0;




	for (int i=0; i < 3* FaceCount; i++) {

		Mesh->Positions[MeshPosIndex++] = filePosition[posIndex[i]*3+0];
		Mesh->Positions[MeshPosIndex++] = filePosition[posIndex[i]*3+1];
		Mesh->Positions[MeshPosIndex++] = filePosition[posIndex[i]*3+2];




		if (useNormals) {
			Mesh->Normals[MeshNormalIndex++] =  fileNormal[normalIndex[i]*3+0] ;
			Mesh->Normals[MeshNormalIndex++] =  fileNormal[normalIndex[i]*3+1] ;
			Mesh->Normals[MeshNormalIndex++] =  fileNormal[normalIndex[i]*3+2] ;
		}


		if (useUVs) {


			Mesh->UVs[MeshUVIndex++] =  fileUV[uvIndex[i]*2+0] ;
			Mesh->UVs[MeshUVIndex++] =  fileUV[uvIndex[i]*2+1] ;
		}

		Mesh->Indices[i]=i;
		Mesh->IndexCount++;
	}

	free(filePosition);
	free(fileNormal);
	free(fileUV);

	free(posIndex);
	free(normalIndex);
	free(uvIndex);



	LOGI("centerx: %f",Mesh->CenterX);
	LOGI("centery: %f",Mesh->CenterY);
	LOGI("centerz: %f",Mesh->CenterZ);


	//build VBOs
	Mesh->VBOVertices.Vertices =  Mesh->Positions;
	Mesh->VBONormals.Normals =  Mesh->Normals;
	Mesh->VBOTexCoords.TexCoords = Mesh->UVs;
	Mesh->VBOIndices.Indices = Mesh->Indices;

	Mesh->Positions =0;
	Mesh->Normals=0;
	Mesh->UVs=0;
	Mesh->Indices=0;

	//vertex array built, now load into GFX card memory.
	glGenBuffers(1, &Mesh->VBOVertices.Id);
	glBindBuffer( GL_ARRAY_BUFFER, Mesh->VBOVertices.Id);
	glBufferData( GL_ARRAY_BUFFER, 3*FaceCount*3*sizeof(GLfloat), Mesh->VBOVertices.Vertices, GL_STATIC_DRAW);
	glBindBuffer( GL_ARRAY_BUFFER, 0);

	//normals also built, load them into GFX card memory
	glGenBuffers(1, &Mesh->VBONormals.Id);
	glBindBuffer( GL_ARRAY_BUFFER, Mesh->VBONormals.Id);
	glBufferData( GL_ARRAY_BUFFER, 3*FaceCount*3*sizeof(GLfloat), Mesh->VBONormals.Normals, GL_STATIC_DRAW);
	glBindBuffer( GL_ARRAY_BUFFER, 0);

	//texcoords also built, load them into GFX card memory
	//Mesh->VBOTexCoords.Id = (unsigned int*) malloc(sizeof(GLuint));
	glGenBuffers(1, &Mesh->VBOTexCoords.Id);
	glBindBuffer( GL_ARRAY_BUFFER, Mesh->VBOTexCoords.Id);
	glBufferData( GL_ARRAY_BUFFER, 6*FaceCount * sizeof(GLfloat), Mesh->VBOTexCoords.TexCoords, GL_STATIC_DRAW);
	glBindBuffer( GL_ARRAY_BUFFER, 0);

	//index buffer
	glGenBuffers(1, &Mesh->VBOIndices.Id);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Mesh->VBOIndices.Id);
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, Mesh->IndexCount * sizeof(unsigned short), Mesh->VBOIndices.Indices,GL_STATIC_DRAW);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,0);

	LOGI("VBOVertices.Id = %i", Mesh->VBOVertices.Id);
	LOGI("VBONormals.Id = %i", Mesh->VBONormals.Id);
	LOGI("VBOTexCoords.Id = %i", Mesh->VBOTexCoords.Id);
	LOGI("VBOIndices.Id = %i", Mesh->VBOIndices.Id);


}
*/
#endif
