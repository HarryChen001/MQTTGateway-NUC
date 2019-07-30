#include <iostream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>	//sleep()
#include <thread>
#include <queue>
#include <mutex>

#include "modbus/modbus-rtu.h"

#include "libserial/Serial.h"

#include "sqlite3pp/sqlite3pp.h"

#include "soft_sqlite3pp.h"

#include "MyData.h"

#include "soft_aliyunmqtt.h"

#include "soft_mymodbus.h"

#include "base64/base64.h"

#include "soft_myfunction.h"

#include <signal.h>
using std::cin;
using std::cout;
using std::endl;
using std::string;

ConnectInfo_t MqttInfo[MqttConnect];
DeviceInfo_t DevInfo[DeviceNums];
ThemeCtrl_t ThemeCtrl[ThemeCtrlNums];
ThemeUpload_t ThemeUpload[ThemeUploadNums];
VarParam_t VarParam[AllVarNums];

enumdatatype datatype;
Allinfo_t Allinfo[20];
Varinfo_t varinfo;

std::map<std::string, double>var;
std::queue<Varinfo_t>queue_var_write;

MyAliyunMqtt AliyunMqtt;

#define BASE64_ENCODE_TEST
#define BASE64_DECODE_TEST
#define SERIAL_TEST
#define BASE64_ENCODE_NUM_TEST

void sign_handle(int signum)
{
	cout << "Serivce Stop!" << endl;
	exit(0);
}

int main(int argc, char* argv[])
{
	signal(SIGINT, sign_handle);
	if (argc <= 1)
	{
		cout << "no Params" << endl;
		return 0;
	}
#ifdef BASE64_ENCODE_TEST
	if (!strcmp("base64encode", argv[1]))
		while (1)
		{
			char buff[1024];
			cin.getline(buff, 1024);
			int base64data_length = strlen(buff);
			int encoded_data_length = Base64encode_len(base64data_length);
			char* base64_string = (char*)malloc(encoded_data_length);
			Base64encode(base64_string, buff, base64data_length);
			cout << base64_string << endl;
			free(base64_string);
		}
#endif
#ifdef BASE64_DECODE_TEST
	if (!strcmp("base64decode", argv[1]))
		while (1)
		{
			char buff[1024];
			char deststring[1024];
			cin.getline(buff, 1024);
			Base64decode(deststring, buff);
			cout << deststring << endl;
		}
#endif
#ifdef BASE64_DECODE_TEST
	if (!strcmp("base64decodenums", argv[1]))
		while (1)
		{
			char buff[1024];
			char deststring[1024];
			cin.getline(buff, 1024);
			Base64decode(deststring, buff);
			for (int i = 0; i < 8; i++)
			{
				printf("%d ", deststring[i]);
			}
		}
#endif
#ifdef BASE64_ENCODE_NUM_TEST
	if (!strcmp("base64encodenums", argv[1]))
	{
		while (1)
		{
			int nums[1024];
			char buff[1024];
			for (int i = 0; i < 8; i++)
			{
				cin >> nums[i];
				buff[i] = nums[i];
			}
			int base64data_length = 8 * sizeof(char);
			int encoded_data_length = Base64encode_len(base64data_length);
			char* base64_string = (char*)malloc(encoded_data_length);
			Base64encode(base64_string, buff, base64data_length);
			cout << base64_string << endl;
			free(base64_string);
		}
	}
#endif

#ifdef modbus_tcp
	modbus_t* mb;
	mb = modbus_new_tcp("192.168.1.23", 1502);
	modbus_set_slave(mb, 1);
	modbus_set_debug(mb, 1);

	while (modbus_connect(mb) != 0);

	while (1)
	{
		uint16_t buff[100];
		int count = 0;
		int rc = -1;
		while (rc < 0)
		{
			rc = modbus_read_registers(mb, 0, 20, buff);
			count++;
			if (count >= 6)
			{
				break;
			}
			//	sleep(1);
		}
		for (int i = 0; i < rc; i++)
		{
			cout << buff[i] << endl;
		}
		cout << endl;
		sleep(1);
	}
#endif

#ifdef SERIAL_TEST
	if (!strcmp("serialdebug", argv[1]))
	{
		Serial s;
		if (s.open(argv[2], 115200, 8, 'N', 1) != Serial::OK)
		{
			printf("Cannot open serial port!\n");
			return -1;
		}
		s.openreadthread();
		char buff[1024];
		while (1)
		{
			cin.getline(buff, 1024);
			strcat(buff, "\r\n");
			s.write(buff, strlen(buff));
		}
	}
#endif
	std::string str = argv[1];
	if (str.find("conf", 0) == 0)
	{
		cout << "no conf" << endl;
		return 0;
	}
	MySqlite db(argv[1]);
	db.GetAllInfo();
	db.~MySqlite();

	for (int i = 0; i < 20; i++)
	{
		if (Allinfo[i].portinfo.id == 0)
		{
			continue;
		}
		for (int j = 0; j < Allinfo[i].devcount; j++)
		{
			DeviceInfo_t* devtemp = &Allinfo[i].deviceinfo[j];
			if (devtemp->id == 0)
			{
				continue;
			}
			for (int k = 0; k < devtemp->allvarcount; k++)
			{
				VarParam_t* vartemp = &devtemp->allvarparams[k];
				if (vartemp->id == 0)
				{
					continue;
				}
			//	cout << devtemp->address << endl << vartemp->VarName << endl;
			}
		}
	}
	modbus newmodbus;
	newmodbus.openmainthread();

//	sleep(5);
//	MyAliyunMqtt Mqtt;
	AliyunMqtt.openmainthread();

	while (1)
	{
		char input;
		std::cin >> input;
		if (input == 'q' || input == 'Q')
		{
			return -1;
		}
		sleep(1);
	}
	return 0;
}
