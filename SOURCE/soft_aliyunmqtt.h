#pragma once
#ifndef _SOFT_ALIYUNMQTT_H
#define _SOFT_ALIYUNMQTT_H_

#include "thread"
//#include "mqtt_api.h"
#include "sdk_include.h"
#include "cJSON/cJSON.h"
extern "C" {
	int     LITE_get_loglevel(void);
	void LITE_set_loglevel(int);
}

class MyAliyunMqtt {
public:
	MyAliyunMqtt();
	void* MqttInit(char* host, int port, char* clientid, char* username, char* password);

	~MyAliyunMqtt();

	int subscribe(void* client,char* subscribetopic, int Qos);
	int publish_cjson(void* client,char* publishtopic, int Qos, cJSON* payload);
	int publish(void* client,char* publishtopic, int Qos, char* payload);
	int openmainthread();
	int openintervalthread();
	int openrecparsethread();
private:
	static void event_handle(void* pcontext, void* pclient, iotx_mqtt_event_msg_pt msg);
	static void message_arrive(void* pcontext, void* pclient, iotx_mqtt_event_msg_pt msg);

	std::thread threadid;
	std::thread threadid_interval;
	std::thread threadid_recparse;

	static int MqttMain(void* Param);
	static int MqttInterval(void* Param);
	static int MqttRecParse(void* Param);
};
class MqttModbus :public MyAliyunMqtt {

};
#endif
