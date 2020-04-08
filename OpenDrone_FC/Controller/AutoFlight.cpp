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
bool runObject = true;

static void runObjectDetection(PID* pid, Ultrasonic* ultrasonic) {
	while (runObject) {
		int distance = ultrasonic->getDistance();
		if (distance < 150 && distance > 20) {
			status = 1337;
			//Stop the drone immediately
			pid->setPitchSetpoint_Degree(0);
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
}

void AutoFlight::start() {
	//run = true;
	doAutoFlight();
}

void AutoFlight::stop() {
	run = false;
}

void AutoFlight::startFlying() {
	run = true;
}

void AutoFlight::doAutoFlight() {
	int curWaypoint = 0;
	double curDegrees = 0;

	thread objectDetectionThread(runObjectDetection, pid, ultrasonic);

	while (!run) { delay(100); }

	while (run) {
		if (status == 1337) {
			run = false;
			pid->setPitchSetpoint(1500);
			break;
		}

		double* curGPS = gps->getGPSValues();
		WayPoint* curWP;
		if (waypoints->size() < curWaypoint + 1) {
			curWP = waypoints->at(curWaypoint);
		}
		else {
			pid->landDrone();
			run = false;
			break;
		}
		
		double diffLat = curWP->getLatitude() - curGPS[0];
		double diffLong = curWP->getLongitude() - curGPS[1];

		if (diffLat < 0.000025 && diffLat > -0.000025 && diffLong < 0.000025 && diffLong > -0.000025) {
			curWaypoint++;
			continue;
		}

		double degrees = calcDegrees(curWP);
		double newDegrees = degrees;
		//-180° to 180°
		if (degrees > 180) {
			newDegrees = degrees - 360;
		}

		double degreeDiff = newDegrees - curDegrees;
		if (degreeDiff < 5 && degreeDiff > -5) {
			pid->setPitchSetpoint_Degree(20);
		}
		else {
			pid->setPitchSetpoint_Degree(0);

			//Calculate the realDegree according to the results of the Magnetometer
			double realDegree = newDegrees;
			if (realDegree >= 0 && realDegree <= 180) {
				if (compassValue >= 0 && compassValue <= 180) {
					if (realDegree > compassValue) {
						realDegree = realDegree - compassValue;
					}
					else {
						realDegree = (compassValue - realDegree) * (-1);
					}
				}
				if (compassValue > 180) {
					realDegree = (compassValue - realDegree) * (-1);
					if (realDegree < -180) {
						realDegree = realDegree + 360;
					}
				}
			}
			else if (realDegree < 0 && realDegree >= -180) {
				if (compassValue >= 0 && compassValue <= 180) {
					realDegree = realDegree - compassValue;
				}
				if (compassValue > 180) {
					double newCompass = compassValue - 360;
					if (realDegree > newCompass) {
						realDegree = (newCompass - realDegree) * (-1);
					}
					else {
						realDegree = realDegree - newCompass;
					}
				}
			}

			pid->setYawSetpoint_Degree(realDegree);
		}
		
		curDegrees = newDegrees;
		delay(500);
	}

	objectDetectionThread.join();

}

/*Results (HMC5883L):
	If D is greater than 337.5 degrees or less than 22.5 degrees – North
	If D is between 292.5 degrees and 337.5 degrees – North-West
	If D is between 247.5 degrees and 292.5 degrees – West
	If D is between 202.5 degrees and 247.5 degrees – South-West
	If D is between 157.5 degrees and 202.5 degrees – South
	If D is between 112.5 degrees and 157.5 degrees – South-East
	If D is between 67.5 degrees and 112.5 degrees – East
	If D is between 22.5 degrees and 67.5 degrees – North-East
*/

void AutoFlight::setWaypoints(string points) {
	size_t pos = 0;
	std::string token;
	double curVal;
	
	int counter = 0;
	double curLat = 0.0;
	double curLong = 0.0;
	
	string delimiter = ":";
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