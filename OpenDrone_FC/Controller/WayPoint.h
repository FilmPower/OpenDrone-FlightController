/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: This class is used to store the different waypoints of a route
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#pragma once
class WayPoint
{
private:
	double latitude;
	double longitude;
	double altitude;

public:
	WayPoint(double lat, double lon);
	WayPoint();
	
	double getLatitude();
	double getLongitude();
	double setLatitude(double lat);
	double setLongitude(double longi);
};

