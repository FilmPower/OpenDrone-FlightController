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

