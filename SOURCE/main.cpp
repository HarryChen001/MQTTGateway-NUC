#include <iostream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <unistd.h>	//sleep()
#include <thread>
#include <queue>
#include <mutex>

#include "modbus/modbus-rtu.h"

#include "libserial/Serial.h"

#include "sqlite3pp/sqlite3pp.h"

#include "base64/base64.h"

#include "soft_sqlite3pp.h"

#include "MyData.h"

#include "soft_aliyunmqtt.h"

#include "soft_mymodbus.h"

#include "soft_myfunction.h"

#include <signal.h>

#include "glog/logging.h"

#include "sdk_include.h"

#include "infra_cjson.h"

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

std::map<std::string, double> var;
std::queue<Varinfo_t> queue_var_write;
std::queue<MessageInfo_t> queueMessageInfo;

#define Version "2019/11/22"

#define BASE64_ENCODE_TEST
#define BASE64_DECODE_TEST
#define SERIAL_TEST
#define BASE64_ENCODE_NUM_TEST

void sign_handle(int signum)
{
	LOG(WARNING)<< "Program stop by User" << endl << endl;
	google::ShutdownGoogleLogging();
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
	LITE_set_loglevel(0);

	FLAGS_logtostderr = 1;					//设置日志消息是否转到标准输出而不是日志文件
	FLAGS_alsologtostderr = 0;				//设置日志消息除了日志文件之外是否去标准输出
	FLAGS_colorlogtostderr = 1;				//设置记录到标准输出的颜色消息（如果终端支持）
	FLAGS_log_prefix = 1;					//设置日志前缀是否应该添加到每行输出
	FLAGS_logbufsecs = 0;					//设置可以缓冲日志的最大秒数，0指实时输出
	FLAGS_max_log_size = 10;				//设置最大日志文件大小（以MB为单位）
	FLAGS_stop_logging_if_full_disk = true;	//设置是否在磁盘已满时避免日志记录到磁盘
	FLAGS_log_dir = "./logdir";

	google::InitGoogleLogging("LOG");
	google::SetLogDestination(google::GLOG_INFO, "LOG");//设置特定严重级别的日志的输出目录和前缀。第一个参数为日志级别，第二个参数表示输出目录及日志文件名前缀
	google::SetLogFilenameExtension("");		//在日志文件名中级别后添加一个扩展名。适用于所有严重级别
	google::SetStderrLogging(google::INFO);				//大于指定级别的日志都输出到标准输出

	LOG(INFO)<< "Start Work! Version :" << Version << endl << endl;
#ifdef BASE64_ENCODE_TEST
	if (!strcmp("base64encode", argv[1]))
		while (1)
		{
			char buff[1024];
			cin.getline(buff, 1024);
			int base64data_length = strlen(buff);
			int encoded_data_length = Base64encode_len(base64data_length);
			char* base64_string = (char*) malloc(encoded_data_length);
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
			char* base64_string = (char*) malloc(encoded_data_length);
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
	if (str.find("conf", 0) == string::npos)
	{
		LOG(FATAL)<< "NO CONF!" << endl;
	}
	MySqlite db(argv[1]);
	db.GetAllInfo();
	db.~MySqlite();

	LOG(INFO)<<"Got all sqlite" << endl;
	modbus newmodbus;
	newmodbus.openmainthread();

	MyAliyunMqtt Mqtt;

	for (int i = 0; i < MqttInfo[0].MqttCount; i++)
	{
		if (!MqttInfo[i].Enable)
			continue;
		char* host = MqttInfo[i].ServerLink;
		char* clientid = MqttInfo[i].ClientId;
		char* username = MqttInfo[i].UserName;
		char* password = MqttInfo[i].Password;
		int port = MqttInfo[i].ServerPort;

		MqttInfo[i].client = Mqtt.MqttInit(host, port, clientid, username,
				password);

		for (int j = 0; j < ThemeCtrl[0].Ctrlcount; j++)
		{
			if (ThemeCtrl[j].MqttId == MqttInfo[i].id)
			{
				ThemeCtrl[j].client = MqttInfo[i].client;
			}
		}
		for (int j = 0; j < ThemeUpload[0].UploadThemeqCount; j++)
		{
			if (ThemeUpload[j].MqttId == MqttInfo[i].id)
			{
				ThemeUpload[j].client = MqttInfo[i].client;
			}
		}
		if (strstr(host, "aliyuncs") != NULL)
		{
			std::string temp = username;
			size_t pos = temp.find('&', 0);
			MqttInfo[i].DeviceName = temp.substr(0, pos);
			MqttInfo[i].ProductName = temp.substr(pos + 1);
			string rrpc_topic = "/sys/" + MqttInfo[i].ProductName + "/"
					+ MqttInfo[i].DeviceName + "/rrpc/request/+";
			Mqtt.subscribe(ThemeCtrl[i].client, (char*) rrpc_topic.c_str(), 0);
		}
	}
	for (int i = 0; i < ThemeCtrl[0].Ctrlcount; i++)
	{
		if (!ThemeCtrl[i].Enable)
			continue;
		Mqtt.subscribe(ThemeCtrl[i].client, ThemeCtrl[i].CtrlSub, 0);
	}
	Mqtt.openmainthread();
	while (1)
	{
		char input;
		std::cin >> input;
		if (input == 'q' || input == 'Q')
		{
			Mqtt.~MyAliyunMqtt();
			google::ShutdownGoogleLogging();
			return -1;
		}
		usleep(1000);
	}
	return 0;
}
