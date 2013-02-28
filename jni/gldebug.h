//helper functions for the GL stuff

#ifndef GLDEBUG_H
#define GLDEBUG_H

#ifndef LOG_TAG
#define  LOG_TAG    "headdemo"
#endif

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)



static void printGLString(const char *name, GLenum s) {
	const char *v = (const char *) glGetString(s);
	LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
	//for (GLint error = glGetError(); error; error = glGetError()) {
	//	LOGI("after %s() glError (0x%x)\n", op, error);
	//}

	GLint error = glGetError();
	switch(error){
	case GL_INVALID_ENUM:
		LOGI("after %s() glError (0x%x)\n", op, error);
		LOGI("GL_INVALID_ENUM","ERROR");
		break;
	case GL_INVALID_VALUE:
		LOGI("after %s() glError (0x%x)\n", op, error);
		LOGI("GL_INVALID_VALUE","ERROR");
		break;
	case GL_INVALID_OPERATION:
		LOGI("after %s() glError (0x%x)\n", op, error);
		LOGI("GL_INVALID_OPERATION","ERROR");
		break;

	case GL_OUT_OF_MEMORY:
		LOGI("after %s() glError (0x%x)\n", op, error);
		LOGI("GL_OUT_OF_MEMORY","ERROR");
		break;
	case GL_NO_ERROR:
		break;
	default: break;
	}

}

#endif
