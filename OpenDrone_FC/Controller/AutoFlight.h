#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "PID.h"
#include "../Sensor/AbstractSensor/Ultrasonic.h"
#include "../Sensor/AbstractSensor/GPS.h"
#include <vector>
#include "WayPoint.h"

#pragma once
class AutoFlight
{
public:
	AutoFlight(PID *p, Ultrasonic *u, GPS *g);
	~AutoFlight();
	void start();
	void stop();
	void setWaypoints(std::string points);

private:
	PID* pid;
	Ultrasonic* ultrasonic;
	GPS* gps;

	std::vector<WayPoint *> *waypoints;

	bool run = false;

	void doAutoFlight();
	double calcDegrees(WayPoint* curWaypoint);

	void setPitchDegree(float degree);
	void setRollDegree(float degree);
	void setYawDegree(float degree);
};

