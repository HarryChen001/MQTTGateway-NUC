#include "soft_mymodbus.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <queue>
#include <map>
#include <thread>

#include "soft_myfunction.h"
#include "MyData.h"

#include "glog/logging.h"

#define modbus_debug 0

using std::cout;
using std::endl;
using std::string;
using std::queue;
using std::map;

void set_coms(int gpio, int on)
{
	if (gpio == 1)
		gpio = 46;
	else if (gpio == 2)
		gpio = 103;
	else if (gpio == 3)
		gpio = 102;
	else if (gpio == 4)
		gpio = 132;
	else if (gpio == 5)
		gpio = 33;

	gpiovalue(gpio, on);
}

void setrts_com1(modbus_t* ctx, int on)
{
	set_coms(1, on);
}
void setrts_com2(modbus_t* ctx, int on)
{
	set_coms(2, on);
}
void setrts_com3(modbus_t* ctx, int on)
{
	set_coms(3, on);
}
void setrts_com4(modbus_t* ctx, int on)
{
	set_coms(4, on);
}
void setrts_com5(modbus_t* ctx, int on)
{
	set_coms(5, on);
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
modbus::modbus()
{
}
modbus::~modbus()
{
}
void modbus::modbus_rtu_init()
{
#ifndef gcc
	for (unsigned int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
	{
		if (Allinfo[i].portinfo.PortNum == 0 || Allinfo[i].portinfo.gpio == -1)
			continue;
		int gpio = Allinfo[i].portinfo.gpio;

		gpioexport(gpio);
		gpiooutput(gpio);
	}
#endif
	for (unsigned int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
	{
		if (Allinfo[i].portinfo.PortNum == 0)
			continue;

		Allinfo[i].write_flag = false;
		int portnums = Allinfo[i].portinfo.PortNum;
		int baud = Allinfo[i].portinfo.baud;
		int databits = Allinfo[i].portinfo.DataBits;
		int stopbits = Allinfo[i].portinfo.StopBits;
		char parity = Allinfo[i].portinfo.Parity[0];
#ifdef gcc //ubuntu debug
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

		string serial = "/dev/ttyS" + std::to_string(portnums);
		Allinfo[i].fdmodbus = modbus_new_rtu(serial.c_str(), baud, parity, databits, stopbits);
		modbus_set_debug(Allinfo[i].fdmodbus, modbus_debug);
#ifndef gcc
		int ret = -1;
		while (ret == -1 && Allinfo[i].portinfo.gpio != -1)
		{
			ret = modbus_rtu_set_rts(Allinfo[i].fdmodbus, MODBUS_RTU_RTS_UP);
		}
		ret = -1;
		rts* setrts = NULL;
		if (portnums == 10)
			setrts = setrts_com1;
		else if (portnums == 3)
		{
			setrts = setrts_com2;
		}
		else if (portnums == 9)
		{
			setrts = setrts_com3;
		}
		else if (portnums == 1)
		{
			setrts = setrts_com4;
		}
		else if (portnums == 6)
		{
			setrts = setrts_com5;
		}
		while (ret == -1 && Allinfo[i].portinfo.gpio != -1)
		{
			ret = modbus_rtu_set_custom_rts(Allinfo[i].fdmodbus, setrts);
		}
#endif
		int rc = -1;
		rc = modbus_connect(Allinfo[i].fdmodbus);
		while (rc == -1)
		{
			cout << "faile to connect" << endl;
			exit(0);
		}
		unsigned int sec = 0;
		unsigned int usec = 50000;
		modbus_set_response_timeout(Allinfo[i].fdmodbus, sec, usec);
	}
}
void modbus::modbus_read_thread(modbus* params, struct _Allinfo_t* pallinfotemp)
{
	LOG(INFO) << "Modbus read thread : Start work! Serial is " << pallinfotemp->portinfo.PortNum << endl << endl;
	modbus_t* modbus = pallinfotemp->fdmodbus;
	uint16_t buff[10];
	while (1)
	{
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
				enumdatatype* datatypetemp = &vartemp->datatype;
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
					while (pallinfotemp->write_flag == true)
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
					if (count >= 7)
					{
					//	cout << BOLDRED << vartemp->VarName << "read modbus timeout!" << endl << RESET;
						LOG(WARNING) << vartemp->VarName << "read modbus timeout" << endl << endl;
						break;
					}
				}
				if (rc == -1)
					continue;

				if (*datatypetemp != uint16 && *datatypetemp != int16 && *datatypetemp != bool_type)
					ByteChange(buff, regnums, regendian, regbyteorder);

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
		sleep(1);
	}
}
void modbus::modbus_write_thead(modbus* params)
{
	LOG(INFO) << "Modbus write thread : Start work!" << endl << endl;
	while (1)
	{
		if (!queue_var_write.empty())
			cout << endl;
		while (!queue_var_write.empty())
		{
			string rec_varname = queue_var_write.front().varname;
			double rec_value = queue_var_write.front().value;
			uint16_t buff[4];
			queue_var_write.pop();
			for (unsigned int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
			{
				if (Allinfo[i].portinfo.id == 0)
				{
					continue;
				}
				modbus_t* modbus = Allinfo[i].fdmodbus;
				for (unsigned int j = 0; j < sizeof(Allinfo[i].deviceinfo) / sizeof(DeviceInfo_t); j++)
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
							Allinfo[i].write_flag = true;
							modbus_flush(modbus);
							modbus_set_slave(modbus, slaveid);
							int rc = -1;
							int count = 0;
							uint16_t temp[4];
							while (rc == -1)
							{
								MODBUS_SET_INT64_TO_INT16(buff, 0, (int64_t)rec_value);
								uint16_t* tempbuff;
								if (str_datatypetemp == "uint16" || str_datatypetemp == "int16")
								{
									regnums = 1;
									*tempbuff = (uint16_t)rec_value;
								}
								else if (str_datatypetemp == "uint32" || str_datatypetemp == "int32")
								{
									regnums = 2;
									ByteChange(buff, regnums, regendian, regbyteorder);
									buff[0] = buff[3];
									buff[1] = buff[2];
									tempbuff = buff;
								}
								else if (str_datatypetemp == "uint64" || str_datatypetemp == "int64")
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
								else if (str_datatypetemp == "double")
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
									rc = modbus_write_and_read_registers(modbus, regadr, regnums, tempbuff, regadr, regnums, temp);

								if (str_datatypetemp != "uint16" && str_datatypetemp != "int16" && str_datatypetemp != "bool")
									ByteChange(temp, regnums, regendian, regbyteorder);

								string varnametemp = varparamstemp->VarName;
								double value = 0;
								if (str_datatypetemp == "uint16")
								{
									value = (uint16_t)temp[0];
								}
								else if (str_datatypetemp == "uint32")
								{
									value = (uint32_t)MODBUS_GET_INT32_FROM_INT16(temp, 0);
								}
								else if (str_datatypetemp == "uint64")
								{
									value = (uint64_t)MODBUS_GET_INT64_FROM_INT16(temp, 0);
								}
								else if (str_datatypetemp == "int16")
								{
									value = (int16_t)temp[0];
								}
								else if (str_datatypetemp == "int32")
								{
									value = (int32_t)MODBUS_GET_INT32_FROM_INT16(temp, 0);
								}
								else if (str_datatypetemp == "int64")
								{
									value = (int64_t)MODBUS_GET_INT64_FROM_INT16(temp, 0);
								}
								else if (str_datatypetemp == "float")
								{
									value = params->modbus_get_float_from_int16(temp);
								}
								else if (str_datatypetemp == "double")
								{
									value = params->modbus_get_double_from_int16(temp);
								}
								else if (str_datatypetemp == "bool")
								{
									value = (uint16_t)temp[0];
								}

								cJSON* publish_json;
								cJSON* params_json;
								time_t nowtime;
								char tmp[64];
								nowtime = time(NULL); //get now time
								strftime(tmp, sizeof(tmp), "%Y%m%d%H%M%S", localtime(&nowtime));
								publish_json = cJSON_CreateObject();

								cJSON_AddStringToObject(publish_json, "id", tmp);
								cJSON_AddStringToObject(publish_json, "method", "method.event.property.post");
								cJSON_AddItemToObject(publish_json, "params", params_json = cJSON_CreateObject());
								cJSON_AddNumberToObject(params_json, varparamstemp->VarName.c_str(), value);
								if (AliyunMqtt.messageid == "0")
								{
									AliyunMqtt.publish(ThemeUpload[0].CtrlPub, 1, publish_json);
								}
								else
								{
									string topic = "/sys/" + AliyunMqtt.productkey + "/" + AliyunMqtt.devicename + "/rrpc/response/" + AliyunMqtt.messageid;
									AliyunMqtt.publish((char*)topic.c_str(), 0, publish_json);
								}
								cJSON_Delete(publish_json);

								count++;
								if (count >= 5)
								{
									cout << BOLDRED << "Write Failed!" << RESET << endl;
									break;
								}
							}
							Allinfo[i].write_flag = false;
						}
					}
				}
			}
		}
		sleep(1);
	}
}
void modbus::openmainthread()
{
	modbus_rtu_init();
	for (unsigned int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
	{
		if (Allinfo[i].portinfo.id != 0)
		{
			int varcount = 0;
			for (int j = 0; j < Allinfo[i].devcount; j++)
			{
				varcount += Allinfo[i].deviceinfo[j].uploadvarcount;
			}
			if (varcount == 0)
				continue;
			modbus_read_threadid[i] = std::thread(modbus_read_thread, this, &Allinfo[i]);
		}
	}
	modbus_write_threadid = std::thread(modbus_write_thead, this);
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
