#include "AutoFlight.h"
#include <wiringPi.h>
#include "PID.h"
#include "../Sensor/AbstractSensor/Ultrasonic.h"
#include "../Sensor/AbstractSensor/GPS.h"
#include "../Sensor/HMC5883L.h"
#include "WayPoint.h"
#include "chrono"
#include <thread>
#include <iostream>
#include <math.h>
using namespace std::chrono;
using namespace std;

#define PI 3.14159265

int status = 0;

//Obeject-Detection --> TODO: needs to be finished
static void runObjectDetection(PID* pid, Ultrasonic* ultrasonic) {
	while (1) {
		int distance = ultrasonic->getDistance();
		if (distance < 150) {
			status = 1337;
			//Stop the drone immediately
			pid->setPitchSetpoint(1500);
			pid->landDrone();
		}
		delay(20);
	}
}

AutoFlight::AutoFlight(PID* p, Ultrasonic* u, GPS* g, HMC5883L *m) {
	pid = p;
	ultrasonic = u;
	gps = g;
	compass = m;
	pid->setAutoFlight(this);
	compassValue = compass->compassValue;

	double curYaw = pid->getOrientatin()->getPitchRoll()[2];
	this->correctionDegree = 0.0; //TODO: Calculate
}

void AutoFlight::start() {
	//run = true;
	doAutoFlight();
}

void AutoFlight::stop() {
	run = false;
}

void AutoFlight::doAutoFlight() {
	int curWaypoint = 0;
	double curDegrees = 0;

	thread objectDetectionThread(runObjectDetection, pid, ultrasonic);

	while (run) {
		if (status == 1337) {
			run = false;
			pid->setPitchSetpoint(1500);
			break;
		}

		double* curGPS = gps->getGPSValues();
		WayPoint* curWP = waypoints->at(curWaypoint);
		
		double diffLat = curWP->getLatitude() - curGPS[0];
		double diffLong = curWP->getLongitude() - curGPS[1];

		if (diffLat < 0.000025 && diffLat > -0.000025 && diffLong < 0.000025 && diffLong > -0.000025) {
			curWaypoint++;
			curGPS = gps->getGPSValues();
			curWP = waypoints->at(curWaypoint);
		}

		double degrees = calcDegrees(curWP) - correctionDegree; //TODO: Check if this is correct
		double degreeDiff = degrees - curDegrees;
		if (degreeDiff < 5 && degreeDiff > -5) {
			pid->setPitchSetpoint_Degree(20);
		}
		else {
			//-180° to 180°
			if (degrees > 180) {
				double newDegrees = degrees - 360;
				pid->setYawSetpoint_Degree(newDegrees);
			}
			else {
				pid->setYawSetpoint_Degree(degrees);
			}
		}
		delay(500);
	}

	objectDetectionThread.join();

}

void AutoFlight::setWaypoints(string points) {
	std::string delimiter = ":";
	size_t pos = 0;
	std::string token;
	double curVal;
	
	int counter = 0;
	double curLat = 0.0;
	double curLong = 0.0;
	
	while ((pos = points.find(delimiter)) != std::string::npos) {
		token = points.substr(0, pos);
		curVal = stod(token);

		if (counter != 0) {
			if (counter % 2 == 1) {
				curLat = curVal;
			} 
			else {
				curLong = curVal;
				WayPoint *w = new WayPoint();
				w->setLatitude(curLat);
				w->setLongitude(curLong);

				waypoints->push_back(w);
			}
		}

		points.erase(0, pos + delimiter.length());
		counter++;
	}
}

void AutoFlight::setPitchDegree(float degree) {
	pid->setPitchSetpoint_Degree(degree);
}

void AutoFlight::setRollDegree(float degree) {
	pid->setRollSetpoint_Degree(degree);
}

void AutoFlight::setYawDegree(float degree) {
	pid->setYawSetpoint_Degree(degree);
}

double AutoFlight::calcDegrees(WayPoint* curWaypoint) {
	double* curP = gps->getGPSValues();

	double length = curWaypoint->getLatitude() - curP[0];
	double height = curWaypoint->getLongitude() - curP[1];

	std::cout << "LengthDiff: " << length << " HeightDiff: " << height << std::endl;

	double degrees = 0;
	if (length == 0 && height == 0) {
		std::cout << "Both same" << std::endl;
		degrees = 0;
	}
	else if (height == 0 && length > 0) {
		degrees = 0;
	}
	else if (height == 0 && length < 0) {
		degrees = 180;
	}
	else if (length == 0 && height > 0) {
		degrees = 90;
	}
	else if (length == 0 && height < 0) {
		degrees = 270;
	}

	if (length > 0 && height > 0)
	{
		//upper right (0-90)
		std::cout << "Upper Right" << std::endl;
		degrees = atan(height / length) * (180 / PI);
	}
	else if (length < 0 && height > 0) {
		//lower right (90-180)
		std::cout << "Lower Right" << std::endl;
		height *= -1;
		degrees = 90 + (atan(height / length) * (180 / PI));
	}
	else if (height < 0 && length < 0) {
		//lower left (180-270)
		std::cout << "Lower Left" << std::endl;
		height *= -1;
		length *= -1;
		degrees = 180 + (atan(height / length) * (180 / PI));
	}
	else if (height < 0 && length > 0) {
		//upper left (270-360)
		std::cout << "Upper Left" << std::endl;
		length *= -1;
		degrees = 270 + (atan(height / length) * (180 / PI));
	}

	std::cout << "Degrees: " << degrees << std::endl;
}

AutoFlight::~AutoFlight() {
	
}


/*

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
	pid->setPitchSetpoint(1500);
}

*/