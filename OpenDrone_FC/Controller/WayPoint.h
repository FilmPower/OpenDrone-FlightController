#pragma once
class WayPoint
{
private:
	double latitude;
	double longitude;
	double altitude;

public:
	WayPoint(double lat, double lon, double alt);
	double getLatitude();
	double getLongitude();
	double getAltitude();
};

