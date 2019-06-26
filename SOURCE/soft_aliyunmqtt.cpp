#include "soft_aliyunmqtt.h"

#include <iostream>
#include <string>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "mqtt_api.h"
#include "dev_sign_api.h"
#include "MyData.h"
#include "soft_mymodbus.h"
#include "libserial/Serial.h"
#include "base64/base64.h"
//#include "rapidjson/document.h"
//#include "rapidjson/stringbuffer.h"
//#include "rapidjson/writer.h"

#include "cJSON/cJSON.h"

#include "soft_myfunction.h"

using std::cout;
using std::endl;

void MyAliyunMqtt::message_arrive(void* pcontext, void* pclient, iotx_mqtt_event_msg_pt msg)
{
	MyAliyunMqtt* pointer = (MyAliyunMqtt*)pcontext;
	iotx_mqtt_topic_info_t* topic_info = (iotx_mqtt_topic_info_pt)msg->msg;
	switch (msg->event_type) {
	case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
		/* print topic name and topic message */
		printf("Message Arrived: \n");
		printf("Topic  : %.*s\n", topic_info->topic_len, topic_info->ptopic);
		printf("Payload: %.*s\n", topic_info->payload_len, topic_info->payload);
		printf("\n");
		pointer->getmessage = true;
		strcpy(pointer->payload, topic_info->payload);
		break;
	default:
		break;
	}
}
int MyAliyunMqtt::subscribe(char* subscribetopic, int Qos)
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

	res = IOT_MQTT_Subscribe_Sync(pclient, subscribetopic, iotx_qos, message_arrive, this, 1000);
	if (res < 0) {
		printf("subscribe failed\n");
		return -1;
	}
	else
	{
		printf("subscribe success,topic is:[%s]\n", subscribetopic);
	}
	return 0;
}

