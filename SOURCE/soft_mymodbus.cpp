#include "soft_mymodbus.h"

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <iomanip>

#include "MyData.h"
using std::cout;
using std::endl;

int portsubscript;
int varsubscript = 0;
int devsubscript = 0;

void setrts(modbus_t* ctx, int on)
{
	char control[100];
	if (on == 1)
	{
		sprintf(control, "%s%d%s", "echo 1 > /sys/class/gpio/gpio", PortInfo[portsubscript].gpio, "/value");
		system(control);
	}
	else
	{
		sprintf(control, "%s%d%s", "echo 0 > /sys/class/gpio/gpio", PortInfo[portsubscript].gpio, "/value");
		system(control);
	}
}

void ByteChange(uint16_t* buff,int nums,int endian,int byteorder)
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
int modbus_set(int write, double inputdata, char* varname, double* buff_dest)
{
	modbus_t* mb;
	int portid = 0;
	char serialport[20];
	uint16_t buff[10];
	uint16_t write_buff[10];
	for (int i = 0; i < VarParam[0].VarCount; i++)
	{
		if (!strcmp(VarParam[i].VarName, varname))
		{
			varsubscript = i;
			portid = VarParam[i].PortId;
			break;
		}
	}
	for (int i = 0; i < PortInfo[0].portcount; i++)
	{
		if (PortInfo[i].id == portid)
		{
			portsubscript = i;
			break;
		}
	}
	for (int i = 0; i < DevInfo[0].devcount; i++)
	{
		if (VarParam[varsubscript].DevId == DevInfo[i].id)
			devsubscript = i;
	}
	sprintf(serialport, "%s%d", "/dev/ttyS", PortInfo[portsubscript].PortNum);

	char parity = PortInfo[portsubscript].Parity[0];
	mb = modbus_new_rtu(serialport, PortInfo[portsubscript].baud, parity, PortInfo[portsubscript].DataBits, PortInfo[portsubscript].StopBits);
	if (mb == NULL)
	{
		printf("Unable to create the libmodbus context\n");
		return -1;
	}
//#define modbus_debug
#ifdef modbus_debug
	modbus_set_debug(mb, 1);
#endif
	modbus_set_slave(mb, DevInfo[devsubscript].address);
#ifndef gcc
	int ret = -1;
	while (ret == -1 && PortInfo[portsubscript].gpio != -1)
	{
		ret = modbus_rtu_set_rts(mb, MODBUS_RTU_RTS_UP);
	}
	ret = -1;
	while (ret == -1 && PortInfo[portsubscript].gpio != -1)
	{
		ret = modbus_rtu_set_custom_rts(mb, setrts);
	}
#endif
	if (modbus_connect(mb) == -1)
	{
		printf("Modbus Connection failed\n");
		modbus_free(mb);
		//	    return -1;
	}
	struct timeval t;
	t.tv_sec = 0;
	t.tv_usec = 500000;
	modbus_set_response_timeout(mb, t.tv_sec, t.tv_usec);
	modbus_set_byte_timeout(mb, t.tv_sec, t.tv_usec);
	if (!write)
		memset(buff, 0, sizeof(buff));
	int res = -1;
	int count = 0;
	int regnums = 1;

	int regdian = DevInfo[devsubscript].regedian;
	int byteorder = DevInfo[devsubscript].byteorder;

//	regdian = 1;
//	byteorder = 1;
	//	strcpy(VarParam[varsubscript].DataType, "int32");
	if (write)
	{
		cout << "inputdata is " << inputdata << endl;
		memset(write_buff, 0, sizeof(write_buff));
		MODBUS_SET_INT64_TO_INT16(write_buff, 0, (int64_t)inputdata);
	}
	if (!strcmp("uint16", VarParam[varsubscript].DataType))
	{
		datatype = uint16;
		regnums = 1;
		write_buff[0] = write_buff[3];
	}
	else if (!strcmp("uint32", VarParam[varsubscript].DataType))
	{
		datatype = uint32;
		regnums = 2;
		write_buff[0] = write_buff[2];
		write_buff[1] = write_buff[3];
	}
	else if (!strcmp("uint64", VarParam[varsubscript].DataType))
	{
		datatype = uint64;
		regnums = 4;
	}
	else if (!strcmp("int16", VarParam[varsubscript].DataType))
	{
		datatype = int16;
		regnums = 1;
		write_buff[0] = write_buff[3];
	}
	else if (!strcmp("int32", VarParam[varsubscript].DataType))
	{
		datatype = int32;
		regnums = 2;
		write_buff[0] = write_buff[2];
		write_buff[1] = write_buff[3];
	}
	else if (!strcmp("int64", VarParam[varsubscript].DataType))
	{
		datatype = int64;
		regnums = 4;
	}
	else if (!strcmp("double", VarParam[varsubscript].DataType))
	{
		datatype = double_type;
		regnums = 4;
		modbus_set_double_to_int16(write_buff, inputdata);
	}
	else if (!strcmp("float", VarParam[varsubscript].DataType))
	{
		datatype = float_type;
		regnums = 2;
		floatunion floattype;
		floattype.floattype = inputdata;
		modbus_set_float_to_int16(write_buff, floattype.floattype);
	}
	if(datatype != int16 && datatype != uint16 )
		ByteChange(write_buff, regnums, regdian, byteorder);

	while (res == -1)
	{
		count++;
		if (count >= 5)
			break;

		if (!strcmp("B0", VarParam[varsubscript].RegType))
		{
			if (!write)
				res = modbus_read_bits(mb, VarParam[varsubscript].RegAdr, 1, (uint8_t*)buff);
			else
				res = modbus_write_bit(mb, VarParam[varsubscript].RegAdr, inputdata);
		}
		else if (!strcmp("B1", VarParam[varsubscript].RegType))
		{
			if (!write)
				res = modbus_read_input_bits(mb, VarParam[varsubscript].RegAdr, 1, (uint8_t*)buff);
		}
		else if (!strcmp("W3", VarParam[varsubscript].RegType))
		{
			if (!write)
				res = modbus_read_registers(mb, VarParam[varsubscript].RegAdr, regnums, (uint16_t*)buff);
			else
				res = modbus_write_registers(mb, VarParam[varsubscript].RegAdr, regnums, write_buff);

		}
		else if (!strcmp("W4", VarParam[varsubscript].RegType))
		{
			if (!write)
				res = modbus_read_input_registers(mb, VarParam[varsubscript].RegAdr, regnums, (uint16_t*)buff);
		}
	}

	if (write)
		return 0;
	if (regnums == 1)
	{
		if (datatype == uint16)
		{
			*buff_dest = ((uint16_t)buff[0]) * VarParam[varsubscript].modules;
		}
		else
		{
			*buff_dest = ((int16_t)buff[0]) * VarParam[varsubscript].modules;
		}
	}
	else
	{
		ByteChange(buff, regnums, regdian, byteorder);

		if (datatype == uint32)
		{
			*buff_dest = (uint32_t)MODBUS_GET_INT32_FROM_INT16(buff, 0);
		}
		else if (datatype == uint64)
		{
			*buff_dest = (uint64_t)MODBUS_GET_INT64_FROM_INT16(buff, 0);
		}
		else if (datatype == int32)
		{
			*buff_dest = (int32_t)MODBUS_GET_INT32_FROM_INT16(buff, 0);
		}
		else if (datatype == int64)
		{
			*buff_dest = (int64_t)MODBUS_GET_INT64_FROM_INT16(buff, 0);
		}
		else if (datatype == float_type)
		{
			*buff_dest = modbus_get_float_from_int16(buff);
		}
		else if (datatype == double_type)
		{
			*buff_dest = modbus_get_double_from_int16(buff);
		}
	}
	modbus_close(mb);
	modbus_free(mb);
	return 0;
}
//#define modbus_thread
#ifdef modbus_thread
int modbus_read_thread(void* Params)
{
	modbus_t* mb;
	pthread_mutex_t modbus_mutex;
	int portid = 0;
	char serialport[20];
	uint16_t buff[10];
	uint16_t write_buff[10];

	for (int i = 0; i < VarParam[0].VarCount; i++)
	{
		if (!strcmp(VarParam[i].VarName, varname))
		{
			varsubscript = i;
			portid = VarParam[i].PortId;
			break;
		}
	}
	for (int i = 0; i < PortInfo[0].portcount; i++)
	{
		if (PortInfo[i].id == portid)
		{
			portsubscript = i;
			break;
		}
	}
	for (int i = 0; i < DevInfo[0].devcount; i++)
	{
		if (VarParam[varsubscript].DevId == DevInfo[i].id)
			devsubscript = i;
	}
	sprintf(serialport, "%s%d", "/dev/ttyS", PortInfo[portsubscript].PortNum);

	char parity = PortInfo[portsubscript].Parity[0];
	mb = modbus_new_rtu(serialport, PortInfo[portsubscript].baud, parity, PortInfo[portsubscript].DataBits, PortInfo[portsubscript].StopBits);
	if (mb == NULL)
	{
		printf("Unable to create the libmodbus context\n");
		return -1;
	}
//#define modbus_debug
#ifdef modbus_debug
	modbus_set_debug(mb, 1);
#endif
	modbus_set_slave(mb, DevInfo[devsubscript].address);
#ifndef gcc
	int ret = -1;
	while (ret == -1 && PortInfo[portsubscript].gpio != -1)
	{
		ret = modbus_rtu_set_rts(mb, MODBUS_RTU_RTS_UP);
	}
	ret = -1;
	while (ret == -1 && PortInfo[portsubscript].gpio != -1)
	{
		ret = modbus_rtu_set_custom_rts(mb, setrts);
	}
#endif
	if (modbus_connect(mb) == -1)
	{
		printf("Modbus Connection failed\n");
		modbus_free(mb);
		//	    return -1;
	}
	struct timeval t;
	t.tv_sec = 0;
	t.tv_usec = 500000;
	modbus_set_response_timeout(mb, t.tv_sec, t.tv_usec);
	modbus_set_byte_timeout(mb, t.tv_sec, t.tv_usec);
	if (!write)
		memset(buff, 0, sizeof(buff));
	int res = -1;
	int count = 0;
	int regnums = 1;

	int regdian = DevInfo[devsubscript].regedian;
	int byteorder = DevInfo[devsubscript].byteorder;

//	regdian = 1;
//	byteorder = 1;
	//	strcpy(VarParam[varsubscript].DataType, "int32");
	if (write)
	{
		cout << "inputdata is " << inputdata << endl;
		memset(write_buff, 0, sizeof(write_buff));
		MODBUS_SET_INT64_TO_INT16(write_buff, 0, (int64_t)inputdata);
	}
	if (!strcmp("uint16", VarParam[varsubscript].DataType))
	{
		datatype = uint16;
		regnums = 1;
		write_buff[0] = write_buff[3];
	}
	else if (!strcmp("uint32", VarParam[varsubscript].DataType))
	{
		datatype = uint32;
		regnums = 2;
		write_buff[0] = write_buff[2];
		write_buff[1] = write_buff[3];
	}
	else if (!strcmp("uint64", VarParam[varsubscript].DataType))
	{
		datatype = uint64;
		regnums = 4;
	}
	else if (!strcmp("int16", VarParam[varsubscript].DataType))
	{
		datatype = int16;
		regnums = 1;
		write_buff[0] = write_buff[3];
	}
	else if (!strcmp("int32", VarParam[varsubscript].DataType))
	{
		datatype = int32;
		regnums = 2;
		write_buff[0] = write_buff[2];
		write_buff[1] = write_buff[3];
	}
	else if (!strcmp("int64", VarParam[varsubscript].DataType))
	{
		datatype = int64;
		regnums = 4;
	}
	else if (!strcmp("double", VarParam[varsubscript].DataType))
	{
		datatype = double_type;
		regnums = 4;
		modbus_set_double_to_int16(write_buff, inputdata);
	}
	else if (!strcmp("float", VarParam[varsubscript].DataType))
	{
		datatype = float_type;
		regnums = 2;
		floatunion floattype;
		floattype.floattype = inputdata;
		modbus_set_float_to_int16(write_buff, floattype.floattype);
	}
	if(datatype != int16 && datatype != uint16 )
		ByteChange(write_buff, regnums, regdian, byteorder);

	while (res == -1)
	{
		count++;
		if (count >= 5)
			break;

		if (!strcmp("B0", VarParam[varsubscript].RegType))
		{
			if (!write)
				res = modbus_read_bits(mb, VarParam[varsubscript].RegAdr, 1, (uint8_t*)buff);
			else
				res = modbus_write_bit(mb, VarParam[varsubscript].RegAdr, inputdata);
		}
		else if (!strcmp("B1", VarParam[varsubscript].RegType))
		{
			if (!write)
				res = modbus_read_input_bits(mb, VarParam[varsubscript].RegAdr, 1, (uint8_t*)buff);
		}
		else if (!strcmp("W3", VarParam[varsubscript].RegType))
		{
			if (!write)
				res = modbus_read_registers(mb, VarParam[varsubscript].RegAdr, regnums, (uint16_t*)buff);
			else
				res = modbus_write_registers(mb, VarParam[varsubscript].RegAdr, regnums, write_buff);

		}
		else if (!strcmp("W4", VarParam[varsubscript].RegType))
		{
			if (!write)
				res = modbus_read_input_registers(mb, VarParam[varsubscript].RegAdr, regnums, (uint16_t*)buff);
		}
	}

	if (write)
		return 0;
	if (regnums == 1)
	{
		if (datatype == uint16)
		{
			*buff_dest = ((uint16_t)buff[0]) * VarParam[varsubscript].modules;
		}
		else
		{
			*buff_dest = ((int16_t)buff[0]) * VarParam[varsubscript].modules;
		}
	}
	else
	{
		ByteChange(buff, regnums, regdian, byteorder);

		if (datatype == uint32)
		{
			*buff_dest = (uint32_t)MODBUS_GET_INT32_FROM_INT16(buff, 0);
		}
		else if (datatype == uint64)
		{
			*buff_dest = (uint64_t)MODBUS_GET_INT64_FROM_INT16(buff, 0);
		}
		else if (datatype == int32)
		{
			*buff_dest = (int32_t)MODBUS_GET_INT32_FROM_INT16(buff, 0);
		}
		else if (datatype == int64)
		{
			*buff_dest = (int64_t)MODBUS_GET_INT64_FROM_INT16(buff, 0);
		}
		else if (datatype == float_type)
		{
			*buff_dest = modbus_get_float_from_int16(buff);
		}
		else if (datatype == double_type)
		{
			*buff_dest = modbus_get_double_from_int16(buff);
		}
	}
	varinfo[varinfo[0].count].varname(VarParam[varsubscript].VarName);
	varinfo[varinfo[0].count].value = *buff_dest;
	varinfo[0].count++;
	modbus_close(mb);
	modbus_free(mb);
	return 0;
}
#endif
