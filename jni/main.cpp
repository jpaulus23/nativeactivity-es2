#include <jni.h>
#include <errno.h>

//opengl headers
#include "es2util.h"
#include <EGL/egl.h>

//custom opengl resources
#include "gldebug.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "skybox.h"
#include "cubetexture.h"

//#include "cubetexture.h"

//asset manager (file loader)
#include "androidassetmanager.hpp"

//other stuff
#include "timer.h"

//android ndk stuff
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>

//phong shader
Shader PhongShader;
GLuint PositionAttributes=0;
GLuint NormalAttributes=0;
GLuint TexCoordAttributes=0;

//GLSL Uniforms of shaders
GLint  MVPMatrixUniform; //"pointer" to the uniform in the shader
GLint  EyePosUniform;
GLint  LightPosUniform;
GLint  TextureSampler;

//depth shader
Shader DepthShader;
GLuint DepthShaderMVPMatrixUniform=0;
GLuint DepthShaderPositionAttributes=0;

//skybox shader (texpassthru)
Shader TexPassThruShader;
GLint  TexPassThruSampler=-1;
GLint  TexPassThruMVPMatrixUniform=-1;
GLuint TexPassThruPositionAttributes=-1;
GLuint TexPassThruTexCoordAttributes=-1;

//environment mapping shader (uses cubetexture)

Shader EnvironmentMappingShader;
GLuint EnvironmentMappingShaderMVPMatrixUniform=-1;
GLuint EnvironmentMappingShaderPositionAttributes=-1;
GLuint EnvironmentMappingShaderNormalAttributes=-1;
GLint EnvironmentMappingShaderCubeSampler=-1;


//GL Matrices
ESMatrix    MVPMatrix;
ESMatrix3x3 NormalMatrix;
ESMatrix3x3 Upper3x3;
ESMatrix3x3 TransposeUpper3x3;

ESMatrix translateMatrix;
ESMatrix rotateMatrix;

int ContextWidth = 2;
int ContextHeight = 2;

//GL resources (mesh, tex, skybox, etc)
Mesh StatueMesh;
Texture StoneTexture;
SkyBox CubeSkyBox;
CubeTexture EnvironmentCubeTexture; //used for reflection mapping of model

//CubeTextureType EnvironmentCubeTexture; //not yet

//multitouch related
float RotateX=0.0f;
float RotateY=0.0f;

float ScaleAmount = 2.5f;

bool Wireframe=false;

int ShaderToUse =0; //0=phong, 1=depth to color.....lame!!!


/**
 * Our saved state data.
 */

struct saved_state {
    float angle;
    int32_t x;
    int32_t y;
};

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

   // int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    struct saved_state state;
};

/**
 * Initialize an EGL context for the current display.
 */

