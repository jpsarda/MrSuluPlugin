#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod\plugin\bakkesmodplugin.h"

class CarWrapper;
class BallWrapper;

class MrSuluPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	shared_ptr<bool> enabled;
	shared_ptr<float> waitStartTimer;
	/*
	
	shared_ptr<float> limitThrottle;
	shared_ptr<float> limitYaw;
	shared_ptr<float> limitPitch;
	shared_ptr<float> limitRoll;
	shared_ptr<bool> disableHandbrake;
	shared_ptr<bool> disableJump;
	shared_ptr<bool> disableBoost;
	*/
	bool timerStarted;
	float timerStartTime;

	int timerHitBall, timerHitWorld, timerScore;

	Vector lastCarLocation;
	bool carIdle,timerReady;
	float carIdleStartTime;

	//float lastBallTouchTime;

	void enable();
	void timerStart();
	void timerIsReady();
	void timerDisplay(std::string category);

	bool IsCarReady();
	CarWrapper GetGameCar();
	BallWrapper GetGameBall();
	float GetSecondsElapsed();

	void log(std::string msg);

public:
	//MrSuluPlugin();
	//~MrSuluPlugin();
	virtual void onLoad();
	virtual void onUnload();

	void OnHitBall(std::string eventName);
	void OnHitWorld(std::string eventName);
	void OnBallHitGoal(std::string eventName);

	void OnPreAsync(std::string funcName);
	void OnWorldLoad(std::string eventName);
	void OnWorldDestroy(std::string eventName);
	void OnEnabledChanged(std::string oldValue, CVarWrapper cvar);
};

