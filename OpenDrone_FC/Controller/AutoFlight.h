/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: This class is used so the drone can fly autonomously
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "PID.h"
#include "../Sensor/AbstractSensor/Ultrasonic.h"
#include "../Sensor/AbstractSensor/GPS.h"
#include "../Sensor/HMC5883L.h"
#include <vector>
#include "WayPoint.h"

#pragma once
class AutoFlight
{
public:
	AutoFlight(PID *p, Ultrasonic *u, GPS *g, HMC5883L* m);
	~AutoFlight();
	void start();
	void stop();
	void setWaypoints(std::string points);
	void startFlying();

private:
	PID* pid;
	Ultrasonic* ultrasonic;
	GPS* gps;
	HMC5883L* compass;

	std::vector<WayPoint *> *waypoints;

	double compassValue;
	double correctionDegree = 0.0;
	bool run = false;

	void doAutoFlight();
	double calcDegrees(WayPoint* curWaypoint);

	void setPitchDegree(float degree);
	void setRollDegree(float degree);
	void setYawDegree(float degree);
};

