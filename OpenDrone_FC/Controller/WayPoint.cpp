#include "WayPoint.h"

WayPoint::WayPoint(double lat, double lon, double alt) {
	latitude = lat;
	longitude = lon;
	altitude = alt;
}

double WayPoint::getLatitude() {
	return latitude;
}

double WayPoint::getLongitude() {
	return longitude;
}

double WayPoint::getAltitude() {
	return altitude;
}