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