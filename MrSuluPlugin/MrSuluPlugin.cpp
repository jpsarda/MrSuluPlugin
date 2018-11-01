#include "MrSuluPlugin.h"
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <iostream>
#include <cstdio>
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

	/*
	limitSteer = make_shared<float>(1.f);
	limitThrottle = make_shared<float>(1.f);
	limitPitch = make_shared<float>(1.f);
	limitYaw = make_shared<float>(1.f);
	limitRoll = make_shared<float>(1.f);

	disableHandbrake = make_shared<bool>(false);
	disableJump = make_shared<bool>(false);
	disableBoost = make_shared<bool>(false);
	*/

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


	gameWrapper->HookEvent("Function TAGame.Car_TA.EventHitBall", bind(&MrSuluPlugin::OnHitBall, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.EventHitWorld", bind(&MrSuluPlugin::OnHitWorld, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", bind(&MrSuluPlugin::OnBallHitGoal, this, std::placeholders::_1));

	gameWrapper->HookEvent("Function TAGame.Car_TA.OnJumpPressed", bind(&MrSuluPlugin::OnJumpPressed, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.OnJumpReleased", bind(&MrSuluPlugin::OnJumpReleased, this, std::placeholders::_1));

	//not the right event ?
	//gameWrapper->HookEvent("Function CarComponent_TA.Active.BeginState", bind(&MrSuluPlugin::OnCarSpawn, this, std::placeholders::_1));

	

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
	for (size_t i = 0; i < canvasLogsMaxCount; i++)
	{
		canvasLogs[i] = "";
	}

	gameWrapper->RegisterDrawable([this](CanvasWrapper cw) {
		if (!canBeEnabled()) {
			return;
		}

		//if (cvarManager->getCvar("showHUD").getBoolValue())
		//{
			this->drawTimerPanel(cw); // , 1350, 50);
		//}
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
	//int index = canvasLogsIndex - (canvasLogsMaxCount - 1);
	//if (index < 0) index += canvasLogsMaxCount;

	int index = canvasLogsIndex;
	for (int i = 0; i < canvasLogsMaxCount; i++)
	{
		/*
		stringstream stream;
		stream << index;
		string s = stream.str();
		log( "index "+s );

		log( "drawStringAt  canvasLogs[index] " + canvasLogs[index]);
		*/
		drawStringAt(cw, canvasLogs[index], x + marginLeft, currentY);
		currentY += lineSpacing;

		index++;
		if (index >= canvasLogsMaxCount) index -= canvasLogsMaxCount;
	}
	

	/*

	drawStringAt(cw, "Car derived info", x + marginLeft, y + marginTop);

	this->drawStringAt(cw, "Car derived info", x + marginLeft, y + marginTop);

	int currentLine = marginTop + 50;

	this->drawStringAt(cw, "Linear speed", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(lin.magnitude(), 4), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Horizontal speed", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(horVel.magnitude(), 4), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Local lin. velocity", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(linLocal.X, 4), x + marginLeft + nameSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string(linLocal.Y, 4), x + marginLeft + nameSpacing + vecSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string(linLocal.Z, 4), x + marginLeft + nameSpacing + vecSpacing * 2, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Long. speed", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(lonSpeed, 4), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Lat. speed", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(latSpeed, 4), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Rotation fwd", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(fwd.X), x + marginLeft + nameSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(fwd.Y), x + marginLeft + nameSpacing + vecSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(fwd.Z), x + marginLeft + nameSpacing + vecSpacing * 2, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Rotation right", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(right.X), x + marginLeft + nameSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(right.Y), x + marginLeft + nameSpacing + vecSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(right.Z), x + marginLeft + nameSpacing + vecSpacing * 2, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Rotation up", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(up.X), x + marginLeft + nameSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(up.Y), x + marginLeft + nameSpacing + vecSpacing, y + currentLine);
	this->drawStringAt(cw, sp::to_string_scientific(up.Z), x + marginLeft + nameSpacing + vecSpacing * 2, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Left steer", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(wheels.Get(0).GetSteer2(), 6), x + marginLeft + nameSpacing, y + currentLine);

	currentLine += lineSpacing;

	this->drawStringAt(cw, "Right steer", x + marginLeft, y + currentLine);
	this->drawStringAt(cw, sp::to_string(wheels.Get(1).GetSteer2(), 6), x + marginLeft + nameSpacing, y + currentLine);
	*/
}

void MrSuluPlugin::canvasLog(std::string msg) {
	/*
	log("canvasLog " + msg);
	stringstream stream;
	stream << canvasLogsIndex;
	string s = stream.str();
	log( "canvasLogsIndex " + s);
	*/

	canvasLogs[canvasLogsIndex] = msg;
	canvasLogsIndex++;
	if (canvasLogsIndex >= canvasLogsMaxCount) canvasLogsIndex -= canvasLogsMaxCount;
	/*
	stringstream stream2;
	stream2 << canvasLogsIndex;
	string s2 = stream2.str();
	log( "canvasLogsIndex " + s2 );
	*/
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
	//log("hit ball");
	if (timerStarted) {
		if (timerHitBall < 1) { //display it once only
			timerDisplay("hit ball");
			timerHitBall++;
		}
	}
}
void MrSuluPlugin::OnHitWorld(std::string eventName) {
	//log("hit world");
	if (timerStarted) {
		if (timerHitWorld < 1) { //display it once only
			timerDisplay("hit world");
			timerHitWorld++;
		}
	}
}

void MrSuluPlugin::OnBallHitGoal(std::string eventName) {
	//log("hit goal");
	if (timerStarted) {
		if (timerScore < 1) { //display it once only
			timerDisplay("score");
			timerScore++;
		}
	}
}

void MrSuluPlugin::OnJumpPressed(std::string eventName) {
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

				timerDisplay("2nd jump +"+s0+"s, height="+s1);
			}
			
			timerJumpPressed++;
		}
	}
}

void MrSuluPlugin::OnJumpReleased(std::string eventName) {
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

			timerDisplay("1st jump hold duration "+s0+"s, height=" + s1);
			timerJumpReleased++;
		}
	}
}

