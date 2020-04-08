/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: The class for the ultrasonic to measure the distance
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#include "./AbstractSensor/Ultrasonic.h"

#pragma once
class GYUS42 : 
	public virtual Ultrasonic
{
public:
	GYUS42();
	int distance;
	int fd;
	bool run;

	void runUltrasonic();
	double getDistance();
	void interruptUltrasonic();
	void readVal();
};

