#include "soft_aliyunmqtt.h"

#include <iostream>
#include <string>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <queue>

#include "sdk_include.h"

#include "MyData.h"
#include "soft_mymodbus.h"
#include "libserial/Serial.h"
#include "base64/base64.h"
#include "cJSON/cJSON.h"
#include "soft_myfunction.h"

#include "glog/logging.h"
#include "glog/log_severity.h"

#include "iotx_mqtt_client.h"

using std::cout;
using std::endl;
using std::string;
using std::map;
using std::queue;

void MyAliyunMqtt::message_arrive(void* pcontext, void* pclient,
		iotx_mqtt_event_msg_pt msg)
{
	MyAliyunMqtt* pointer = (MyAliyunMqtt*) pcontext;
	iotx_mqtt_topic_info_t* topic_info = (iotx_mqtt_topic_info_pt) msg->msg;
	switch (msg->event_type)
	{
	case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
	{

		MessageInfo_t temp;
		temp.Message.assign(topic_info->payload, topic_info->payload_len);
		temp.TopicName.assign(topic_info->ptopic, topic_info->topic_len);
		temp.client = pclient;
		queueMessageInfo.push(temp);

		LOG(INFO)<< "GET Message from: " << temp.TopicName << endl << "Message Payload is : " << temp.Message << endl;

		break;
	}
	default:
		break;
	}
}
int MyAliyunMqtt::subscribe(void* client, char* subscribetopic, int Qos)
{
	int res = 0;
	iotx_mqtt_qos_t iotx_qos;
	if (Qos == 0)
		iotx_qos = IOTX_MQTT_QOS0;
	else if (Qos == 1)
		iotx_qos = IOTX_MQTT_QOS1;
	else if (Qos == 2)
		iotx_qos = IOTX_MQTT_QOS2;
	else
	{
		printf("Unsupport Qos Setting!\n");
		return -1;
	}
	LOG(INFO)<< "Subscribe Topic is: " << subscribetopic << endl << endl;
	res = IOT_MQTT_Subscribe(client, subscribetopic, iotx_qos, message_arrive,
			this);
	if (res < 0)
	{
		LOG(WARNING)<< "Subscribe Topic Fail" << endl << endl;
		cout << BOLDRED << "Subscribe Topic fail!" << RESET << endl;
	}
	else
	{
		LOG(INFO) << "Subscribe Topic SUCCESS!" << endl << endl;
	}
	return 0;
}
int MyAliyunMqtt::publish(void* client, char* publishtopic, int Qos,
		char* json_payload)
{
	int res = -1;
	iotx_mqtt_topic_info_t topic_msg;
	topic_msg.qos = Qos;
	topic_msg.retain = 0;
	topic_msg.dup = 0;
	topic_msg.payload = json_payload;
	topic_msg.payload_len = strlen(json_payload);
	res = IOT_MQTT_Publish(client, publishtopic, &topic_msg);

	LOG(INFO)<< "Publish to topic : " << publishtopic << endl << endl;
	if (res < 0)
	{
		LOG(WARNING)<< "FAILED to publish.Payload is : " << json_payload << endl << endl;

		return -1;
	}
	LOG(INFO)<< "SUCCESS! Payload is ->" << json_payload << endl << endl;

	return 0;
}
int MyAliyunMqtt::publish_cjson(void* client, char* publishtopic, int Qos,
		cJSON* json_payload)
{
	int res = -1;
	iotx_mqtt_topic_info_t topic_msg;
	char* publish_payload = (char*) malloc(
			strlen(cJSON_Print(json_payload)) + 1);

	publish_payload = cJSON_PrintUnformatted(json_payload);
	topic_msg.qos = Qos;
	topic_msg.retain = 0;
	topic_msg.dup = 0;
	topic_msg.payload = publish_payload;
	topic_msg.payload_len = strlen(publish_payload);
	res = IOT_MQTT_Publish(client, publishtopic, &topic_msg);

	LOG(INFO)<< "Publish to topic : " << publishtopic << endl << endl;
	if (res < 0)
	{
		LOG(WARNING)<< "FAILED to publish.Payload is : " << publish_payload << endl << endl;
		free(publish_payload);
		return -1;
	}
	LOG(INFO)<< "SUCCESS! Payload is ->" << publish_payload << endl << endl;
	free(publish_payload);
	return 0;
}
void MyAliyunMqtt::event_handle(void* pcontext, void* pclient,
		iotx_mqtt_event_msg_pt msg)
{
}

