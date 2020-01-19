#pragma once
class PID;
class Utrasonic;

class AutoFlight
{
public:
	AutoFlight(PID *p, Ultrasonic *u);
	~AutoFlight();
	void start();
	void stop();

private:
	bool run = false;
	PID* pid;
	Ultrasonic* ultrasonic;
	void doAutoFlight();
	void testAutoFlight();
	void startObjectDetection();

	void setPitchDegree(float degree);
	void setRollDegree(float degree);
	void setYawDegree(float degree);
};

