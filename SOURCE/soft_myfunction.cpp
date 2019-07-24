/*
 * soft_myfunction.cpp
 *
 *  Created on: Jun 13, 2019
 *      Author: ccl
 */

#include "soft_myfunction.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "MyData.h"
using std::cout;
using std::endl;

int getportinfosubscript(PortInfo_t* param,int portnums)
{
	for(int i = 0; i < param[0].portcount;i++)
	{
		if(portnums == param[i].PortNum)
		{
			return i;
		}
	}
	return -1;
}
void gpioexport(int gpio)
{
	char buff[100] = "/sys/class/gpio/export";
	FILE* fp = NULL;
	while(fp==NULL)
	{
		fp = fopen(buff,"a");
		if(fp != NULL)
		{
			fprintf(fp,std::to_string(gpio).c_str());
			fclose(fp);
			break;
		}
	}
}
void gpiooutput(int gpio)
{
	char buff[100];
	sprintf(buff,"/sys/class/gpio/gpio%d/direction",gpio);
	FILE* fp = NULL;
	while(fp == NULL)
	{
		fp = fopen(buff,"a");
		if(fp != NULL)
		{
			fprintf(fp,"output");
			fclose(fp);
			break;
		}
	}
}
void gpiovalue(int gpio,int value)
{
	FILE* fp = NULL;
	char buff[100];
	sprintf(buff,"/sys/class/gpio/gpio%d/value",gpio);
	while(fp == NULL)
	{
		fp = fopen(buff,"a");
		if(fp != NULL)
		{
			fprintf(fp,std::to_string(value).c_str());
			fclose(fp);
			break;
		}
	}
}
