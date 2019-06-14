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
