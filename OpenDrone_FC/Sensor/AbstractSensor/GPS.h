/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: The abstract class used to implement the gps
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#pragma once
class GPS
{
public:
    
    virtual double* getGPSValues() = 0;
    virtual void runGPS() = 0;
    virtual void interruptGPS() = 0;
};