#include "MrSuluPlugin.h"
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <iostream>
#include <cstdio>
#define _USE_MATH_DEFINES
#include <math.h>
#include "bakkesmod\wrappers\GameEvent\TutorialWrapper.h"
#include "bakkesmod\wrappers\GameEvent\ServerWrapper.h"
#include "bakkesmod\wrappers\GameObject\BallWrapper.h"
#include "bakkesmod\wrappers\GameObject\CarWrapper.h"
#include "bakkesmod\wrappers\GameObject\GoalWrapper.h"
#include "bakkesmod\wrappers\ArrayWrapper.h"
#include "bakkesmod\wrappers\WrapperStructs.h"
using namespace std::placeholders;

BAKKESMOD_PLUGIN(MrSuluPlugin, "MrSulu Plugin", "0.1", PLUGINTYPE_FREEPLAY | PLUGINTYPE_CUSTOM_TRAINING)

/*
template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
	return std::max(lower, std::min(n, upper));
}
*/

// Very complete plugin
// https://github.com/AratorRL/SciencePlugin/tree/master/SciencePlugin

FILE *stream;

void MrSuluPlugin::onLoad() {
	/*
	enabled = make_shared<bool>(false);

	//auto abc = gameWrapper->GetGameEventAsTutorial();

	//gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.OnInit", std::bind(&MrSuluPlugin::OnWorldLoad, this, std::placeholders::_1));

	
	gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.OnInit",  std::bind(&MrSuluPlugin::OnWorldLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.Destroyed",  std::bind(&MrSuluPlugin::OnWorldDestroy, this, std::placeholders::_1));
	
	cvarManager->registerCvar("mrsulu_enabled", "0", "Enables/disable plugin", true, true, 0.f, true, 1.f)
		.addOnValueChanged(std::bind(&MrSuluPlugin::OnEnabledChanged, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->getCvar("mrsulu_enabled").bindTo(enabled);
	
	*/

	//freopen_s(&stream, "mrsulu.log", "w", stdout);
	//freopen_s(&stream, "mrsulu.log", "w", stderr);

	enabled = make_shared<bool>(false);
	waitStartTimer = make_shared<float>(1.f);
	fastAerials = make_shared<bool>(false);

	carJustReset = 10;

	gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.OnInit", bind(&MrSuluPlugin::OnWorldLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.Destroyed", bind(&MrSuluPlugin::OnWorldDestroy, this, std::placeholders::_1));


	//Wrong event name, doesn't work, trying to get custom training events

	/*
	To find new events in the game
	Add - debug launch option to rocket league (through steam).
	Create file debug.txt (doesn't matter what's in it) in the bakkesmod folder.
	Then launch the game (you will probably lagg with this on) and make sure the event you want gets fired.
	When that's done execute "debug_dumpevents" in the console (F6).
	And it will be written to a file called allevents.txt in the win32 folder which contain all executed functions + their call amount.
	*/

	//Function TAGame.GameEvent_TrainingEditor_TA.Active.BeginState
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.OnInit", bind(&MrSuluPlugin::OnWorldLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.Destroyed", bind(&MrSuluPlugin::OnWorldDestroy, this, std::placeholders::_1));



	cvarManager->registerCvar("mrsulu_enabled", "0", "Enables/disable MrSulu", true, true, 0.f, true, 1.f)
		.addOnValueChanged(std::bind(&MrSuluPlugin::OnEnabledChanged, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->getCvar("mrsulu_enabled").bindTo(enabled);

	cvarManager->registerCvar("mrsulu_wait_start_timer", "0.25", "Wait duration to restart timer", true, true, 0.1f, true, 2.f).bindTo(waitStartTimer);

	cvarManager->registerCvar("mrsulu_fastaerials", "0", "Enables/disable fast aerials info", true, true, 0.f, true, 1.f).bindTo(fastAerials);

	/* moved to enter world event
	gameWrapper->HookEvent("Function TAGame.Car_TA.EventHitBall", bind(&MrSuluPlugin::OnHitBall, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.EventHitWorld", bind(&MrSuluPlugin::OnHitWorld, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", bind(&MrSuluPlugin::OnBallHitGoal, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.OnJumpPressed", bind(&MrSuluPlugin::OnJumpPressed, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.OnJumpReleased", bind(&MrSuluPlugin::OnJumpReleased, this, std::placeholders::_1));

	//not the right event ?
	gameWrapper->HookEvent("Function CarComponent_TA.Active.BeginState", bind(&MrSuluPlugin::OnCarSpawn, this, std::placeholders::_1));
	*/
	

	/*events
	Function TAGame.Ball_TA.OnCarTouch
	Function TAGame.Ball_TA.OnHitGoal
	Function TAGame.Ball_TA.EventCarTouch
	Function TAGame.Ball_TA.EventHitGoal

	Function TAGame.Car_TA.EventHitBall
	Function TAGame.Car_TA.EventHitWorld
	Function TAGame.Car_TA.EventLanded
	Function TAGame.Car_TA.OnHitBall
	Function TAGame.Car_TA.OnJumpPressed
	Function TAGame.Car_TA.OnJumpReleased
	*/
	/*
	cvarManager->registerCvar("mech_steer_limit", "1", "Clamps steer", true, true, 0.f, true, 1.f).bindTo(limitSteer);
	cvarManager->registerCvar("mech_throttle_limit", "1", "Clamps throttle", true, true, 0.f, true, 1.f).bindTo(limitThrottle);
	cvarManager->registerCvar("mech_yaw_limit", "1", "Clamps yaw", true, true, 0.f, true, 1.f).bindTo(limitYaw);
	cvarManager->registerCvar("mech_pitch_limit", "1", "Clamps pitch", true, true, 0.f, true, 1.f).bindTo(limitYaw);
	cvarManager->registerCvar("mech_roll_limit", "1", "Clamps roll", true, true, 0.f, true, 1.f).bindTo(limitRoll);

	cvarManager->registerCvar("mech_disable_handbrake", "0", "Disables handbrake", true, true, 0.f, true, 1.f).bindTo(disableHandbrake);
	cvarManager->registerCvar("mech_disable_jump", "0", "Disables jump", true, true, 0.f, true, 1.f).bindTo(disableJump);
	cvarManager->registerCvar("mech_disable_boost", "0", "Disables boost", true, true, 0.f, true, 1.f).bindTo(disableBoost);
	*/


	//drawables
	canvasLogsMaxCount = (sizeof(canvasLogs) / sizeof(*canvasLogs));
	canvasLogsIndex=0;
	canvasLogsActiveTimerIndex = 0;
	for (size_t i = 0; i < canvasLogsMaxCount; i++)
	{
		canvasLogs[i] = "";
	}

	gameWrapper->RegisterDrawable([this](CanvasWrapper cw) {
		if (!canBeEnabled()) {
			return;
		}

		if (*enabled) {
			this->drawTimerPanel(cw); // , 1350, 50);
		}
	});
}

void MrSuluPlugin::onUnload() {

}

void MrSuluPlugin::drawStringAt(CanvasWrapper cw, std::string text, int x, int y, Color col)
{
	cw.SetPosition({ x, y });
	cw.SetColor(col.r, col.g, col.b, col.a);
	cw.DrawString(text);
}

void MrSuluPlugin::drawStringAt(CanvasWrapper cw, std::string text, Vector2 loc, Color col)
{
	drawStringAt(cw, text, loc.X, loc.Y, col);
}

void MrSuluPlugin::drawTimerPanel(CanvasWrapper cw) //, int x, int y)
{
	Vector2 size = cw.GetSize();

	int marginLeft = 10;
	int marginTop = 10;

	int width = 400;
	int height = 300;

	int lineSpacing = 20;

	int panelMargin = 10;
	int x = size.X - panelMargin - width;
	int y = panelMargin;

	cw.SetPosition({ x, y });
	cw.SetColor(COLOR_PANEL);
	cw.FillBox({ width, height });
	cw.SetColor(COLOR_TEXT);

	int currentY = y + marginTop;

	if (timerReady) {
		drawStringAt(cw, "0.000 seconds", x + marginLeft, currentY, { COLOR_TIMER_RESET });
		currentY += lineSpacing;
	} else if (timerStarted) {
		float time = GetSecondsElapsed() - timerStartTime;

		stringstream stream;
		stream << fixed << setprecision(3) << time;
		string s = stream.str();

		drawStringAt(cw, s+" seconds", x + marginLeft, currentY, { COLOR_TIMER });

		currentY += lineSpacing;
	}
	else {
		currentY += lineSpacing;
	}

	currentY += lineSpacing;

	//browse and display canvasLogs

	int index = canvasLogsIndex;

	Color textColor = { COLOR_TEXT_UNACTIVE };
	bool activeReached = false;
	if (index == canvasLogsActiveTimerIndex) activeReached = true; 
	for (int i = 0; i < canvasLogsMaxCount; i++)
	{
		if (!activeReached) {
			if (index == canvasLogsActiveTimerIndex) {
				activeReached = true;
				textColor = { COLOR_TEXT };
			}
		}
		drawStringAt(cw, canvasLogs[index], x + marginLeft, currentY, textColor);
		currentY += lineSpacing;

		index++;
		if (index >= canvasLogsMaxCount) index -= canvasLogsMaxCount;
	}
}

void MrSuluPlugin::canvasLog(std::string msg) {
	canvasLogs[canvasLogsIndex] = msg;
	canvasLogsIndex++;
	if (canvasLogsIndex >= canvasLogsMaxCount) canvasLogsIndex -= canvasLogsMaxCount;
}


BallWrapper MrSuluPlugin::GetGameBall() {
	TutorialWrapper training = gameWrapper->GetGameEventAsTutorial();
	if (training.IsNull()) {
		ServerWrapper server = gameWrapper->GetGameEventAsServer();
		return server.GetBall();
	}
	return training.GetBall();
}

bool MrSuluPlugin::IsCarReady() {
	TutorialWrapper training = gameWrapper->GetGameEventAsTutorial();
	if (training.IsNull()) {
		ServerWrapper server = gameWrapper->GetGameEventAsServer();
		auto players = gameWrapper->GetGameEventAsServer().GetCars();
		return (players.Count() >= 1);
	}
	return !training.GetGameCar().IsNull();
}

CarWrapper MrSuluPlugin::GetGameCar() {
	TutorialWrapper training = gameWrapper->GetGameEventAsTutorial();
	if (training.IsNull()) {
		ServerWrapper server = gameWrapper->GetGameEventAsServer();
		auto players = gameWrapper->GetGameEventAsServer().GetCars();
		if (players.Count() >= 1) {
			return players.Get(0);
		}
	}
	return training.GetGameCar();
}

float MrSuluPlugin::GetSecondsElapsed() {
	TutorialWrapper training = gameWrapper->GetGameEventAsTutorial();
	if (training.IsNull()) {
		ServerWrapper server = gameWrapper->GetGameEventAsServer();
		return server.GetSecondsElapsed();
	}
	
	return training.GetSecondsElapsed();
}

void MrSuluPlugin::OnHitBall(std::string eventName) {
	if (!canBeEnabled()) return;
	if (!*enabled) return;
	//log("hit ball");
	if (timerStarted) {
		if (timerHitBall < 1) { //display it once only
			timerDisplay("hit ball");
			timerHitBall++;
		}
	}
}
void MrSuluPlugin::OnHitWorld(std::string eventName) {
	if (!canBeEnabled()) return;
	if (!*enabled) return;
	//log("hit world");
	if (timerStarted) {
		if (timerHitWorld < 1) { //display it once only
			timerDisplay("hit world");
			timerHitWorld++;
		}
	}
}

void MrSuluPlugin::OnBallHitGoal(std::string eventName) {
	if (!canBeEnabled()) return;
	if (!*enabled) return;
	//log("hit goal");
	if (timerStarted) {
		if (timerScore < 1) { //display it once only
			timerDisplay("score");
			timerScore++;
		}
	}
}

void MrSuluPlugin::OnJumpPressed(std::string eventName) {
	if (!canBeEnabled()) return;
	if (!*enabled) return;
	//log("jump press");
	if (!timerStarted) { //start timer if not already done by car moving detection
		timerIsReady();
		timerStart();
	}
	if (timerStarted && *fastAerials) {
		if (timerJumpPressed < 2) { //display it twice only
			jumpPressedTime = GetSecondsElapsed();
			if (timerJumpPressed == 0) {
				timerDisplay("1st jump");
			}
			else {
				float duration = jumpPressedTime - jumpReleaseTime;

				stringstream stream0;
				stream0 << fixed << setprecision(3) << duration;
				string s0 = stream0.str();


				stringstream stream1;
				stream1 << fixed << setprecision(2) << GetGameCar().GetLocation().Z;
				string s1 = stream1.str();

				timerDisplay("2nd jump +"+s0+"s, Z="+s1);
			}
			
			timerJumpPressed++;
		}
	}
}

void MrSuluPlugin::OnJumpReleased(std::string eventName) {
	if (!canBeEnabled()) return;
	if (!*enabled) return;
	//log("jump release");
	if (timerStarted && *fastAerials) {
		if (timerJumpReleased < 1) { //display it once only

			jumpReleaseTime = GetSecondsElapsed();
			float duration = jumpReleaseTime - jumpPressedTime;

			stringstream stream0;
			stream0 << fixed << setprecision(3) << duration;
			string s0 = stream0.str();


			stringstream stream1;
			stream1 << fixed << setprecision(2) << GetGameCar().GetLocation().Z;
			string s1 = stream1.str();

			timerDisplay("1st jump hold duration "+s0+"s, Z=" + s1);
			timerJumpReleased++;
		}
	}
}


void MrSuluPlugin::OnCarSpawn(std::string eventName) {
	//log("OnCarSpawn");
	carJustReset = 10;
	/*
	if (!IsCarReady()) {
		log("OnCarSpawn car is not ready.");
		return;
	}
	if (gameWrapper->IsInReplay()) {
		log("OnCarSpawn car is in replay.");
		return;
	}
	log("OnCarSpawn");

	//force reset timer
	CarWrapper gameCar = GetGameCar();
	lastCarLocation = gameCar.GetLocation();
	carIdle = true;
	timerIsReady();
	*/
}

void MrSuluPlugin::OnFireBall(std::string eventName)
{
	timerIsReady();
	timerStart();
	timerDisplay("fire ball");
}


void MrSuluPlugin::OnTick(std::string funcName)
{
	if (canBeEnabled())
	{
		TutorialWrapper training = gameWrapper->GetGameEventAsTutorial();

		ServerWrapper server = gameWrapper->GetGameEventAsServer();

		if (server.IsNull()) return; //or crash

		if (!IsCarReady()) return;

		CarWrapper gameCar = GetGameCar();

		if (gameCar.IsNull()) return; //or crash

		BallWrapper ball = GetGameBall();

		if (ball.IsNull()) return; //or  ball.GetLastTouchTime(); will crash

		Vector carLocation = gameCar.GetLocation();

		//float ballTouchTime = ball.GetLastTouchTime();

		/*
		if (carJustReset) {
			if (!gameWrapper->IsInReplay()) {
				lastCarLocation = carLocation;
				carIdle = true;
				timerIsReady();
			}
			carJustReset = false;
		}
		*/
		
		//if (carJustReset > 1) carJustReset--;
		//if (carJustReset <= 1) { //don't do anything for 10 ticks after car reset

		// Check if car was iddle for more than X seconds
		if (fabsf(carLocation.X - lastCarLocation.X) + fabsf(carLocation.Y - lastCarLocation.Y) + fabsf(carLocation.Z - lastCarLocation.Z) < 0.1f) {
			// Check if
			if (!timerReady) {
				if (carIdle) {
					//if idle for more than x second or idle after just being reset
					float waitDuration = *waitStartTimer;
					if (carJustReset > 0) waitDuration = 0.1f;
					if (GetSecondsElapsed() - carIdleStartTime > waitDuration) {
						/*
						stringstream stream;
						stream << "carJustReset ";
						stream << carJustReset;
						log(stream.str());
						*/
						timerIsReady();
						carJustReset = 0;
					}
				}
				else {
					carIdle = true;
					carIdleStartTime = GetSecondsElapsed();
				}
			}
		}
		else {
			/*
			stringstream stream;
			stream << "Xdiff ";
			stream << fixed << setprecision(10) << (carLocation.X - lastCarLocation.X);
			stream << " Ydiff ";
			stream << fixed << setprecision(10) << (carLocation.Y - lastCarLocation.Y);
			stream << " Zdiff ";
			stream << fixed << setprecision(10) << (carLocation.Z - lastCarLocation.Z);
			string s = stream.str();
			log(s);
			*/
			carIdle = false;
		}

		if (timerStarted) {
			/*
			int collide = gameCar.GetLastHitBallFrame();

			stringstream stream;
			stream << collide;
			string s = stream.str();
			log( "MrSulu collides " + s );

			if (gameCar.IsOnGround() || gameCar.IsOnWall()) //player has landed
			{
				timerStop();
			}
			*/

			//if ball touched
			/* used an event instead
			if (ballTouchTime != lastBallTouchTime) {
				if (timerHitBall < 1) { //display it once only
					timerDisplay("hit ball");
					timerHitBall++;
				}
			}
			*/
			if (*fastAerials) {
				float timeElpased = GetSecondsElapsed() - timerStartTime;
				if (timerFastAerialBilan0 == 0) {
					if (timeElpased >= timerFastAerialBilan0Time) {
						stringstream stream1;
						stream1 << fixed << setprecision(2) << GetGameCar().GetLocation().Z;
						string s1 = stream1.str();
						timerDisplay("Z=" + s1, timerFastAerialBilan0Time);
						timerFastAerialBilan0++;
					}
				}
				if (timerFastAerialBilan1 == 0) {
					if (timeElpased >= timerFastAerialBilan1Time) {
						stringstream stream1;
						stream1 << fixed << setprecision(2) << GetGameCar().GetLocation().Z;
						string s1 = stream1.str();
						timerDisplay("Z=" + s1, timerFastAerialBilan1Time);
						timerFastAerialBilan1++;
					}
				}
			}

			//if scored
			if (!training.IsNull()) { //how to do it without training ?

				/*
				stringstream stream;
				stream << server.IsNull();
				string s = stream.str();
				log("server is null ? " + s);
				*/

				/*
				if (training.IsInGoal(ball.GetLocation()))
				{
					if (timerScore < 1) { //display it once only
						timerDisplay("score");
						timerScore++;
					}
				}
				*/
			}
			else {
				/*
				stringstream stream;
				//stream << server.GetTotalScore(); //always 0 in custom training
				auto goals = server.GetGoals();
				stream << goals.Count();
				string s = stream.str();
				log("total goals " + s);

				GoalWrapper goal = goals.Get(0);
				*/
			}

		}
		else {
			if (timerReady) {
				if (!carIdle) {
					timerStart(); //the timer is ready and the car is not idle anymore, we start the timer
				}
			} 
		}
		//}
		lastCarLocation = carLocation;
		//lastBallTouchTime = ballTouchTime;
	}
}

void MrSuluPlugin::log(std::string msg) {
	//cout << msg << endl;  
	cvarManager->log(msg); // goes to bakkesmod.log
}

bool MrSuluPlugin::canBeEnabled() {
	return ((gameWrapper->IsInTutorial()) || (gameWrapper->IsInCustomTraining()));
}

void MrSuluPlugin::enable()
{
	timerStarted = false;
	lastCarLocation.X = lastCarLocation.Y = lastCarLocation.Z = 0;
	carIdle = false;
	timerReady = false;
	// this tick depends on the FPS and can vary for differetn people
	//gameWrapper->HookEvent("Function TAGame.RBActor_TA.PreAsyncTick", bind(&MrSuluPlugin::OnTick, this, _1));

	//this tick is always 120/s
	gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", bind(&MrSuluPlugin::OnTick, this, _1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.EventHitBall", bind(&MrSuluPlugin::OnHitBall, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.EventHitWorld", bind(&MrSuluPlugin::OnHitWorld, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", bind(&MrSuluPlugin::OnBallHitGoal, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.OnJumpPressed", bind(&MrSuluPlugin::OnJumpPressed, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.OnJumpReleased", bind(&MrSuluPlugin::OnJumpReleased, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.PostBeginPlay", bind(&MrSuluPlugin::OnCarSpawn, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_GameEditor_TA.FireBalls", bind(&MrSuluPlugin::OnFireBall, this, std::placeholders::_1));

	log("Warp speed MrSulu !!!");
}

void MrSuluPlugin::disable()
{
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.EventHitBall");
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.EventHitWorld");
	gameWrapper->UnhookEvent("Function TAGame.Ball_TA.OnHitGoal");
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.OnJumpPressed");
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.OnJumpReleased");
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.PostBeginPlay");
	gameWrapper->UnhookEvent("Function TAGame.GameEvent_GameEditor_TA.FireBalls");
}

float MrSuluPlugin::getCarTilt() {
	/*
	RBState rbstate = GetGameCar().GetRBState();

	Vector loc = rbstate.Location;
	Vector lin = rbstate.LinearVelocity;
	FQuat quat = rbstate.Quaternion;
	Vector ang = rbstate.AngularVelocity;

	auto horVel = Vector(lin.X, lin.Y, 0);

	Vector up = quatToUp(quat);
	Vector right = quatToRight(quat);
	Vector fwd = quatToFwd(quat);

	return  asinf(fwd.Z) * 180.0f / M_PI;
	*/
	return 0;
}

void MrSuluPlugin::timerIsReady()
{
	timerReady = true;
	timerStarted = false;
	log("MrSulu timer ready");
}

void MrSuluPlugin::timerStart()
{
	timerStarted = true;
	timerReady = false;
	carIdle = false;
	carIdleStartTime = GetSecondsElapsed();
	timerHitBall = timerHitWorld = timerScore = timerJumpPressed = timerJumpReleased = 0;
	timerFastAerialBilan0 = timerFastAerialBilan1 = 0;
	timerFastAerialBilan0Time = 0.6f;
	timerFastAerialBilan1Time = 1.2f;

	timerStartTime = GetSecondsElapsed();
	//BallWrapper ball = GetGameBall();
	//lastBallTouchTime = ball.GetLastTouchTime();

	canvasLogsActiveTimerIndex = canvasLogsIndex;

	log("MrSulu timer starts");
}

float MrSuluPlugin::timerDisplay(std::string category, float timeElapsed)
{
	float time = GetSecondsElapsed() - timerStartTime;
	if (timeElapsed >= 0) time = timeElapsed;

	stringstream stream;
	stream << fixed << setprecision(3) << time;
	string s = stream.str();
	log("MrSulu timer "+category+" : "+s+"s");
	canvasLog(s + "s (" + category + ")");

	//gameWrapper->LogToChatbox("YO"); this line prevent the plugin from loading
	return time;
}

void MrSuluPlugin::OnWorldLoad(std::string eventName)
{
	log("OnWorldLoad");
	if (*enabled) enable();
}

void MrSuluPlugin::OnWorldDestroy(std::string eventName)
{
	log("OnWorldDestroy");
	disable();

}

void MrSuluPlugin::OnEnabledChanged(std::string oldValue, CVarWrapper cvar)
{
	if (cvar.getBoolValue() && canBeEnabled())
	{
		enable();
	}
	else
	{
		disable();
	}
}

