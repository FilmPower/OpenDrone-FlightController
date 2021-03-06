/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: The abstract class used to implement the barometer
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#pragma once
class Barometer
{
public:
	//ar[0] = Temperature, ar[1] = Pressure
	virtual double *getBarometerValues() = 0;
	virtual void runBarometer() = 0;
	virtual void interruptBaromter() = 0;
};