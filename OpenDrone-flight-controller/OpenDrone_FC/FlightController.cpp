/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 */

#include "FlightController.h"
#include "Sensor/HCSR04.h"
#include "Sensor/BMP180.h"
#include "Sensor/MPU6050.h"
#include "Sensor/HMC5883L.h"
#include "Motor/PWMMotorTest.h"
#include "Network/TCPServer.h"
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <wiringPi.h>
using namespace std;

TCPServer tcp;
PWMMotorTest* pw = new PWMMotorTest();

FlightController::FlightController()
{
}

static void *runUltrasonic(void *interval)
{
	//Initializing the wiringPi-Gpio's
	int rc = wiringPiSetupGpio();
	if (rc != 0)
	{
		cout << "Failed to wiringPiSetupGpio()\n";
		exit(1);
	}

	//Initialize the sensors
	int len = 2;
	HCSR04* sensors[len];
	sensors[0] = new HCSR04(17, 27, 1);
	sensors[1] = new HCSR04(17, 27, 1);

	//Infinite loop to keep measuring --> TODO: Need to be changed
	float curDistance;
	while (1)
	{
		for (int i = 0; i < len; i++)
		{
			curDistance = sensors[i]->distance();
			cout << "Ultrasonic " << sensors[i]->getId() << ": Distance: " << curDistance << "\n";
			delay(2);
		}
		delay((int)interval);
	}
}

static void *runGyroAccelerometer(void *interval)
{
	//Initializing the sensor
	MPU6050* gyroAcc = new MPU6050();
	//ofstream myfile;
	//myfile.open("example.csv");

	//Infinite loop to keep measuring --> TODO: Need to be changed
	while (1)
	{
		float *values = gyroAcc->getValues();
		cout << "Acc x=" << values[1] << " y=" << values[2] << " z=" << values[3];
		cout << " Gyrometer x=" << values[4] << " y=" << values[5] << " z=" << values[6] << "\n";
		//myfile << values[0] << ";" << values[1] << ";" << values[2] << ";" << values[3] << ";" << values[4] << ";" << values[5] << ";" << values[6] << ";\n";
		delay((int)interval);
	}

	//myfile.close();
}

static void *runBarometer(void *interval)
{
	//Initializing the sensor
	BMP180 *barometer = new BMP180();

	//Infinite loop to keep measuring --> TODO: Need to be changed
	while (1)
	{
		float *values = barometer->getBarometerValues();
		cout << "Barometer Temperature: " << values[0] << " Pressure: " << values[1] << "\n";
		delay((int)interval);
	}
}

static void *runMagnetometer(void *interval)
{
	//Initializing the sensor
	HMC5883L *magnetometer = new HMC5883L();

	//Infinite loop to keep measuring --> TODO: Need to be changed
	while (1)
	{
		float *values = magnetometer->getMagnetometerValues();
		cout << "Magnet x=" << values[0] << " y=" << values[1] << " z=" << values[2] << "\n";
		delay((int)interval);
	}
}

/*static void *loop(void * m)
{
	pthread_detach(pthread_self());
	cout << "Starting TCP-Server";
	cout.flush();
	while (1)
	{
		srand(time(NULL));
		string str = tcp.getMessage();
		cout << str;
		cout.flush();
		tcp.Send(str);
		if (str != "")
		{
			int b = atoi(str.c_str());
			if (b >= 180 || b <= 400)
			{
				//pw->SetSpeed(b);
			}
		}
		usleep(1000);
	}
}

static void *sendTemp(void *m) {
	FILE *temperatureFile;
	int temp;

	while (1) {
		temperatureFile = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
		if (temperatureFile == NULL)
			cout << "Error opening file!";
		fscanf(temperatureFile, "%d", &temp);
		fclose(temperatureFile);
		temp /= 1000;
		string s = to_string(temp);
		if (conn) {
			tcp.Send(s);
			cout << "Sent";
		}
		cout << s;
		cout.flush();
		delay(1000);
	}
}*/

int FlightController::run()
{
	//Creating the threads
	int len = 5;
	pthread_t threadIds[len];
	int threads[len];

	//Starts the Thread with the threadId in arg[0], arg[2] is the method, which is called by the thread, arg[3] the arguments of the method
	threads[0] = pthread_create(&threadIds[0], NULL, runUltrasonic, (void *)500);
	threads[1] = pthread_create(&threadIds[1], NULL, runGyroAccelerometer, (void *)500);
	threads[2] = pthread_create(&threadIds[2], NULL, runBarometer, (void *)500);
	threads[3] = pthread_create(&threadIds[3], NULL, runMagnetometer, (void *)500);

	//Checking if there was a problem with creating the Threads
	for (int i = 0; i < len; i++)
	{
		if (threads[i] != 0)
		{
			cout << "Error while creating the Thread " << i << " Error Code " << threads[i];
			return (1);
		}
	}

	//Waits until the threads are interrupted
	pthread_join(threadIds[0], (void**)1);
	pthread_join(threadIds[1], (void**)1);
	pthread_join(threadIds[2], (void**)1);
	pthread_join(threadIds[3], (void**)1);

	/*pthread_t msg;
	int rc = wiringPiSetupGpio();
	if (rc != 0)
	{
		cout << "Failed to wiringPiSetupGpio()\n";
		exit(1);
	}

	//pw->ArmMotor(); //Start Motors !!ONLY one time!!
	//delay(5000);

	tcp.setup(2018);
	if (pthread_create(&msg, NULL, loop, (void *)0) == 0)
	{
		tcp.receive();
	}
	pthread_join(msg, (void**)1);
	tcp.detach();*/
	return (0);
}

FlightController::~FlightController()
{
}
