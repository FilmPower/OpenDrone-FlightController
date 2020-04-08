/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: The class, that initializes all objects and starts all the necessary threads
 *
 * 	@author Thomas Brych, Tim Klecka
 * 	@version 1.0.0 20.03.2020
 */
#include "FlightController.h"
#include <iostream>
#include <string.h>

using namespace std;

/**
	Start our Flightcontroller
	@param int arg

	@return void
*/
void startFC(int arg) {
	FlightController* fc = new FlightController(arg);

	cout << "Starting the Flight Controller\n";
	fc->run();
	cout << "Stopping Flight Controller!\n";
}

/**
	Main Method of our Flightcontroller
	@param int argc, char * argv[]
	@return Errorcode
*/
int main(int argc, char* argv[])
{
	//Arguments should be used to start the FC in a specific mode (-d Debug, -l Log) --> not implemented yet
	if (argc == 1) {
		startFC(0);
	}
	else if (argc == 2) {
		if (strcmp(argv[1], "-d") == 0) {
			startFC(1);
		}
		else if (strcmp(argv[1], "-t") == 0) {
			startFC(2);
		}
		else {
			return 1;
		}
	}
	else {
		return 1;
	}
}