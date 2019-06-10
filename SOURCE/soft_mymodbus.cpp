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

void setrts(modbus_t *ctx, int on)
{
	char control[100];
	if(on == 1)
	{
		sprintf(control,"%s%d%s","echo 1 > /sys/class/gpio/gpio",PortInfo[portsubscript].gpio,"/value");
		system(control);
	}
	else
	{
		sprintf(control,"%s%d%s","echo 0 > /sys/class/gpio/gpio",PortInfo[portsubscript].gpio,"/value");
		system(control);
	}
}

int modbus_set(int write,int inputdata, char* varname,float* buff_dest)
{
	modbus_t* mb;
	int varsubscript = 0;
	int portid = 0;
	int devsubscript = 0;
	char serialport[20];
	uint16_t buff[10];
	for (int i = 0; i < VarParam[0].VarCount;i++)
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

//	cout << "create modbus ,port is " << serialport << endl;

	char parity = PortInfo[portsubscript].Parity[0];
	mb = modbus_new_rtu(serialport,PortInfo[portsubscript].baud, parity,8 - PortInfo[portsubscript].DataBits,1 - PortInfo[portsubscript].StopBits);
	if (mb == NULL) {
		printf("Unable to create the libmodbus context\n");
		//	    return -1;
	}
#ifdef modbus_debuf
	modbus_set_debug(mb, 1);
#endif
	modbus_set_slave(mb, DevInfo[devsubscript].address);
	int ret = -1;
	while(ret == -1)
	{
		ret = modbus_rtu_set_rts(mb, MODBUS_RTU_RTS_UP);
	}
	ret = -1;
	while(ret == -1)
	{
		ret = modbus_rtu_set_custom_rts(mb,setrts);
	}
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
	modbus_set_byte_timeout(mb,t.tv_sec,t.tv_usec);
	if(!write)
		memset(buff, 0, sizeof(buff));
	int res = -1;
	int count  = 0;
	while(res == -1)
	{
		count++;
		if(count >= 5)
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
			if(!write)
				res = modbus_read_input_bits(mb, VarParam[varsubscript].RegAdr, 1, (uint8_t*)buff);
			else
				res = modbus_write_bit(mb, VarParam[varsubscript].RegAdr, inputdata);
		}
		else if (!strcmp("W3", VarParam[varsubscript].RegType))
		{
			if (!write)
				res = modbus_read_registers(mb, VarParam[varsubscript].RegAdr, 1, (uint16_t*)buff);
			else
				res = modbus_write_register(mb, VarParam[varsubscript].RegAdr, inputdata);

		}
		else if (!strcmp("W4", VarParam[varsubscript].RegType))
		{
			if (!write)
				res = modbus_read_input_registers(mb, VarParam[varsubscript].RegAdr, 1, (uint16_t*)buff);
			else
				res = modbus_write_register(mb, VarParam[varsubscript].RegAdr, inputdata);
		}
	}
//	*buff_dest=modbus_get_float_dcba(&buff[0]);
//	cout << "buff[0] is " << buff[0] << endl;
//	cout << "buff_dest is " << *buff_dest << endl;
	*buff_dest=buff[0]*VarParam[varsubscript].modules;
	uint16_t a[2] = {0x0020,0xf147};
	cout << modbus_get_float_dcba(a) << endl;

	modbus_free(mb);
	modbus_close(mb);
	if(strstr(VarParam[varsubscript].DataType,"float") || strstr(VarParam[varsubscript].DataType,"Double"))
	{
		return 1;
	}
	return 0;
}
