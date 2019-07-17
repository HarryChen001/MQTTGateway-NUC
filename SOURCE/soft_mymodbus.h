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

class modbus {
public:
	modbus();
	~modbus();
	void openmainthread();
private:
	void modbus_set_double_to_int16(uint16_t buff[], double input);
	double modbus_get_double_from_int16(uint16_t buff[]);
	void modbus_set_float_to_int16(uint16_t buff[], float input);
	float modbus_get_float_from_int16(uint16_t buff[]);
	void modbus_rtu_init();
	static void modbus_read_thread(modbus* params, struct _Allinfo_t* pAllinfo);
	static void modbus_read485_thread(modbus* params,struct _Allinfo_t* pallinfotemp);
	static void modbus_write_thead(modbus* params);
	std::mutex modbus_mutex;
};

#endif // _SOFT_MYMODBUS_H_
