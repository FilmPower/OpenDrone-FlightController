#include "AutoFlight.h"
#include <wiringPi.h>
#include "PID.h"
#include "../Sensor/AbstractSensor/Ultrasonic.h"
#include "chrono"
#include <thread>
using namespace std::chrono;
using namespace std;

static void runObjectDetection(PID* pid, Ultrasonic *ultrasonic) {
	while (1) {
		int distance = ultrasonic->getDistance();
		if (distance < 250) {
			status = 1337;
			//Stop the drone immediately
			pid->setPitchSetpoint(1500);

			//TODO: Find another way
		}
		delay(20);
	}
}


int status = 0;

AutoFlight::AutoFlight(PID* p, Ultrasonic* u) {
	pid = p;
	ultrasonic = u;
	pid->setAutoFlight(this);
	
	thread objectDetectionThread(runObjectDetection, pid, ultrasonic);
	
	doAutoFlight();
}

void AutoFlight::start() {
	run = true;
}

void AutoFlight::stop() {
	run = false;
}

void AutoFlight::doAutoFlight() {
	while (run) {
		switch (status) 
		{
			//0: Do nothing
			case 0: return;
			//1: Test AutoFlight
			case 1: testAutoFlight(); return;
			//TODO: Other cases
		}
		delay(5);
	}
}

void AutoFlight::testAutoFlight() {
	//Fly forward for 3 seconds, stop after that
	int64_t startTime = system_clock::now().time_since_epoch().count();
	int timeDiff = 0;
	while (timeDiff < 3000) {
		if (status != 1337)
			pid->setPitchSetpoint(1600);
		else
			break;
		int64_t curTime = system_clock::now().time_since_epoch().count();
		timeDiff = curTime - startTime;
	}

	//Stop
	pid->setPitchSetpoint(1550);
	delay(25);
	pid->setPitchSetpoint(1500);
}

void AutoFlight::startObjectDetection() {

}

AutoFlight::~AutoFlight() {
	
}