/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: This class is used to store the different waypoints of a route
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#include "WayPoint.h"

WayPoint::WayPoint(double lat, double lon) {
	latitude = lat;
	longitude = lon;
}

WayPoint::WayPoint(){}

double WayPoint::getLatitude() {
	return latitude;
}

double WayPoint::getLongitude() {
	return longitude;
}

double WayPoint::setLatitude(double lat) {
	latitude = lat;
}

double WayPoint::setLongitude(double longi) {
	longitude = longi;
}