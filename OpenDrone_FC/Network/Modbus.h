/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: This class is used to react on the commands received from the app
 *
 * 	@author Markus Kurzmann, Thomas Brych
 * 	@version 0.0.2 26.06.2019
 */
#pragma once

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

class PWMMotorTest;

class Modbus
{
public:
    Modbus();
    ~Modbus();
    void Interpret(string str);
	void checkMotors(PWMMotorTest *pwm, int motorNum);
};

