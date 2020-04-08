/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: This class is used to calculate the speed of the single motors of the drone
 *
 * 	@author Thomas Brych, Tim Klecka
 * 	@version 0.0.2 27.06.2019
 */
#include "Orientation.h"
#include "iostream"
#include <string.h>

class PWMMotorTest;
class Barometer;
class Ultrasonic;
class AutoFlight;

#pragma once
class PID
{
public:
	~PID();
	static PID *getInstance(Orientation *o, PWMMotorTest *p, Barometer *b, Ultrasonic *u);
	static PID *getInstanceCreated();
	static PID *instance;


	Orientation* getOrientatin();
	PWMMotorTest* getPwmMotorTest();

	void calcValues();

	void setRun(bool curRun);
	void armMotor();
	bool isInit();
	void interruptPid();
  
  void updateHeightControl();
  void setWayPoints(std::string points);
  
	//Used to fly the drone
	void setThrottle(float curThrottle);
	void setPitchSetpoint(int curPitchSetpoint);
	void setRollSetpoint(int curRollSetpoint);
	void setYawSetpoint(int curYawSetpoint);
	void setAutoFlight(AutoFlight* a);

  
	//Used for logging
	int *getThrottles();
	float *getPIDVals();
	double *getAltVals();

	//Only for testing
	void setP(float curP);
	void setI(float curI);
	void setD(float curD);

	void setPitchSetpoint_Degree(float degree);
	void setRollSetpoint_Degree(float degree);
	void setYawSetpoint_Degree(float degree);

private:
	/* These variables are used for the HeightControl: */
	float pid_p_gain_heightHold = 2.5;
	float pid_i_gain_heightHold = 0.5;
	float pid_d_gain_heightHold = 4.5;
	int startThrottleAltitude = 1450;		//The default throttle after reaching the wanted height

	PID(Orientation *o, PWMMotorTest *p, Barometer *b, Ultrasonic *u);
	PWMMotorTest *pwm = NULL;
	Orientation *orientation = NULL;
	Barometer *barometer = NULL;
	Ultrasonic *ultrasonic = NULL;
	AutoFlight *autoFlight = NULL;

	float pid_error_temp;
	float pid_cur_val = 0;
	double* curPitchRollYaw;

	//Variables for the Pitch/Roll/Yaq PID
	float pid_i_mem_roll = 0, pid_roll_setpoint = 0, pid_output_roll, pid_last_roll_d_error = 0;
	float pid_i_mem_pitch = 0, pid_pitch_setpoint = 0, pid_output_pitch, pid_last_pitch_d_error = 0;
	float pid_i_mem_yaw = 0, pid_yaw_setpoint = 0, pid_output_yaw, pid_last_yaw_d_error = 0;

	float pid_p_gain_roll = 2.0; //1.25             //Gain setting for the roll P-controller 0.65
	float pid_i_gain_roll = 0.01; // 0.05;          //Gain setting for the roll I-controller 0.0006
	float pid_d_gain_roll = 60; // 90;              //Gain setting for the roll D-controller 60
	int pid_max_roll = 1000;						//Maximum output of the PID-controller (+/-)

	float pid_p_gain_pitch = pid_p_gain_roll;		//Gain setting for the pitch P-controller.
	float pid_i_gain_pitch = pid_i_gain_roll;		//Gain setting for the pitch I-controller.
	float pid_d_gain_pitch = pid_d_gain_roll;		//Gain setting for the pitch D-controller.
	int pid_max_pitch = pid_max_roll;				//Maximum output of the PID-controller (+/-)

	float pid_p_gain_yaw = 2.0;						//Gain setting for the yaw P-controller. 4.0
	float pid_i_gain_yaw = 0.005;					//Gain setting for the yaw I-controller. 0.02
	float pid_d_gain_yaw = 0.00;					//Gain setting for the yaw D-controller.
	int pid_max_yaw = 200;							//Maximum output of the PID-controller (+/-)

	//Varibles for the HeightControl PID
	float pid_i_mem_heightHold, pid_output_height, pid_last_height_error = 0;
	float pid_max_heightHold = 200;
	float pid_last_heightHold_error = 0.0;

	float pid_last_start_error = 0.0;
	float pid_p_gain_start = 3;
	float pid_d_gain_start = 11;

	bool startUp = true;		//Used to bring the drone to the wanted start-height
	bool isStarting = true;		//Used to start the drone (up to 30cm)
	bool heightControl = false;	//If heightControl should be used
	bool hasHeightControl = false;
	int wantedDistanceStart = 120;	//The wanted distance when auto-starting the drone
	double wantedPressure = 0;

	//Default variables
	int esc_1, esc_2, esc_3, esc_4;
	int throttle = 1050;
	float maxAngle = 20;
	float factorControl = maxAngle / 480;			//Maximum maxAngle° (480 steps)
	bool run = false, stop = false, log = false;
	void calcPid();


};
