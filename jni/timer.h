#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <time.h>

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static double Timer_DeltaTime = 0.0f;
static double Timer_Fps = 0.0f;

static const int TimeHistorySize =10;
static double TimeHistory[TimeHistorySize];
static int TimeHistoryIndex=0;
static double TimeSum=0;
double oldTime=0.0f;

static double getDeltaTime(void)
{
	struct timeval t;

	const static double usec = 1.0/1000000.0;

	gettimeofday(&t, NULL);

	//LOGI("tv_sec: %f",t.tv_sec);
	//LOGI("tv_usec: %f \n",usec * t.tv_usec);

	double rawTime = (double)t.tv_sec + (double)(usec * t.tv_usec);
	double deltaTime = rawTime - oldTime;

	oldTime = rawTime;

	if(rawTime <0)
		rawTime = 0;

	TimeHistory[TimeHistoryIndex++] = deltaTime;

	//LOGI("time: %f", rawTime);

	if(TimeHistoryIndex>=10)
	{
		TimeHistoryIndex=0;
	}

	TimeSum = 0;

	for(int i =0; i < TimeHistorySize; i++) 
	{
		TimeSum += TimeHistory[i];
	}

	//LOGI("timeSum: %f", TimeSum);

	Timer_DeltaTime = TimeSum / TimeHistorySize;
	Timer_Fps = 1.0/Timer_DeltaTime;

	return (Timer_DeltaTime);
}


static double getFPS(void)
{

	if(Timer_DeltaTime != 0.0)
	{
		return 1.0/Timer_DeltaTime;
	}
	else return 0.0;

}

#endif
