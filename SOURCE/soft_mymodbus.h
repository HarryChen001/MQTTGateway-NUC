#pragma once
#ifndef _SOFT_MYMODBUS_H_
#define _SOFT_MYMODBUS_H_

#include"modbus/modbus.h"
#include "MyData.h"

void ByteChange(uint16_t* buff, int nums, int endian, int byteorder);
int modbus_set(int write, double inputdata, char* varname, double* buff_dest);

class modbus {
public:
	modbus();
	~modbus();
	int openmainthread();
private:
	void modbus_set_double_to_int16(uint16_t buff[], double input);
	double modbus_get_double_from_int16(uint16_t buff[]);
	void modbus_set_float_to_int16(uint16_t buff[], float input);
	float modbus_get_float_from_int16(uint16_t buff[]);
	int modbus_rtu_init();
	static void* modbus_read_thread(void* params);
	pthread_t pthread_id = 0;
	pthread_mutex_t pthread_mutex;
};

#endif // _SOFT_MYMODBUS_H_
