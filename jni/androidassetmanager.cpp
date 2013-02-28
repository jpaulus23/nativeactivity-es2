#include "androidassetmanager.hpp"
#include <stdio.h>

#ifndef LOG_TAG
#define  LOG_TAG    "headdemo"
#endif

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

AndroidAssetManager* AndroidAssetManager::pInstance = NULL;

AndroidAssetManager* AndroidAssetManager::Inst(AAssetManager* assetm) {
	if(pInstance==NULL) {
		pInstance = new AndroidAssetManager(assetm);
	}
	return pInstance;
}

AndroidAssetManager* AndroidAssetManager::Inst() {

	return pInstance;
}

AndroidAssetManager::AndroidAssetManager(AAssetManager* assetm) {
	this->AssetManager = assetm;
}

void AndroidAssetManager::openDir(char* dirname) {

	this->AssetDir = AAssetManager_openDir(this->AssetManager, dirname);

}

AAsset* AndroidAssetManager::openAsset(char* filename) {

	return AAssetManager_open(AssetManager,filename, AASSET_MODE_RANDOM);

}

char* AndroidAssetManager::getLineFromAsset(char* buffer, int size, AAsset* asset) {

	// 'fgets' but for assets heh

	int index =0;
	char c;

	if(AAsset_read(asset,&c,1) ==0) {
		return NULL;
	}

	if(c=='\n')
		return (char*) "\0";

	while(c!= '\n') {
		buffer[index]=c;
		AAsset_read(asset,&c,1);
		index++;
	}

	if(c=='\n') {
		buffer[index]='\0';
	}

	return buffer;

}
