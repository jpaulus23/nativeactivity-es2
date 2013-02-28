#ifndef SHADER_H
#define SHADER_H

#include "androidassetmanager.hpp"

#include "gldebug.h"

class Shader{

public:

	GLuint ID;

	GLuint compileAndLinkShader(GLenum shaderType, const char* pSource);
	GLuint loadShaderSourceFromFile(GLenum shaderType, const char* filename);
	bool createShader(char* VertexFile,char* FragFile);
};

GLuint Shader::compileAndLinkShader(GLenum shaderType, const char* pSource) {

	GLuint shader = glCreateShader(shaderType);
	if (shader) {
		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				char* buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGE("Could not compile shader %d:\n%s\n",
							shaderType, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

GLuint Shader::loadShaderSourceFromFile(GLenum shaderType, const char* filename)
{
	AAsset* shaderFileAsset = AndroidAssetManager::Inst()->openAsset((char*)filename);

	if(shaderFileAsset){

		LOGI("Shader Asset: %s, size: %i", filename, AAsset_getLength(shaderFileAsset));
		int length = AAsset_getLength(shaderFileAsset);

		if(length > 0) {
			char* ShaderSource = (char*)AAsset_getBuffer(shaderFileAsset);
			ShaderSource[length]= '\0';

			if(shaderType==GL_VERTEX_SHADER) {
				//LOGI("====VERTEX SHADER ASSET SOURCE====\n%s",ShaderSource);
				return compileAndLinkShader(GL_VERTEX_SHADER,ShaderSource);
			}
			else {
				//LOGI("====FRAGMENT SHADER ASSET SOURCE====\n    %s",ShaderSource);
				return compileAndLinkShader(GL_FRAGMENT_SHADER,ShaderSource);
			}
		}
	}
	else {
		LOGI("NULL ASSET RETURNED! (%s)", filename);
	}
}

bool Shader::createShader(char* VertexFile,char* FragFile) {

	GLuint vertexShader = loadShaderSourceFromFile(GL_VERTEX_SHADER,VertexFile);
	if (!vertexShader) {
		return false;
	}

	GLuint pixelShader = loadShaderSourceFromFile(GL_FRAGMENT_SHADER, FragFile);
	if (!pixelShader) {
		return false;
	}

	GLuint program = glCreateProgram();

	if (program) {
		glAttachShader(program, vertexShader);
		checkGlError("glAttachShader");
		glAttachShader(program, pixelShader);
		checkGlError("glAttachShader");
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {
				char* buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, 255, NULL, buf);
					LOGE("Could not link program:\n%s\n", buf);
					free(buf);
				}
			}
			glDeleteProgram(program);
			program = 0;
			return false;
		}
	}
	this->ID = program;
	return true;
}
#endif
