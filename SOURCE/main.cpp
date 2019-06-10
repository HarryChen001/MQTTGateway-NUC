#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <chrono>
#include <unistd.h>
#include <thread>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include "modbus/modbus-rtu.h"

#include "libserial/Serial.h"

#include "sqlite3pp/sqlite3pp.h"

#include "soft_sqlite3pp.h"

#include "MyData.h"

//#include "soft_mqtt.h"
#include "soft_aliyunmqtt.h"

#include "soft_mymodbus.h"
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

int main(int argc, char* argv[])
{
#ifdef modbus_tcp
#define
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

/*	system("echo 13 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio13/direction");
	while(1)
	{
		system("echo 1 > /sys/class/gpio/gpio13/value");
		sleep(1);
		system("echo 0 > /sys/class/gpio/gpio13/value");
		sleep(1);
	}*/
/*	Serial s;
	char dev[20];
//	sprintf(dev,"%s%s","/dev/ttyS",argv[1]);
	if (s.open(argv[1], 115200, 8,'N', 1) != Serial::OK)
	{
		printf("Cannot open serial port!\n");
		return -1;
	}
	char buff[1024];
	while(1)
	{
		cin.getline(buff,1024);
		strcat(buff,"\r\n");

		system("echo 1 > /sys/class/gpio/gpio46/value");
		s.write(buff,strlen(buff));
		system("echo 0 > /sys/class/gpio/gpio46/value");
	}*/
	MySqlite db(argv[1]);
	db.GetAllInfo();

	MyAliyunMqtt Mqtt;
	Mqtt.openthread();

/***********************************���Դ���**************************************/
/*	Serial s;
	if (s.open("/dev/ttyS0", 115200, 8,'N', 1) != Serial::OK)
	{
		printf("Cannot open serial port!\n");
		return -1;
	}
	char test[] = "test data";
	s.write(test, strlen(test));
	s.close();*/
/***********************************���Դ���**************************************/
	char input;
	while (1)
	{
		sleep(1);
	}
	return 0;
}