static int engine_init_display(struct engine* engine) {
    // initialize OpenGL ES and EGL


    const EGLint attribs[] = {
            EGL_NATIVE_VISUAL_ID, WINDOW_FORMAT_RGB_565,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    		EGL_BLUE_SIZE, 5,
            EGL_GREEN_SIZE, 6,
            EGL_RED_SIZE, 5,
            EGL_DEPTH_SIZE,1,
            EGL_NONE
    };



    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);


    EGLint majorVersion;
    EGLint minorVersion;

    eglInitialize(display, &majorVersion, &minorVersion);
    //eglInitialize(display, 0, 0);
    LOGI("OpenGL %i.%i", majorVersion,minorVersion);

    //query num of configs
    int* num_conf = new int[1];
    eglGetConfigs(display, NULL, 0, num_conf);  //if configuration array is null it still returns the number of configurations
    int configurations = num_conf[0];

    LOGI("total num configs: %i", configurations);

    //just some debugging info if the need arises...
    LOGI("EGL_OPENGL_ES2_BIT id:%i", EGL_OPENGL_ES2_BIT); //print the numerical code for the ES2 bit mask, etc
    LOGI("EGL_SURFACE_TYPE::EGL_WINDOW_BIT id:%i", EGL_WINDOW_BIT);
    LOGI("WINDOW_FORMAT_RGB_565 id:%i", WINDOW_FORMAT_RGB_565);

    //now query the configs
    EGLConfig* conf = new EGLConfig[configurations];
    eglGetConfigs(display, conf, configurations, num_conf);

    int* depth = new int[1];
	int* r = new int[1];
	int* g = new int[1];
	int* b = new int[1];
	int* a = new int[1];
	int* s = new int[1];
	int* renderType = new int[1];
	int* surfaceType = new int[1];
	int* formatType = new int[1];

	EGLConfig configToUse; //this is the one true config that we will use

    for(int i = 0; i < configurations; i++)
     {
      eglGetConfigAttrib(display, conf[i], EGL_DEPTH_SIZE, depth);
      eglGetConfigAttrib(display, conf[i], EGL_RED_SIZE, r);
      eglGetConfigAttrib(display, conf[i], EGL_GREEN_SIZE, g);
      eglGetConfigAttrib(display, conf[i], EGL_BLUE_SIZE, b);
      eglGetConfigAttrib(display, conf[i], EGL_ALPHA_SIZE, a);
      eglGetConfigAttrib(display, conf[i], EGL_RENDERABLE_TYPE, renderType);
      eglGetConfigAttrib(display, conf[i], EGL_STENCIL_SIZE, s);
      eglGetConfigAttrib(display, conf[i], EGL_SURFACE_TYPE, surfaceType);
      eglGetConfigAttrib(display, conf[i], EGL_NATIVE_VISUAL_ID, formatType);

      LOGI("(R%i,G%i,B%i,A%i)depth:(%i) stencil:(%i) surfaceType:(%i) renderType:(%i) formatType:(%i)",r[0],g[0],b[0],a[0],depth[0],s[0],surfaceType[0], renderType[0],formatType[0]);


      if((renderType[0] & EGL_OPENGL_ES2_BIT) > 0 &&
    	(surfaceType[0] & EGL_WINDOW_BIT) > 0 &&
    	(formatType[0] & WINDOW_FORMAT_RGB_565) > 0 &&
    	depth[0]>0)
      {

    	  configToUse=conf[i];

    	  LOGI("Config #%i" , i );

    	  LOGI("(R%i,G%i,B%i,A%i) %idepth %istencil %isurfaceType %iNativeVisualId",r[0],g[0],b[0],a[0],depth[0],s[0],surfaceType[0],formatType[0]);
      }

     }

    //bridge the pixel format back into android
    eglGetConfigAttrib(display, configToUse, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, configToUse, engine->app->window, NULL);

    if(surface == EGL_NO_SURFACE ) {
    	LOGW("Error making surface, EGL_NO_SURFACE");
    }

    //now create the OpenGL ES2 context
    const EGLint contextAttribs[] = {
    		EGL_CONTEXT_CLIENT_VERSION , 2,
    		EGL_NONE
    };

    context = eglCreateContext(display, configToUse, NULL, contextAttribs);

    if(context == EGL_NO_CONTEXT ) {
    	LOGW("Error making context, EGL_NO_CONTEXT");
    }

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);


    ContextWidth = w;
    ContextHeight = h;

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    engine->state.angle = 0;

    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    //GLubyte* rendererString = new GLubyte[512];
    const GLubyte* rendererString = rendererString=glGetString(GL_VERSION);
    LOGI("Renderer: %s",rendererString);


    //
	//
	// LOAD ALL RESOURCES
	//
	//

    //////////////////
    // PHONG SHADER //
    //////////////////

    if(!PhongShader.createShader((char*)"phong.vs",(char*)"phong.fs")) {
    		LOGE("Could not create phong program.");
    		return false;
    	}

	PositionAttributes = glGetAttribLocation(PhongShader.ID, "aPosition");
	NormalAttributes =   glGetAttribLocation(PhongShader.ID, "aNormal");
	TexCoordAttributes = glGetAttribLocation(PhongShader.ID, "aTexCoords");
	MVPMatrixUniform = glGetUniformLocation( PhongShader.ID, "MVPMatrixUniform" );
	EyePosUniform = glGetUniformLocation(PhongShader.ID,"EyePosUniform");
	LightPosUniform = glGetUniformLocation(PhongShader.ID,"LightPosUniform");
	TextureSampler = glGetUniformLocation(PhongShader.ID,"sTexture");

	LOGI("===PHONG-DEBUG VALUES===");
	LOGI("PositionAttributes: %i",PositionAttributes);
	LOGI("NormalAttributes: %i",NormalAttributes);
	LOGI("TexCoordAttributes: %i",TexCoordAttributes);

	LOGI("MVPMatrixUniform: %i",MVPMatrixUniform);
	LOGI("EyePosUniform: %i",EyePosUniform);
	LOGI("LightPosUniform: %i",LightPosUniform);

	LOGI("TextureSampler: %i",TextureSampler);
	LOGI("===END===");

    //////////////////
    // DEPTH SHADER //
    //////////////////

	if(!DepthShader.createShader((char*)"depthcolor.vs",(char*)"depthcolor.fs")) {

		LOGE("Could not create depth shader program.");
		return false;
	}

	DepthShaderMVPMatrixUniform = 	glGetUniformLocation(DepthShader.ID, "MVPMatrixUniform");
	DepthShaderPositionAttributes = glGetAttribLocation(DepthShader.ID, "aPosition");

	LOGI("===DEPTH-DEBUG VALUES===");
	LOGI("DepthShaderPositionAttributes: %i",DepthShaderPositionAttributes);
	LOGI("DepthShaderMVPMatrixUniform: %i",DepthShaderMVPMatrixUniform);
	LOGI("===END===");
	/////////////////////////////////
	// TEXPASSTHRU (SKYBOX) SHADER //
	/////////////////////////////////

	if(!TexPassThruShader.createShader((char*)"texpassthru.vs",(char*)"texpassthru.fs")) {
		LOGE("Could not create texpassthru program.");
		return false;
	}

	TexPassThruSampler = glGetUniformLocation(TexPassThruShader.ID,"sTexture");
	TexPassThruMVPMatrixUniform = glGetUniformLocation(TexPassThruShader.ID,"MVPMatrixUniform");

	TexPassThruPositionAttributes = glGetAttribLocation(TexPassThruShader.ID, "aPosition");
	TexPassThruTexCoordAttributes = glGetAttribLocation(TexPassThruShader.ID, "aTexCoords");

	LOGI("===TEXPASSTHRU-DEBUG VALUES===");
	LOGI("TexPassThruSampler: %i",TexPassThruSampler);
	LOGI("TexPassThruMVPMatrixUniform: %i",TexPassThruMVPMatrixUniform);
	LOGI("TexPassThruPositionAttributes: %i",TexPassThruPositionAttributes);
	LOGI("TexPassThruTexCoordAttributes: %i",TexPassThruTexCoordAttributes);
	LOGI("===END===");



    ////////////////////////
    // ENVIRONMENT SHADER //
    ////////////////////////

	if (!EnvironmentMappingShader.createShader((char*)"environmentcubemap.vs",(char*)"environmentcubemap.fs")) {
		LOGE("Could not create program.");
		return false;
	}

	EnvironmentMappingShaderMVPMatrixUniform=glGetUniformLocation(EnvironmentMappingShader.ID,"MVPMatrixUniform");
	EnvironmentMappingShaderPositionAttributes=glGetAttribLocation(EnvironmentMappingShader.ID, "aPosition");
	EnvironmentMappingShaderNormalAttributes=glGetAttribLocation(EnvironmentMappingShader.ID, "aNormal");
	EnvironmentMappingShaderCubeSampler=glGetUniformLocation(EnvironmentMappingShader.ID,"sCubeTexture");

	LOGI("===ENVIRONMENT-DEBUG VALUES===");
	LOGI("EnvironmentMappingShaderCubeSampler: %i",EnvironmentMappingShaderCubeSampler);
	LOGI("EnvironmentMappingShaderMVPMatrixUniform: %i",EnvironmentMappingShaderMVPMatrixUniform);
	LOGI("EnvironmentMappingShaderPositionAttributes: %i",EnvironmentMappingShaderPositionAttributes);
	LOGI("TexPassThruTeEnvironmentMappingShaderNormalAttributesxCoordAttributes: %i",EnvironmentMappingShaderNormalAttributes);
	LOGI("===END===");

	//Matrices
	esMatrixLoadIdentity(&MVPMatrix);

	//viewport
	glViewport(0, 0, w, h);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	checkGlError("glViewport");

	//load resources internally from the APK

	StatueMesh.loadMesh((char*)"athena.obj");
	checkGlError("loadMesh");

	if(!StoneTexture.loadTexture((char*)"rockish.tga")) {
		LOGE("texture loading FAILED");
	}

	CubeSkyBox.loadSkyBox();

	if(!EnvironmentCubeTexture.loadCubeTexture())
		LOGE("Could not load cube texture.");

    return 0;
}

