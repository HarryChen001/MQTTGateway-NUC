/*
 * soft_myfunction.h
 *
 *  Created on: Jun 13, 2019
 *      Author: ccl
 */

#ifndef SOURCE_SOFT_MYFUNCTION_H_
#define SOURCE_SOFT_MYFUNCTION_H_

#include "MyData.h"
int getportinfosubscript(PortInfo_t* param,int portnums);
void gpioexport(int gpio);
void gpiooutput(int gpio);
void gpiovalue(int gpio,int value);

#endif /* SOURCE_SOFT_MYFUNCTION_H_ */