int MyAliyunMqtt::publish(char* publishtopic, int Qos, char* payload)
{
	int res = 0;
	iotx_mqtt_topic_info_t topic_msg;

	topic_msg.qos = Qos;
	topic_msg.retain = 0;
	topic_msg.dup = 0;
	topic_msg.payload = (char*)payload;
	topic_msg.payload_len = strlen(payload);

	res = IOT_MQTT_Publish(pclient, publishtopic, &topic_msg);
	if (res < 0) {
		printf("publish failed\n");
		return -1;
	}
	cout << "publish success,payload is : " << payload << endl;
	return 0;
}
void MyAliyunMqtt::event_handle(void* pcontext, void* pclient, iotx_mqtt_event_msg_pt msg)
{
	printf("msg->event_type : %d\n", msg->event_type);
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

}
int MyAliyunMqtt::MqttInit(char* host, int port, char* clientid, char* username, char* password)
{
	iotx_mqtt_param_t       mqtt_params;

	memset(&mqtt_params, 0x0, sizeof(mqtt_params));

	mqtt_params.write_buf_size = 1024;
	mqtt_params.read_buf_size = 1024;
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

	pclient = IOT_MQTT_Construct(&mqtt_params);
	if (NULL == pclient) {
		printf("MQTT construct failed");
		return -1;
	}
	return 0;
}
MyAliyunMqtt::MyAliyunMqtt()
{
}
int MyAliyunMqtt::MqttInterval(void* Params)
{
	MyAliyunMqtt* point = (MyAliyunMqtt*)Params;
	while (1)
	{
		IOT_MQTT_Yield((point->pclient), 200);
	}
	return 0;
}
int MyAliyunMqtt::MqttRecParse(void* Params)
{
	MyAliyunMqtt* point = (MyAliyunMqtt*)Params;
	double buff;
	Serial s;
	while (1)
	{
		if (point->getmessage == true)
		{
			cJSON* json, * json_params;
			json = cJSON_Parse(point->payload);

			json_params = cJSON_GetObjectItem(json, "params");
			if (json_params)
			{
				char* varname = json_params->child->string;
				double value = json_params->child->valuedouble;
				modbus_set(1, value, varname, &buff);
			}
			else if ((json_params = cJSON_GetObjectItem(json, "COM")))
			{
				int com = json_params->valueint;
				char dev[100];
				char dest[1024];
				char readbuff[1024];
#ifndef gcc
				int gpio = PortInfo[getportinfosubscript(PortInfo, com)].gpio;
				if(gpio != -1)
				{
					char buff[100];
					sprintf(buff, "%s%d%s", "echo ", gpio, " > /sys/class/gpio/export");
					system(buff);
					sprintf(buff, "%s%d%s", "echo out > /sys/class/gpio/gpio", gpio, "/direction");
					system(buff);
					sprintf(buff, "%s%d%s", "echo 1 > /sys/class/gpio/gpio", gpio, "/value");
					system(buff);
				}
#endif
				cJSON* json_payload;

				json_payload = cJSON_GetObjectItem(json, "payload");
				int ComIsResp = cJSON_GetObjectItem(json, "ComIsResp")->valueint;
				int serialtimeout = 0;
				if (ComIsResp)
					serialtimeout = cJSON_GetObjectItem(json, "ComTimeoutMs")->valueint;
				sprintf(dev, "%s%d", "/dev/ttyS", com);

				int len = Base64decode(dest, json_payload->valuestring);

				s.open(dev, 115200, 8, 'N', 1);

				s.write(dest, len);
#ifndef gcc
				if(gpio != -1)
				{
					char buff[100];
					sprintf(buff, "%s%d%s", "echo 0 > /sys/class/gpio/gpio", gpio, "/value");
					system(buff);
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
					sprintf(payload, "%s%d %s", "Com", com, "Compelete");
				}
				else
					strcpy(payload, dest);

				if (seriallen == 0)
				{
					sprintf(payload, "%s%d %s", "Com", com, "Timeout");
				}

				cJSON_AddStringToObject(response, "Time", tmp);
				cJSON_AddStringToObject(response, "payload", payload);
				point->publish(ThemeCtrl[0].CtrlPub, 1, cJSON_PrintUnformatted(response));
				cJSON_Delete(response);
			}
			else
			{
				exit(-1);
			}
			cJSON_Delete(json);
			point->getmessage = false;
		}
		usleep(10000);
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
int MyAliyunMqtt::MqttMain(void* Params)
{
#ifndef gcc
	for (int i = 0; i < PortInfo[0].portcount; i++)
	{
		int gpio = PortInfo[i].gpio;
		if(gpio != -1)
		{
			char buff[100];
			sprintf(buff, "%s%d%s", "echo ", PortInfo[i].gpio, " > /sys/class/gpio/export");
			system(buff);
			sprintf(buff, "%s%d%s", "echo out > /sys/class/gpio/gpio", PortInfo[i].gpio, "/direction");
			system(buff);
			sprintf(buff, "%s%d%s", "echo 1 > /sys/class/gpio/gpio", PortInfo[i].gpio, "/value");
			system(buff);
		}
	}
#endif
	MyAliyunMqtt* point = (MyAliyunMqtt*)Params;

	int loop_cnt = 0;

	point->MqttInit(MqttInfo[0].ServerLink, MqttInfo[0].ServerPort, MqttInfo[0].ClientId, MqttInfo[0].UserName, MqttInfo[0].Password);
	int res = point->subscribe(ThemeCtrl[0].CtrlSub, 1);
	if (res < 0) {
		IOT_MQTT_Destroy(&(point->pclient));
		return -1;
	}

	point->openintervalthread();		//create MQTT interval thread
	point->openrecparsethread();		//create thread----parse the receive data
	while (1)
	{
		if(!ThemeUploadList[0].UploadCount)
			continue;
		time_t nowtime;
		nowtime = time(NULL); //get now time
		char tmp[64];
		strftime(tmp, sizeof(tmp), "%Y%m%d%H%M%S", localtime(&nowtime));

		cJSON* publish_json, * params_json;

		publish_json = cJSON_CreateObject();

		cJSON_AddStringToObject(publish_json, "id", tmp);
		cJSON_AddStringToObject(publish_json, "method", "method.event.property.post");
		cJSON_AddItemToObject(publish_json, "params", params_json = cJSON_CreateObject());

		double temp;
		for (int i = 0; i < ThemeUploadList[0].UploadCount; i++)
		{
			cout << "var name is :"  << endl;
			cout << ThemeUploadList[i].VarName << endl;
			modbus_set(0, 0, ThemeUploadList[i].VarName, &temp);
			cJSON_AddNumberToObject(params_json, ThemeUploadList[i].VarName, temp);
		}
		char* payload = cJSON_PrintUnformatted(publish_json);
		point->publish(ThemeUpload[0].CtrlPub, ThemeUpload[0].QosPub, payload);

		sleep(ThemeUpload[0].PubPeriod);

		loop_cnt += 1;
	}
}