/**
 * Just the current frame in the display.
 */

static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        // No display.
        return;
    }

	ESMatrix perspective;
	ESMatrix modelview;
	float    aspect;
	ESMatrix lookAtMatrix;

	static float EyePos[4] = {0.0f,2.0f,3.2f,0.0f};
	static float LightPos[4] = {4.0f,0.0f,3.2f,0.0f};
	static int LightPosDirection = 1.0f;  //1 is up, -1 is down

	//do some movement of the light in the phong shader
	LightPos[1]+= 0.25f * LightPosDirection;
	if(LightPos[1]>20.0f ||  LightPos[1] < -20.0f)
		LightPosDirection*=-1;

	//here we go
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glClearDepthf(1.0f);
	glClear(  GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_FALSE);
	//set up the perspective and frustum, etc
	esMatrixLoadIdentity( &perspective );

	if(ContextHeight > ContextWidth) {
		aspect = ContextHeight / ContextWidth;
		esFrustum(&perspective, 1, -1.0f, -aspect, aspect, 0.1f, 50.0f);
	}
	else {
		aspect = ContextWidth/ContextHeight;
		esFrustum(&perspective, -aspect, aspect, 1, -1.0f, 0.1f, 50.0f);
	}

	//SKYBOX
	esMatrixLoadIdentity( &modelview );
	esMatrixLoadIdentity(&lookAtMatrix);
	esLookAt(&lookAtMatrix,0,0,0,0,0,1,0,1,0);

		esRotate(&modelview,-RotateY,0,1,0);
		esRotate(&modelview, -RotateX, 1, 0,0);
		esTranslate(&modelview, 0,0,-1);

		esMatrixMultiply(&modelview,&modelview,&lookAtMatrix);
		esMatrixMultiply( &MVPMatrix,&modelview,&perspective );

		glUseProgram(TexPassThruShader.ID);

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(TexPassThruSampler,0);

		glUniformMatrix4fv(TexPassThruMVPMatrixUniform, 1, GL_FALSE, (GLfloat*) &MVPMatrix.m[0][0] );

		//FRONT
			glBindTexture(GL_TEXTURE_2D,CubeSkyBox.Front.Tex.ID[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		 	//vert
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Front.VBOVertId);
			glEnableVertexAttribArray(TexPassThruPositionAttributes);
			glVertexAttribPointer(TexPassThruPositionAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//tex
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Front.VBOTexCoordId);
			glEnableVertexAttribArray(TexPassThruTexCoordAttributes);
			glVertexAttribPointer(TexPassThruTexCoordAttributes,2,GL_FLOAT,GL_FALSE,0,0);

			//index
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,CubeSkyBox.Front.VBOIndexId);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE,0);

		//LEFT
			glBindTexture(GL_TEXTURE_2D,CubeSkyBox.Left.Tex.ID[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//vert
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Left.VBOVertId);
			glEnableVertexAttribArray(TexPassThruPositionAttributes);
			glVertexAttribPointer(TexPassThruPositionAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//tex
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Left.VBOTexCoordId);
			glEnableVertexAttribArray(TexPassThruTexCoordAttributes);
			glVertexAttribPointer(TexPassThruTexCoordAttributes,2,GL_FLOAT,GL_FALSE,0,0);

			//index
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,CubeSkyBox.Left.VBOIndexId);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE,0);

		//RIGHT

			glBindTexture(GL_TEXTURE_2D,CubeSkyBox.Right.Tex.ID[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//vert
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Right.VBOVertId);
			glEnableVertexAttribArray(TexPassThruPositionAttributes);
			glVertexAttribPointer(TexPassThruPositionAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//tex
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Right.VBOTexCoordId);
			glEnableVertexAttribArray(TexPassThruTexCoordAttributes);
			glVertexAttribPointer(TexPassThruTexCoordAttributes,2,GL_FLOAT,GL_FALSE,0,0);

			//index
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,CubeSkyBox.Right.VBOIndexId);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE,0);

		//BACK

			glBindTexture(GL_TEXTURE_2D,CubeSkyBox.Back.Tex.ID[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//vert
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Back.VBOVertId);
			glEnableVertexAttribArray(TexPassThruPositionAttributes);
			glVertexAttribPointer(TexPassThruPositionAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//tex
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Back.VBOTexCoordId);
			glEnableVertexAttribArray(TexPassThruTexCoordAttributes);
			glVertexAttribPointer(TexPassThruTexCoordAttributes,2,GL_FLOAT,GL_FALSE,0,0);

			//index
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,CubeSkyBox.Back.VBOIndexId);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE,0);

	//BOTTOM

			glBindTexture(GL_TEXTURE_2D,CubeSkyBox.Bottom.Tex.ID[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//vert
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Bottom.VBOVertId);
			glEnableVertexAttribArray(TexPassThruPositionAttributes);
			glVertexAttribPointer(TexPassThruPositionAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//tex
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Bottom.VBOTexCoordId);
			glEnableVertexAttribArray(TexPassThruTexCoordAttributes);
			glVertexAttribPointer(TexPassThruTexCoordAttributes,2,GL_FLOAT,GL_FALSE,0,0);

			//index
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,CubeSkyBox.Bottom.VBOIndexId);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE,0);

	//TOP

			glBindTexture(GL_TEXTURE_2D,CubeSkyBox.Top.Tex.ID[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//vert
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Top.VBOVertId);
			glEnableVertexAttribArray(TexPassThruPositionAttributes);
			glVertexAttribPointer(TexPassThruPositionAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//tex
			glBindBuffer(GL_ARRAY_BUFFER,CubeSkyBox.Top.VBOTexCoordId);
			glEnableVertexAttribArray(TexPassThruTexCoordAttributes);
			glVertexAttribPointer(TexPassThruTexCoordAttributes,2,GL_FLOAT,GL_FALSE,0,0);

			//index
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,CubeSkyBox.Top.VBOIndexId);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE,0);


		glDepthMask(GL_TRUE);

	//MESH
	esMatrixLoadIdentity( &modelview );
	esMatrixLoadIdentity(&lookAtMatrix);
	esLookAt(&lookAtMatrix,0,0,0,0,0,1,0,1,0);
	esTranslate(&modelview, -StatueMesh.CenterX,-StatueMesh.CenterY,-StatueMesh.CenterZ);
	esRotate(&modelview,RotateY,0,1,0);
	esRotate(&modelview, RotateX, 1, 0,0);
	esScale(&modelview,ScaleAmount,ScaleAmount,ScaleAmount);
	esMatrixMultiply(&modelview,&modelview,&lookAtMatrix);
	esMatrixMultiply( &MVPMatrix,&modelview,&perspective );

	//turn on the shader
	switch(ShaderToUse) {

		case 0: //environment mapping shader

			glBindTexture(GL_TEXTURE_2D,0);

			glUseProgram(EnvironmentMappingShader.ID);
			checkGlError("glUseProgram");

			glActiveTexture(GL_TEXTURE1);

			glBindTexture(GL_TEXTURE_CUBE_MAP,EnvironmentCubeTexture.ID[0]);

			glUniform1i(EnvironmentMappingShaderCubeSampler,1);

			glUniformMatrix4fv( EnvironmentMappingShaderMVPMatrixUniform, 1, GL_FALSE, (GLfloat*) &MVPMatrix.m[0][0] );

			//Vertex Buffer
			glBindBuffer(GL_ARRAY_BUFFER,StatueMesh.VBOVertices.Id);
			glEnableVertexAttribArray(EnvironmentMappingShaderPositionAttributes);
			glVertexAttribPointer(EnvironmentMappingShaderPositionAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//Normals
			glBindBuffer(GL_ARRAY_BUFFER,StatueMesh.VBONormals.Id);
			glEnableVertexAttribArray(EnvironmentMappingShaderNormalAttributes);
			glVertexAttribPointer(EnvironmentMappingShaderNormalAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//Indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,StatueMesh.VBOIndices.Id);

			//draw call
			glDrawElements(GL_TRIANGLES, StatueMesh.IndexCount, GL_UNSIGNED_SHORT,0);

			glUseProgram(0);
			break;

		case 1: //phong
			glUseProgram(PhongShader.ID);
			checkGlError("glUseProgram");

			glActiveTexture(GL_TEXTURE0);

			glBindTexture(GL_TEXTURE_2D,StoneTexture.ID[0]);

			glUniform1i(TextureSampler,0);

			// Load the MVP matrix
			glUniformMatrix4fv( MVPMatrixUniform, 1, GL_FALSE, (GLfloat*) &MVPMatrix.m[0][0] );

			//Uniforms
			glUniform4fv( EyePosUniform, 1,  (GLfloat*) &EyePos );
			glUniform4fv( LightPosUniform, 1,  (GLfloat*) &LightPos );

			//Vertex Buffer
			glBindBuffer(GL_ARRAY_BUFFER,StatueMesh.VBOVertices.Id);
			glEnableVertexAttribArray(PositionAttributes);
			glVertexAttribPointer(PositionAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//Texture Coordinates
			glBindBuffer(GL_ARRAY_BUFFER,StatueMesh.VBOTexCoords.Id);
			glEnableVertexAttribArray(TexCoordAttributes);
			glVertexAttribPointer(TexCoordAttributes,2,GL_FLOAT,GL_FALSE,0,0);

			//Normals
			glBindBuffer(GL_ARRAY_BUFFER,StatueMesh.VBONormals.Id);
			glEnableVertexAttribArray(NormalAttributes);
			glVertexAttribPointer(NormalAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//Indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,StatueMesh.VBOIndices.Id);

			//draw call
			glDrawElements(GL_TRIANGLES, StatueMesh.IndexCount, GL_UNSIGNED_SHORT,0);
			break;

		case 2: //depth as color shader
			glUseProgram(DepthShader.ID);
			glUniformMatrix4fv( DepthShaderMVPMatrixUniform, 1, GL_FALSE, (GLfloat*) &MVPMatrix.m[0][0] );

			//Vertex Buffer
			glBindBuffer(GL_ARRAY_BUFFER,StatueMesh.VBOVertices.Id);
			glEnableVertexAttribArray(DepthShaderPositionAttributes);
			glVertexAttribPointer(DepthShaderPositionAttributes, 3, GL_FLOAT, GL_FALSE, 0, 0);
			//Indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,StatueMesh.VBOIndices.Id);

			//draw call
			glDrawElements(GL_TRIANGLES, StatueMesh.IndexCount, GL_UNSIGNED_SHORT,0);
			break;

		default:
			break;
	}

    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    //engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * INPUT HANDLING
 */

float X1;
float Y1;

float X2;
float Y2;
float S;

void touchTranslate(float dy)
{
	static float translation = -1.3f;

	esMatrixLoadIdentity( &translateMatrix );
	esTranslate(&translateMatrix,0,0,translation);
}

void touchRotate(float dx, float dy) {
	RotateX += 360.0f * dy / ContextHeight;
	RotateY += 360.0f * dx / ContextWidth;
}

void touchScale(float amount) {

	static float scaleMin = 0.25f;
	static float scaleMax = 5.0f;
	ScaleAmount -= amount / sqrtf(ContextWidth*ContextWidth + ContextHeight*ContextHeight);

	if(ScaleAmount < scaleMin)
		ScaleAmount = scaleMin;

	if(ScaleAmount > scaleMax)
		ScaleAmount = scaleMax;
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {

	struct engine* engine = (struct engine*)app->userData;

	int pointers = AMotionEvent_getPointerCount(event);

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

		int pointers = AMotionEvent_getPointerCount(event);

		switch(pointers) {

		case 1:
			//rotation
			if(AKeyEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN ) {

						X1 = AMotionEvent_getX(event, 0);
						Y1 = AMotionEvent_getY(event, 0);

						return 1;
					}
					else if(AKeyEvent_getAction(event) == AMOTION_EVENT_ACTION_UP) {
						// Do nothing
					}
					else if(AKeyEvent_getAction(event) == AMOTION_EVENT_ACTION_MOVE ) {

						float dx = AMotionEvent_getX(event, 0) - X1;
						float dy = AMotionEvent_getY(event, 0) - Y1;

						touchRotate(-dx,-dy);

						X1 = AMotionEvent_getX(event, 0);
						Y1 = AMotionEvent_getY(event, 0);
					}

			break;
		case 2:
			//pinch/zoom (multitouch)

			if(AKeyEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN ) {

				X1 = AMotionEvent_getX(event, 0);
				Y1 = AMotionEvent_getY(event, 0);
				X2 = AMotionEvent_getX(event, 1);
				Y2 = AMotionEvent_getY(event, 1);


				double dx = abs((double) (X2 - X1));
				double dy = abs((double) (Y2 - Y1));
				S = (float) sqrt((dx * dx) + (dy * dy));
			}

			else if(AKeyEvent_getAction(event) == AMOTION_EVENT_ACTION_MOVE )
			{
				float x1 = 0.0F;
				float y1 = 0.0F;
				float x2 = 0.0F;
				float y2 = 0.0F;

				// some unknown device throws an exception here
				x1 = AMotionEvent_getX(event, 0);
				y1 = AMotionEvent_getY(event, 0);
				x2 = AMotionEvent_getX(event, 1);
				y2 = AMotionEvent_getY(event, 1);

				double dx = abs((double) (x2 - x1));
				double dy = abs((double) (y2 - y1));
				float s = (float) sqrt((dx * dx) + (dy * dy));

				touchScale(4.0f * (S - s));

				//LOGI("S-s: %f",S-s);
				X1 = x1;
				Y1 = y1;
				X2 = x2;
				Y2 = y2;
				S = s;
			}
			break;

		case 3:
			//3 finger tap to switch shaders
			//LOGI("action: %i%",AKeyEvent_getAction(event));

			if(AKeyEvent_getAction(event) == AMOTION_EVENT_ACTION_UP || AKeyEvent_getAction(event) == AMOTION_EVENT_ACTION_POINTER_UP ) {
				LOGI("3 finger tap UP");
				ShaderToUse ++;
				if(ShaderToUse>2)
					ShaderToUse=0;
			}

		}
	}

    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {

	struct engine* engine = (struct engine*)app->userData;

    switch (cmd) {
        case APP_CMD_SAVE_STATE:

            // The system has asked us to save our current state.  Do so.
            engine->app->savedState = malloc(sizeof(struct saved_state));

            *((struct saved_state*)engine->app->savedState) = engine->state;

            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
                engine_init_display(engine);
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:

            break;
        case APP_CMD_LOST_FOCUS:

            engine_draw_frame(engine);
            break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */

void android_main(struct android_app* state) {
    struct engine engine;

    // Make sure glue isn't stripped.
    app_dummy();


    AndroidAssetManager::Inst(state->activity->assetManager);
    AndroidAssetManager::Inst()->openDir((char*)"");

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;


    if (state->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        engine.state = *(struct saved_state*)state->savedState;
    }

    // loop waiting for stuff to do.

    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(0, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        getDeltaTime();
        engine_draw_frame(&engine);

    }
}