/*
void MrSuluPlugin::OnCarSpawn(std::string eventName) {
	if (!IsCarReady()) {
		log("OnCarSpawn car is not ready!");
	}
	//force reset timer
	CarWrapper gameCar = GetGameCar();
	carIdle = true;
	timerIsReady();
}
*/

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

		// Check if car was iddle for more than 1 second
		if ((carLocation.X == lastCarLocation.X) && (carLocation.Y == lastCarLocation.Y) && (carLocation.Z == lastCarLocation.Z)) {
			// Check if
			if (!timerReady) {
				if (carIdle) {
					if (GetSecondsElapsed() - carIdleStartTime > *waitStartTimer) {
						timerIsReady();
					}
				}
				else {
					carIdle = true;
					carIdleStartTime = GetSecondsElapsed();
				}
			}
		}
		else {
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


	log("MrSulu enabled !!!");
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

	timerStartTime = GetSecondsElapsed();
	//BallWrapper ball = GetGameBall();
	//lastBallTouchTime = ball.GetLastTouchTime();

	log("MrSulu timer starts");
}

float MrSuluPlugin::timerDisplay(std::string category)
{
	float time = GetSecondsElapsed() - timerStartTime;

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
	if (*enabled) 
		this->enable();
}

void MrSuluPlugin::OnWorldDestroy(std::string eventName)
{
	log("OnWorldDestroy");
	//gameWrapper->UnhookEvent("Function TAGame.RBActor_TA.PreAsyncTick");
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");

}

void MrSuluPlugin::OnEnabledChanged(std::string oldValue, CVarWrapper cvar)
{
	if (cvar.getBoolValue() && canBeEnabled())
	{
		this->enable();
	}
	else
	{
		//gameWrapper->UnhookEvent("Function TAGame.RBActor_TA.PreAsyncTick");
		gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
	}
}

