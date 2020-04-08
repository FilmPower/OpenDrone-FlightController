/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: The class for the ultrasonic to measure the distance
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#include "GYUS42.h"
#include "wiringPi.h"
#include "wiringPiI2C.h"
#include <iostream>
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

#define DEVICE_ADDRESS 0x68

GYUS42::GYUS42() 
{
	this->fd = wiringPiI2CSetup(DEVICE_ADDRESS);
	if (this->fd < 1) {
		cout << "wiringPiI2CSetup(" << DEVICE_ADDRESS << ")" << endl;
		exit(1);
	}
}

void GYUS42::runUltrasonic()
{
	this->run = true;
	while (this->run)
	{
		this->readVal();
	}
}

void GYUS42::interruptUltrasonic()
{
	this->run = false;
}

double GYUS42::getDistance()
{
	return this->distance;
}

void GYUS42::readVal() {
	while (true) {
		//Request Read
		wiringPiI2CWrite(fd, 0x51);

		delay(100);
		//Read
		int res = wiringPiI2CReadReg16(fd, 0xE1);

		int lowByte = res >> 8;
		int highByte = res << 24;
		highByte = highByte >> 24;
		
		distance = (highByte << 8) | lowByte;


	}
}
