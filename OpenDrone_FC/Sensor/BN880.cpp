/*
 * Copyright (c) OpenDrone, 2018.  All rights reserved.
 * The entire project (including this file) is licensed under the GNU GPL v3.0
 * Purpose: The class for the GPS to get the current position
 *
 * 	@author Thomas Brych
 * 	@version 1.0.0 20.03.2020
 */
#include "BN880.h"
#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <iostream>
#include <string>
#include <sstream>
#include "wiringPi.h"
#include <vector>
#include <exception>
using namespace std;

//-------------------------
//----- SETUP USART 0 -----
//-------------------------
//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
int uart0_filestream = -1;

BN880::BN880()
{

	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	}


	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);

}

double* BN880::getGPSValues() {
	static double ar[2];
	ar[0] = latitude;
	ar[1] = longitude;

	return ar;
}

void BN880::interruptGPS() {
	run = false;
}

void BN880::runGPS()
{
	run = true;

	while (run == true)
	{
		// Read up to 255 characters from the port if they are there
		unsigned char rx_buffer[256];
		int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);		//Filestream, buffer to store in, number of bytes to read (max)
		if (rx_length > 0)
		{
			rx_buffer[rx_length] = '\0';
			analyzeResult(rx_buffer);
		}
		delay(500);
	}
}

void BN880::analyzeResult(unsigned char* rx_buffer)
{
	//latitude: N, S
	//longitude: E, W

	//$GNGGA,123656.00,4818.74238,N,01418.00215,E,2,12,0.97,295.8,M,43.9,M,,0000*44
	std::stringstream ss;
	ss << rx_buffer;
	string str;
	ss >> str;

	vector<string> result;
	while (ss.good())
	{
		string substr;
		getline(ss, substr, ',');
		result.push_back(substr);
	}

	int i = 0;
	for (i = 0; i < result.size(); i++) {
		try {
			cout.precision(10);
			if (result.at(i).compare("N") == 0 && i > 0) {
				latitude = stod(result.at(i - 1)) / 100;
				cout << " Found N! Val: " << latitude << std::endl;
			}
			else if (result.at(i).compare("E") == 0 && i > 0) {
				longitude = stod(result.at(i - 1)) / 100;
				cout << "Found E! Val: " << longitude << std::endl;
			}
			else if (result.at(i).compare("S") == 0 && i > 0) {
				latitude = stod(result.at(i - 1)) / 100;
				//cout << "Found S! Val: " << latitude << std::endl;
			}
			else if (result.at(i).compare("W") == 0 && i > 0) {
				longitude = stod(result.at(i - 1)) / 100;
				//cout << "Found W! Val: " << longitude << std::endl;
			}
		}
		catch (int e) {
			cout << "Exception with error-code: " << e << std::endl;
		}
		
		//cout << "Entry: " << result.at(i) << std::endl;
	}
	cout << std::endl;
	
}

BN880::~BN880()
{
}