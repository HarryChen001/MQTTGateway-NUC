#pragma once
#ifndef _SOFT_MYMODBUS_H_
#define _SOFT_MYMODBUS_H_

#include"modbus/modbus.h"
#include "MyData.h"

void ByteChange(uint16_t* buff, int nums, int endian, int byteorder);
int modbus_set(int write, double inputdata, char* varname, double* buff_dest);

#endif // _SOFT_MYMODBUS_H_
