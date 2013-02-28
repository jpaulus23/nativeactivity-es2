#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <jni.h>
#include <errno.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>


class AndroidAssetManager {

public:
	static AndroidAssetManager* Inst(AAssetManager* assetm);
	static AndroidAssetManager* Inst();

	AAssetManager* AssetManager; //used to read files from the APK
	AAssetDir* AssetDir;

	void openDir(char* dirname);
	AAsset* openAsset(char* filename);

	char* getLineFromAsset(char* buffer, int size, AAsset* asset);

protected:
	AndroidAssetManager(AAssetManager* assetm);

private:
	static AndroidAssetManager* pInstance;

};


#endif
