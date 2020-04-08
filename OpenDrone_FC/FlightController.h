/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: The class, that initializes all objects and starts all the necessary threads
 *
 * 	@author Thomas Brych, Tim Klecka
 * 	@version 1.0.0 20.03.2020
 */
#pragma once
class Orientation;
class Barometer;
class Ultrasonic;
class TCPServer;
class Exit;
class PID;
class PWMMotorTest;
class SQLite;
class AutoFlight;
class GPS;
class HMC5883L;

class FlightController
{
public:
	FlightController(int arg);
	~FlightController();
	int run();

private:
	int arg;
	Orientation *orientation;
	Barometer *barometer;
	Ultrasonic *ultrasonic;
	TCPServer *server;
	Exit *error;
	PID *pid;
	PWMMotorTest *pwm;
	GPS *gps;
	SQLite *sql;
	AutoFlight *autoFlight;
	HMC5883L* compass;

	void initObjects();
};
