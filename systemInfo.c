/*
=================================================================================
 Name        : pcd8544_rpi.c
 Version     : 0.1

 Copyright (C) 2012 by Andre Wussow, 2012, desk@binerry.de

 Description :
     A simple PCD8544 LCD (Nokia3310/5110) for Raspberry Pi for displaying some system informations.
	 Makes use of WiringPI-library of Gordon Henderson (https://projects.drogon.net/raspberry-pi/wiringpi/)

	 Recommended connection (http://www.raspberrypi.org/archives/384):
	 LCD pins      Raspberry Pi
	 LCD1 - GND    P06  - GND
	 LCD2 - VCC    P01 - 3.3V
	 LCD3 - CLK    P11 - GPIO0
	 LCD4 - Din    P12 - GPIO1
	 LCD5 - D/C    P13 - GPIO2
	 LCD6 - CS     P15 - GPIO3
	 LCD7 - RST    P16 - GPIO4
	 LCD8 - LED    P01 - 3.3V 

================================================================================
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
================================================================================
 */
#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include "PCD8544.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

// pin setup
int _din  = 1;
int _sclk = 0;
int _dc   = 2;
int _rst  = 4;
int _cs   = 3;
  
// lcd contrast 
int contrast = 50;

double get_cpu_temp() {
	FILE *temperatureFile;
	double T;
	temperatureFile = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
	fscanf (temperatureFile, "%lf", &T);
	T /= 1000;
	fclose (temperatureFile);

	return T;
}

char* get_ip() {
	int fd;
	struct ifreq ifr;
	
	fd                     = socket(AF_INET, SOCK_DGRAM, 0);
	
	ifr.ifr_addr.sa_family = AF_INET;
	
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	
	ioctl(fd, SIOCGIFADDR, &ifr);
	
	close(fd);
	
	//printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	return (char *) inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

int main (void)
{
	// check wiringPi setup
	if (wiringPiSetup() == -1)
	{
		printf("wiringPi-Error\n");
		exit(1);
	}

	// init and clear lcd
	LCDInit(_sclk, _din, _dc, _cs, _rst, contrast);
	LCDclear();

	// show logo
	LCDshowLogo();

	delay(2000);

	for (;;)
	{
		// clear lcd
		LCDclear();
	  
		// get system usage / info
		struct sysinfo sys_info;
		if(sysinfo(&sys_info) != 0)
		{
			printf("sysinfo-Error\n");
		}
	  
		// uptime
		char uptimeInfo[15];
		unsigned long uptime     = sys_info.uptime / 60;
		sprintf(uptimeInfo, "UP   %ld MIN.", uptime);
		
		// cpu info
		char cpuInfo[10]; 
		unsigned long avgCpuLoad = sys_info.loads[0] / 1000;
		sprintf(cpuInfo, "CPU  %ld%%", avgCpuLoad);
		
		// ram info
		char ramInfo[10]; 
		unsigned long totalRam   = sys_info.freeram / 1024 / 1024;
		sprintf(ramInfo, "RAM  %ld MB", totalRam);
		
		char ipInfo[24];
		sprintf(ipInfo, "%s", get_ip());
		
		char tempInfo[24];
		sprintf(tempInfo, "TEMP %5.2f C", get_cpu_temp());

		// build screen
		
		//LCDdrawstring(0, 0, "abcdefghijklmn");
		//LCDdrawstring(0, 7, "opqrstuvwxyz");
		//LCDdrawstring(0, 14, "ABCDEFGHIJKLMN");
		//LCDdrawstring(0, 21, "OPQRSTUVWXYZ");
		//LCDdrawstring(0, 28, "0123456789`-=");
		//LCDdrawstring(0, 35, "~!@#$%^&*()_+");
		//LCDdrawstring(0, 42, "[]{}\\|?/><.,");


		LCDdrawstring(0, 0, ipInfo);
		LCDdrawstring(0, 7, "SYSTEM INFO:");
		LCDdrawline(0, 13, 83, 13, BLACK);
		LCDdrawstring(0, 16, uptimeInfo);
		LCDdrawstring(0, 23, cpuInfo);
		LCDdrawstring(0, 30, ramInfo);
		LCDdrawstring(0, 37, tempInfo);
		LCDdisplay();

		delay(10000);
	}
  
	//for (;;){
	//	printf("LED On\n");
	//	digitalWrite(pin, 1);
	//	delay(250);
	//	printf("LED Off\n");
	//	digitalWrite(pin, 0);
	//	delay(250);
	//}
	
	return 0;
}

