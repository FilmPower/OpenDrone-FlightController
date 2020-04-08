/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: TODO
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#include "AbstractSensor/Magnetometer.h"

#pragma once
class HMC5883L : public virtual Magnetometer
{
public:
	HMC5883L();
	~HMC5883L();
	double *getMagnetometerValues();
    double compassValue;

private:
	int fd;
	short readRawData(int addr);
    double getCompass();

};

