#pragma once
#ifndef _SOFT_MYMODBUS_H_
#define _SOFT_MYMODBUS_H_

#include"modbus/modbus.h"

class MyModbus {
public:
	MyModbus();
	~MyModbus();
	int modbusconfig(int portid, int baud, char* parity,int data_bit, int stop_bit);
	
	int modbusread1(int slave);
private:
	modbus_t* mb;
};

int modbus_set(int write,int inputdata,char* varname, float* buff_dest);

#endif // _SOFT_MYMODBUS_H_
