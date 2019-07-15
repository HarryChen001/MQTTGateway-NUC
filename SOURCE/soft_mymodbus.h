#pragma once
#ifndef _SOFT_MYMODBUS_H_
#define _SOFT_MYMODBUS_H_

#include <pthread.h>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include"modbus/modbus.h"
#include "MyData.h"

int modbus_set(int write, double inputdata, char* varname, double* buff_dest);

class modbus {
public:
	modbus();
	~modbus();
	int openmainthread();
private:
	void modbus_set_double_to_int16(uint16_t buff[], double input);
//	static void* setrts(modbus_t* modbus,int on);
	double modbus_get_double_from_int16(uint16_t buff[]);
	void modbus_set_float_to_int16(uint16_t buff[], float input);
	float modbus_get_float_from_int16(uint16_t buff[]);
	int modbus_rtu_init();
	static void modbus_read_thread(modbus* params, struct _Allinfo_t* pAllinfo);
	static void modbus_write_thead(modbus* params);
	std::mutex modbus_mutex;
	bool write_flag;
};

#endif // _SOFT_MYMODBUS_H_
