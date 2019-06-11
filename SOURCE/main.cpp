#include <iostream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>	//sleep()
#include <thread>

#include "modbus/modbus-rtu.h"

#include "libserial/Serial.h"

#include "sqlite3pp/sqlite3pp.h"

#include "soft_sqlite3pp.h"

#include "MyData.h"

#include "soft_aliyunmqtt.h"

#include "soft_mymodbus.h"

#include "base64/base64.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

ConnectInfo_t MqttInfo[4];
DeviceInfo_t DevInfo[10];
PortInfo_t PortInfo[10];
ThemeCtrl_t ThemeCtrl[10];
ThemeUpload_t ThemeUpload[10];
ThemeUploadList_t ThemeUploadList[100];
VarParam_t VarParam[100];

#define BASE64_ENCODE_TEST
#define BASE64_DECODE_TEST
#define SERIAL_TEST

int main(int argc, char* argv[])
{
#ifdef BASE64_ENCODE_TEST
	if(!strcmp("base64encode",argv[1]))
		while(1)
		{
			char buff[1024];
			cin.getline(buff,1024);
			int base64data_length = strlen(buff);
			int encoded_data_length = Base64encode_len(base64data_length);
			char* base64_string = (char*)malloc(encoded_data_length);
			Base64encode(base64_string,buff,base64data_length);
			cout << base64_string << endl;
			free(base64_string);
		}
#endif
#ifdef BASE64_DECODE_TEST
	if(!strcmp("base64decode",argv[1]))
		while(1)
		{
			char buff[1024];
			char deststring[1024];
			cin.getline(buff,1024);
			Base64decode(deststring,buff);
			cout << deststring << endl;
		}
#endif

#ifdef modbus_tcp
	modbus_t* mb;
	mb = modbus_new_tcp("192.168.1.23",1502);
	modbus_set_slave(mb,1);
	modbus_set_debug(mb,1);

	while(modbus_connect(mb) != 0);

	while(1)
	{
		uint16_t buff[100];
		int count = 0;
		int rc = -1;
		while(rc < 0)
		{
			rc = modbus_read_registers(mb,0,20,buff);
			count++;
			if(count >= 6)
			{
				break;
			}
		//	sleep(1);
		}
		for(int i = 0 ; i < rc; i++)
		{
			cout << buff[i] << endl;
		}
		cout << endl;
		sleep(1);
	}
#endif

#ifdef SERIAL_TEST
	if(!strcmp("serialdebug",argv[1]))
	{
		Serial s;
		if (s.open(argv[2], 115200, 8,'N', 1) != Serial::OK)
		{
			printf("Cannot open serial port!\n");
			return -1;
		}
		char buff[1024];
		while(1)
		{
			cin.getline(buff,1024);
			strcat(buff,"\r\n");
			s.write(buff, strlen(buff));
		}
	}
#endif
	MySqlite db(argv[1]);
	db.GetAllInfo();

	MyAliyunMqtt Mqtt;
	Mqtt.openmainthread();

	while (1)
	{
		sleep(1);
	}
	return 0;
}
