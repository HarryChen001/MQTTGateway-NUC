#include "soft_mymodbus.h"

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <queue>
#include <map>
#include <thread>

#include "MyData.h"
using std::cout;
using std::endl;
using std::string;
using std::queue;
using std::map;

void setrts_com1(modbus_t* ctx,int on)
{
	if(on)
		system("echo 1 > /sys/class/gpio/gpio46/value");
	else
		system("echo 0 > /sys/class/gpio/gpio46/value");
}
void setrts_com2(modbus_t* ctx,int on)
{
	if(on)
		system("echo 1 > /sys/class/gpio/gpio103/value");
	else
		system("echo 0 > /sys/class/gpio/gpio103/value");
}
void setrts_com3(modbus_t* ctx,int on)
{
	if(on)
		system("echo 1 > /sys/class/gpio/gpio102/value");
	else
		system("echo 0 > /sys/class/gpio/gpio102/value");
}
void setrts_com4(modbus_t* ctx,int on)
{
	if(on)
		system("echo 1 > /sys/class/gpio/gpio132/value");
	else
		system("echo 0 > /sys/class/gpio/gpio132value");
}
void setrts_com5(modbus_t* ctx,int on)
{
	if(on)
		system("echo 1 > /sys/class/gpio/gpio33/value");
	else
		system("echo 0 > /sys/class/gpio/gpio33/value");
}
void ByteChange(uint16_t* buff, int nums, int endian, int byteorder)
{
	if (!endian)
	{
		for (int i = 0; i < nums / 2; i++)
		{
			uint16_t temp;
			temp = buff[nums - 1 - i];
			buff[nums - 1 - i] = buff[i];
			buff[i] = temp;
			buff[nums - 1 - i] = (buff[nums - 1 - i] << 8 | buff[nums - 1 - i] >> 8);
			buff[i] = (buff[i] << 8 | buff[i] >> 8);
		}
	}
	if (!byteorder)
	{
		for (int i = 0; i < nums; i++)
		{
			buff[i] = (buff[i] << 8 | buff[i] >> 8);
		}
	}
}
void modbus_set_double_to_int16(uint16_t buff[], double input)
{
	doubleunion doubletype;
	doubletype.doubletype = input;
	MODBUS_SET_INT64_TO_INT16(buff, 0, doubletype.int64type);
}
double modbus_get_double_from_int16(uint16_t buff[])
{
	doubleunion doubletype;
	doubletype.int64type = MODBUS_GET_INT64_FROM_INT16(buff, 0);
	return doubletype.doubletype;
}
void modbus_set_float_to_int16(uint16_t buff[], float input)
{
	floatunion floattype;
	floattype.floattype = input;
	MODBUS_SET_INT32_TO_INT16(buff, 0, floattype.int32type);
}
float modbus_get_float_from_int16(uint16_t buff[])
{
	floatunion floattype;
	floattype.int32type = MODBUS_GET_INT32_FROM_INT16(buff, 0);
	return floattype.floattype;
}
modbus::modbus()
{
}
modbus::~modbus()
{

}
int modbus::modbus_rtu_init()
{
	write_flag = 0;
	for (int i = 0; i < 15; i++)
	{
		if (Allinfo[i].portinfo.PortNum == 0)
			continue;

		int portnums = Allinfo[i].portinfo.PortNum;
		int baud = Allinfo[i].portinfo.baud;
		int databits = Allinfo[i].portinfo.DataBits;
		int stopbits = Allinfo[i].portinfo.StopBits;
		char parity = Allinfo[i].portinfo.Parity[0];
		string serial;
#ifdef gcc
		if (portnums == 10)
			portnums = 0;
		else if (portnums == 3)
			portnums = 1;
		else if (portnums == 9)
			portnums = 2;
		else if (portnums == 1)
			portnums = 3;
		else if (portnums == 6)
			portnums = 4;
		else if (portnums == 7)
			portnums = 5;
		else if (portnums == 2)
			portnums = 6;
		else if (portnums == 8)
			portnums = 7;
		else if (portnums == 4)
			portnums = 8;
#endif
		sprintf((char*)serial.c_str(), "%s%d", "/dev/ttyS", portnums);

		Allinfo[i].fdmodbus = modbus_new_rtu(serial.c_str(), baud, parity, databits, stopbits);
//		modbus_set_debug(Allinfo[i].fdmodbus, 1);
#ifndef gcc
		int ret = -1;
		while (ret == -1 && Allinfo[i].portinfo.gpio != -1)
		{
			ret = modbus_rtu_set_rts(Allinfo[i].fdmodbus, MODBUS_RTU_RTS_UP);
		}
		ret = -1;
		while (ret == -1 && Allinfo[i].portinfo.gpio != -1)
		{
			rts* setrts;
			if(portnums == 10)
				setrts = setrts_com1;
			else if(portnums == 3)
			{
				setrts = setrts_com2;
			}
			else if(portnums == 9)
			{
				setrts = setrts_com3;
			}
			else if(portnums == 1)
			{
				setrts = setrts_com4;
			}
			else if(portnums == 6)
			{
				setrts = setrts_com5;
			}
			ret = modbus_rtu_set_custom_rts(Allinfo[i].fdmodbus, setrts);
		}
#endif
		int rc = modbus_connect(Allinfo[i].fdmodbus);
		while (rc == -1)
		{
			cout << "faile to connect" << endl;
			exit(0);
		}
		unsigned int sec = 0;
		unsigned int usec = 100000;
		modbus_set_response_timeout(Allinfo[i].fdmodbus, sec, usec);
	}
}
void modbus::modbus_read_thread(modbus* params, struct _Allinfo_t* pallinfotemp)
{
	params->modbus_rtu_init();
	while (1)
	{
		modbus_t* modbus = pallinfotemp->fdmodbus;
		uint16_t buff[10];

		for (int j = 0; j < pallinfotemp->devcount; j++)
		{
			if (pallinfotemp->deviceinfo[j].id == 0)
				continue;
			int regendian = pallinfotemp->deviceinfo[j].regedian;
			int regbyteorder = pallinfotemp->deviceinfo[j].byteorder;
			modbus_set_slave(modbus, pallinfotemp->deviceinfo[j].address);
			for (int k = 0; k < pallinfotemp->deviceinfo[j].uploadvarcount; k++)
			{
				/*check variable */
				VarParam_t* vartemp = &(pallinfotemp->deviceinfo[j].uploadvarparam[k]);
				int regadr = vartemp->RegAdr;
				int regnums = 1;
				int rc = -1;
				string regtypetemp = vartemp->RegType;
				string str_datatypetemp = vartemp->DataType;
				enumdatatype* datatypetemp;
				datatypetemp = &vartemp->datatype;
				if (str_datatypetemp == "uint16")
				{
					*datatypetemp = uint16;
					regnums = 1;
				}
				else if (str_datatypetemp == "uint32")
				{
					*datatypetemp = uint32;
					regnums = 2;
				}
				else if (str_datatypetemp == "uint64")
				{
					*datatypetemp = uint64;
					regnums = 4;
				}
				else if (str_datatypetemp == "int16")
				{
					*datatypetemp = int16;
					regnums = 1;
				}
				else if (str_datatypetemp == "int32")
				{
					*datatypetemp = int32;
					regnums = 2;
				}
				else if (str_datatypetemp == "int64")
				{
					*datatypetemp = int64;
					regnums = 4;
				}
				else if (str_datatypetemp == "float")
				{
					*datatypetemp = float_type;
					regnums = 2;
				}
				else if (str_datatypetemp == "double")
				{
					*datatypetemp = double_type;
					regnums = 4;
				}
				else if (str_datatypetemp == "bool")
				{
					*datatypetemp = bool_type;
					regnums = 1;
				}
				int count = 0;
				while (rc == -1)
				{
					while (params->write_flag)
					{
						cout << "Writing" << endl;
						sleep(1);
					}
					if (regtypetemp == "W3")
					{
						rc = modbus_read_registers(modbus, regadr, regnums, buff);
					}
					else if (regtypetemp == "W4")
					{
						rc = modbus_read_input_registers(modbus, regadr, regnums, buff);
					}
					else if (regtypetemp == "B0")
					{
						memset(buff, 0, sizeof(buff));
						rc = modbus_read_bits(modbus, regadr, regnums, (uint8_t*)buff);
					}
					else if (regtypetemp == "B1")
					{
						memset(buff, 0, sizeof(buff));
						rc = modbus_read_input_bits(modbus, regadr, regnums, (uint8_t*)buff);
					}
					count++;
					if (count >= 5)
					{
						cout << BOLDRED << vartemp->VarName <<  " : read modbus timeout!" << endl << RESET;
						break;
					}
				}
				if (rc == -1)
					continue;

				if (*datatypetemp != uint16 && *datatypetemp != int16 && *datatypetemp != bool_type)
					ByteChange(buff, regnums, regendian, regbyteorder);

				if (rc != -1)
				{
					string varnametemp = vartemp->VarName;
					if (*datatypetemp == uint16)
					{
						var[varnametemp] = (uint16_t)buff[0];
					}
					else if (*datatypetemp == uint32)
					{
						var[varnametemp] = (uint32_t)MODBUS_GET_INT32_FROM_INT16(buff, 0);
					}
					else if (*datatypetemp == uint64)
					{
						var[varnametemp] = (uint64_t)MODBUS_GET_INT64_FROM_INT16(buff, 0);
					}
					else if (*datatypetemp == int16)
					{
						var[varnametemp] = (int16_t)buff[0];
					}
					else if (*datatypetemp == int32)
					{
						var[varnametemp] = (int32_t)MODBUS_GET_INT32_FROM_INT16(buff, 0);
					}
					else if (*datatypetemp == int64)
					{
						var[varnametemp] = (int64_t)MODBUS_GET_INT64_FROM_INT16(buff, 0);
					}
					else if (*datatypetemp == float_type)
					{
						var[varnametemp] = params->modbus_get_float_from_int16(buff);
					}
					else if (*datatypetemp == double_type)
					{
						var[varnametemp] = params->modbus_get_double_from_int16(buff);
					}
					else if (*datatypetemp == bool_type)
					{
						var[varnametemp] = (uint16_t)buff[0];
					}
				}
			}
		}
	}
}
void modbus::modbus_write_thead(modbus* params)
{
	while (1)
	{
		while (!queue_var_write.empty())
		{
			string rec_varname = queue_var_write.front().varname;
			double rec_value = queue_var_write.front().value;
			uint16_t buff[4];
			queue_var_write.pop();
			for (int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
			{
				if (Allinfo[i].portinfo.id == 0)
				{
					continue;
				}
				modbus_t* modbus = Allinfo[i].fdmodbus;
				for (int j = 0; j < sizeof(Allinfo[i].deviceinfo) / sizeof(DeviceInfo_t); j++)
				{
					DeviceInfo_t* devicetemp = &Allinfo[i].deviceinfo[j];
					if (devicetemp->id == 0)
					{
						continue;
					}
					int regendian = devicetemp->regedian;
					int regbyteorder = devicetemp->byteorder;
					int slaveid = devicetemp->address;
					for (int k = 0; k < devicetemp->allvarcount; k++)
					{
						VarParam_t* varparamstemp = &(devicetemp->allvarparams[k]);
						if (varparamstemp->id == 0)
						{
							continue;
						}
						if (rec_varname == varparamstemp->VarName)
						{
							int regadr = varparamstemp->RegAdr;
							int regnums = 1;
							string regtypetemp = varparamstemp->RegType;
							string str_datatypetemp = varparamstemp->DataType;
							params->write_flag = 1;
							modbus_flush(modbus);
							modbus_set_slave(modbus, devicetemp->address);
							int rc = -1;
							int count = 0;
							while (rc == -1)
							{
								MODBUS_SET_INT64_TO_INT16(buff, 0, (int64_t)rec_value);
								uint16_t* tempbuff;
								if (str_datatypetemp == "uint16"||str_datatypetemp == "int16")
								{
									regnums = 1;
									*tempbuff = (uint16_t)rec_value;
								}
								else if (str_datatypetemp == "uint32"||str_datatypetemp == "int32")
								{
									regnums = 2;
									ByteChange(buff, regnums, regendian, regbyteorder);
									buff[0] = buff[3];
									buff[1] = buff[2];
									tempbuff = buff;
								}
								else if (str_datatypetemp == "uint64" || str_datatypetemp == "int64" )
								{
									regnums = 4;
									ByteChange(buff, regnums, regendian, regbyteorder);
									tempbuff = buff;
								}
								else if (str_datatypetemp == "float")
								{
									regnums = 2;
									params->modbus_set_float_to_int16(buff, rec_value);
									ByteChange(buff, regnums, regendian, regbyteorder);
									tempbuff = buff;
								}
								else if(str_datatypetemp == "double")
								{
									regnums = 4;
									params->modbus_set_double_to_int16(buff, rec_value);
									ByteChange(buff, regnums, regendian, regbyteorder);
									tempbuff = buff;
								}
								if (str_datatypetemp == "bool")
								{
									regnums = 1;
									rc = modbus_write_bit(modbus, regadr, rec_value);
								}
								else
									cout << "write :" <<  modbus_write_registers(modbus, regadr, regnums, tempbuff) << endl;
								count++;
								if (count >= 5)
								{
									cout << BOLDRED << "Write Failed!" << RESET << endl;
									break;
								}
							}
							params->write_flag = 0;
						}
					}
				}
			}
		}
		usleep(10000);
	}
}
int modbus::openmainthread()
{
	for (unsigned int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
	{
		if (Allinfo[i].portinfo.id != 0)
		{
			std::thread(modbus_read_thread, this, &Allinfo[i]).detach();
		}
	}
	std::thread(modbus_write_thead, this).detach();
}

void modbus::modbus_set_double_to_int16(uint16_t buff[], double input)
{
	doubleunion doubletype;
	doubletype.doubletype = input;
	MODBUS_SET_INT64_TO_INT16(buff, 0, doubletype.int64type);
}
double modbus::modbus_get_double_from_int16(uint16_t buff[])
{
	doubleunion doubletype;
	doubletype.int64type = MODBUS_GET_INT64_FROM_INT16(buff, 0);
	return doubletype.doubletype;
}
void modbus::modbus_set_float_to_int16(uint16_t buff[], float input)
{
	floatunion floattype;
	floattype.floattype = input;
	MODBUS_SET_INT32_TO_INT16(buff, 0, floattype.int32type);
}
float modbus::modbus_get_float_from_int16(uint16_t buff[])
{
	floatunion floattype;
	floattype.int32type = MODBUS_GET_INT32_FROM_INT16(buff, 0);
	return floattype.floattype;
}
