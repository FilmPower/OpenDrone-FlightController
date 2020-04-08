/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: The abstract class used to implement the ultrasonic
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#pragma once
class Ultrasonic
{
public:
	virtual double getDistance() = 0;
	virtual void runUltrasonic() = 0;
	virtual void interruptUltrasonic() = 0;
};
