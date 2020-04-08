/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: The class for the GPS to get the current position
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#include <string>
#include <stdio.h>
#include "AbstractSensor/GPS.h"
#pragma once
class BN880 :
	public virtual GPS
{
public:
	BN880();
	~BN880();

	void interruptGPS();
	void runGPS();
	//[0]: latitude, [1]: longitude
	double *getGPSValues();

private:
	//latitude: N, S
	//longitude: E, W
	double latitude, longitude;
	bool run = true;

	void analyzeResult(unsigned char* rx_buffer);
};

