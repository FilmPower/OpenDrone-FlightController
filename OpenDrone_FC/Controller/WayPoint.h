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