/*
 *  NOTE: About demo topic of /${productKey}/${deviceName}/get
 *
 *  The demo device has been configured in IoT console (https://iot.console.aliyun.com)
 *  so that its /${productKey}/${deviceName}/get can both be subscribed and published
 *
 *  We design this to completely demonstrate publish & subscribe process, in this way
 *  MQTT client can receive original packet sent by itself
 *
 *  For new devices created by yourself, pub/sub privilege also requires being granted
 *  to its /${productKey}/${deviceName}/get for successfully running whole example
 */
MyAliyunMqtt::~MyAliyunMqtt()
{
	threadid.~thread();
	threadid_interval.~thread();
	threadid_recparse.~thread();
}
void* MyAliyunMqtt::MqttInit(char* host, int port, char* clientid,
		char* username, char* password)
{
	iotx_mqtt_param_t mqtt_params;
	memset(&mqtt_params, 0x0, sizeof(mqtt_params));
	mqtt_params.write_buf_size = 2048;
	mqtt_params.read_buf_size = 2048;
	mqtt_params.host = host;
	mqtt_params.client_id = clientid;
	mqtt_params.password = password;
	mqtt_params.username = username;
	mqtt_params.keepalive_interval_ms = 60000;
	mqtt_params.request_timeout_ms = 20000;
	mqtt_params.port = port;
	mqtt_params.clean_session = 1;
	mqtt_params.handle_event.h_fp = event_handle;
	mqtt_params.handle_event.pcontext = NULL;
	void* client = IOT_MQTT_Construct(&mqtt_params);
	if (NULL == client)
	{
		printf("MQTT construct failed\n");
		exit(-1);
	}
	return client;
}
MyAliyunMqtt::MyAliyunMqtt()
{
}
int MyAliyunMqtt::MqttInterval(void* Params)
{
	while (1)
	{
		for (int i = 0; i < MqttInfo[0].MqttCount; i++)
		{
			if (MqttInfo[i].client != NULL)
			{
				IOT_MQTT_Yield(MqttInfo[i].client, 10);
			}
		}
	}
	return 0;
}
int MyAliyunMqtt::MqttRecParse(void* Params)
{
	MyAliyunMqtt* point = (MyAliyunMqtt*) Params;
	Serial s;
	int baud = 0;
	int databits = 0;
	char parity = 0;
	int stopbits = 0;
	while (1)
	{
		if (!queueMessageInfo.empty())
		{
			cout << endl;
		}
		while (!queueMessageInfo.empty())
		{
			cJSON* json, *json_params;
			void* client = queueMessageInfo.front().client;
			string jsonPayload = queueMessageInfo.front().Message;
			string topicname = queueMessageInfo.front().TopicName;
			queueMessageInfo.pop();
			json = cJSON_Parse(jsonPayload.c_str());

			size_t pos = topicname.find("request", 0);
			if (pos != string::npos)
				varinfo.topicname = topicname.substr(0, pos) + "response"
						+ topicname.substr(pos + strlen("request"));
			else
			{
				for (int i = 0; i < ThemeCtrl[0].Ctrlcount; i++)
				{
					strstr(ThemeCtrl[i].CtrlSub, topicname.c_str());
					varinfo.topicname = ThemeCtrl[i].CtrlPub;
					break;
				}
			}
			json_params = cJSON_GetObjectItem(json, "params");
			if (json_params)
			{
				cJSON* jsontemp = json_params->child;
				while (jsontemp != NULL)
				{
					varinfo.varname = jsontemp->string;
					varinfo.value = jsontemp->valuedouble;
					varinfo.client = client;

					queue_var_write.push(varinfo);
					jsontemp = jsontemp->next;
				}
			}
			else if ((json_params = cJSON_GetObjectItem(json, "COM")))
			{
				int com = json_params->valueint;
				char dev[100];
				char dest[1024];
				char readbuff[1024];
				Allinfo_t* pallinfotemp;
				int nums = sizeof(Allinfo) / sizeof(Allinfo_t);
				for (int i = 0; i < nums; i++)
				{
					int portnums = Allinfo[i].portinfo.PortNum;

					if (portnums == 10)
						portnums = 1;
					else if (portnums == 3)
						portnums = 2;
					else if (portnums == 9)
						portnums = 3;
					else if (portnums == 1)
						portnums = 4;
					else if (portnums == 6)
						portnums = 5;
					else if (portnums == 7)
						portnums = 6;
					else if (portnums == 2)
						portnums = 7;
					else if (portnums == 8)
						portnums = 8;
					else if (portnums == 4)
						portnums = 9;

					if (com == portnums)
					{
						baud = Allinfo[i].portinfo.baud;
						databits = Allinfo[i].portinfo.DataBits;
						stopbits = Allinfo[i].portinfo.StopBits;
						parity = Allinfo[i].portinfo.Parity[0];
						pallinfotemp = &Allinfo[i];
						pallinfotemp->write_flag = true;
//						Allinfo[i].write_flag = true;
						cout << Allinfo[i].portinfo.PortNum << endl;
						break;
					}
				}
#ifndef gcc
				int gpio = (com == 1) ? 46 : (com == 2) ? 103 :
							(com == 3) ? 102 : (com == 4) ? 132 :
							(com == 5) ? 32 : -1;
				com = (com == 1) ? 10 : (com == 2) ? 3 : (com == 3) ? 9 :
						(com == 4) ? 1 : (com == 5) ? 6 : com;
				if (gpio != -1)
				{
					gpioexport(gpio);
					gpiooutput(gpio);
					gpiovalue(gpio, 1);
				}
#endif
				cJSON* json_payload;

				json_payload = cJSON_GetObjectItem(json, "payload");
				int ComIsResp = cJSON_GetObjectItem(json, "ComIsResp")->valueint;
				int serialtimeout = 0;
				if (ComIsResp)
					serialtimeout =
							cJSON_GetObjectItem(json, "ComTimeoutMs")->valueint;
				sprintf(dev, "%s%d", "/dev/ttyS", com);

				int len = Base64decode(dest, json_payload->valuestring);

				s.open(dev, baud, databits, parity, stopbits);
				s.write(dest, len);
#ifndef gcc
				if (gpio != -1)
				{
					usleep(10000);
					pallinfotemp->write_flag = false;
					gpiovalue(gpio, 0);
				}
#endif
				int seriallen = s.read_wait(readbuff, 1024, serialtimeout);
				if (seriallen == 0)
				{
					cout << "read serial timeout!" << endl;
				}
				memset(dest, 0, sizeof(dest));
				Base64encode(dest, readbuff, seriallen);
				s.close();

				cJSON* response;
				time_t nowtime;
				char tmp[64];
				char payload[1024];
				nowtime = time(NULL); //get now time
				strftime(tmp, sizeof(tmp), "%Y%m%d%H%M%S", localtime(&nowtime));

				response = cJSON_CreateObject();
				cJSON_AddNumberToObject(response, "COM", com);

				if (!ComIsResp)
				{
					sprintf(payload, "Com%d Complete", com);
				}
				else
					strcpy(payload, dest);

				if (seriallen == 0)
				{
					sprintf(payload, "Com%d Timeout", com);
				}

				cJSON_AddStringToObject(response, "Time", tmp);
				cJSON_AddStringToObject(response, "payload", payload);
				int Qos = 0;
				if (topicname.find("/respone/") == string::npos)
					Qos = 1;
				point->publish(client, (char*) varinfo.topicname.c_str(), Qos,
						payload);

				cJSON_Delete(response);
			}
		}
		usleep(1000);
	}
	return 0;
}
int MyAliyunMqtt::openmainthread()
{
	threadid = std::thread(MqttMain, this);
	return 0;
}
int MyAliyunMqtt::openintervalthread()
{
	threadid_interval = std::thread(MqttInterval, this);
	return 0;
}
int MyAliyunMqtt::openrecparsethread()
{
	threadid_recparse = std::thread(MqttRecParse, this);
	return 0;
}
int MyAliyunMqtt::openuploadthread()
{
	for (int i = 0; i < ThemeUpload[0].UploadThemeqCount; i++)
	{
		threadid_uploadtheme[i] = std::thread(MqttUpload, this,
				&ThemeUpload[i]);
	}
	return 0;
}
int MyAliyunMqtt::MqttMain(void* Params)
{
#ifndef gcc
	for (unsigned int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
	{
		if (Allinfo[i].portinfo.PortNum == 0 || Allinfo[i].portinfo.gpio == -1)
			continue;
		int gpio = Allinfo[i].portinfo.gpio;
		if (gpio != -1)
		{
			char buff[100];
			sprintf(buff, "%s%d%s", "echo ", gpio, " > /sys/class/gpio/export");
			system(buff);
			sprintf(buff, "%s%d%s", "echo out > /sys/class/gpio/gpio", gpio,
					"/direction");
			system(buff);
			sprintf(buff, "%s%d%s", "echo 1 > /sys/class/gpio/gpio", gpio,
					"/value");
			system(buff);
		}
	}
#endif
	MyAliyunMqtt* point = (MyAliyunMqtt*) Params;

	point->openintervalthread();		//create MQTT interval thread
	point->openrecparsethread();	//create thread----parse the receive data
	point->openuploadthread();

	while (1)
		;
	time_t nowtime;
	cJSON* publish_json;
	cJSON* params_json;
	char tmp[64];
	while (1)
	{
		nowtime = time(NULL); //get now time
		strftime(tmp, sizeof(tmp), "%Y%m%d%H%M%S", localtime(&nowtime));

		publish_json = cJSON_CreateObject();

		cJSON_AddStringToObject(publish_json, "id", tmp);
		cJSON_AddStringToObject(publish_json, "method",
				"method.event.property.post");
		cJSON_AddItemToObject(publish_json, "params", params_json =
				cJSON_CreateObject());

		int alluploadvarcount_temp = 0;	//记录已经上传的变量的数量

		for (int i = 0; i < ThemeUpload[0].UploadThemeqCount; i++)
		{
			for (int j = 0; j < ThemeUpload[i].varcount; j++)
			{
				if (ThemeUpload[i].Enable == 0)
					continue;
				string varname = ThemeUpload[i].varname[j];
				if (varname.empty())
					continue;
				double value = var[varname];
				cJSON_AddNumberToObject(params_json, varname.c_str(), value);
				alluploadvarcount_temp++;
				//检查变量数量是否到达设定的数量或者该设备下已无可读变量，是则上发数据到指定主题
				if ((alluploadvarcount_temp % uploadperiod)
						&& alluploadvarcount_temp != ThemeUpload[i].varcount)
				{
					continue;
				}
				alluploadvarcount_temp = 0;
				point->publish(ThemeUpload[i].client, ThemeUpload[i].CtrlPub,
						ThemeUpload[i].QosPub,
						cJSON_PrintUnformatted(publish_json));

				cJSON_DeleteItemFromObject(publish_json, "params");
				cJSON_AddItemToObject(publish_json, "params", params_json =
						cJSON_CreateObject());
			}
		}
		for (int i = 0; i < ThemeUpload[0].UploadThemeqCount; i++)
		{
			for (int j = 0; j < ThemeUpload[i].varcount; j++)
			{
				if (ThemeUpload[i].Enable == 0)
					continue;
				string varname = ThemeUpload[i].varname[j];
				if (varname.empty())
					continue;
				var[varname] = 0;
			}
		}
		sleep(ThemeUpload[0].PubPeriod);
	}
}
int MyAliyunMqtt::MqttUpload(void* Param, void* ThemeUpload)
{
	MyAliyunMqtt* point = (MyAliyunMqtt*) Param;
	ThemeUpload_t* ThemeUploadParam = (ThemeUpload_t*) ThemeUpload;
	time_t nowtime;
	cJSON* publish_json;
	cJSON* params_json;
	char tmp[64];
	while (1)
	{
		nowtime = time(NULL); //get now time
		strftime(tmp, sizeof(tmp), "%Y%m%d%H%M%S", localtime(&nowtime));

		publish_json = cJSON_CreateObject();

		cJSON_AddStringToObject(publish_json, "id", tmp);
		cJSON_AddStringToObject(publish_json, "method",
				"method.event.property.post");
		cJSON_AddItemToObject(publish_json, "params", params_json =
				cJSON_CreateObject());

		int alluploadvarcount_temp = 0;	//记录已经上传的变量的数量
		for (int j = 0; j < ThemeUploadParam->varcount; j++)
		{
			if (ThemeUploadParam->Enable == 0)
				continue;
			string varname = ThemeUploadParam->varname[j];
			if (varname.empty())
				continue;
			double value = var[varname];
			cJSON_AddNumberToObject(params_json, varname.c_str(), value);
			alluploadvarcount_temp++;
			//检查变量数量是否到达设定的数量或者该设备下已无可读变量，是则上发数据到指定主题
			if ((alluploadvarcount_temp % uploadperiod)
					&& alluploadvarcount_temp != ThemeUploadParam->varcount)
			{
				continue;
			}
			alluploadvarcount_temp = 0;
			point->publish(ThemeUploadParam->client, ThemeUploadParam->CtrlPub,
					ThemeUploadParam->QosPub,
					cJSON_PrintUnformatted(publish_json));

			cJSON_DeleteItemFromObject(publish_json, "params");
			cJSON_AddItemToObject(publish_json, "params", params_json =
					cJSON_CreateObject());
		}
		sleep(ThemeUploadParam->PubPeriod);
	}
	return 0;
}
